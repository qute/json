
#include <qute.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fs/fs.h>

#include "json.h"

#define EQ(a, b) (0 == strcmp(a, b))
#define peek(p) p->nodes[p->cursor]
#define next(p) p->nodes[p->cursor++]

#ifndef DEBUG
#define DEBUG 0
#else
#undef DEBUG
#define DEBUG 1
#endif

#define enter(scope) DEBUG && printf("> enter: %s\n", scope);
#define leave(scope) DEBUG && printf("< leave: %s\n", scope);

static void
push (json_value_t *scope, json_value_t *value) {
  value->index = 0;

  if (scope->size > 0) {
    value->prev = scope->values[scope->size -1];
    value->prev->next = value;
    value->index = value->prev->index +1;
  }

  scope->values[scope->size++] = value;
}

json_value_t *
json_new (int type, const char *string) {
  json_value_t *value = (json_value_t *) malloc(sizeof(json_value_t));
  if (NULL == value) { return NULL; }
  value->type = type;
  value->as.string = string;
  return value;
}

void
json_destroy (json_value_t *value) {
  json_value_t *child = NULL;
  json_value_t *tmp = NULL;

  if (NULL == value) {
    return;
  } else if (value->prev) {
    if (value->next) {
      value->next->prev = value->prev;
      value->prev->next = value->next;
    } else {
      value->prev->next = NULL;
    }
  } else if (value->next){
    value->next->prev = NULL;
  }

  if (value->parent) {
    value->parent->values[value->index] = NULL;
  }

  if (value->next && value->parent) {
    tmp = value->next;
    while (tmp) {
      tmp->index--;
      if (tmp->parent) {
        tmp->parent->values[tmp->index] = tmp;
      }
      tmp = tmp->next;
    }
  }

  child = value->values[0];
  while (child) {
    json_destroy(child);
    child = child->next;
  }

  if (value->parent) {
    if (value->parent->size > 0) {
      value->parent->size--;
    }
  }

  free(value);
  value = NULL;
}

void
json_perror (json_value_t *value) {
  char *name = NULL;
  switch (value->errno) {
    case EJSON_OK:
      name = "Okay";
      break;

    case EJSON_MEM:
      name = "Out of memory";
      break;

    case EJSON_PARSE:
      name = "Parser error";
      break;

    case EJSON_TOKEN:
      name = "Unexpected token";
      break;

    case EJSON_PARSERMEM:
      name = "Parser out of memory";
      break;
  }

  fprintf(stderr, "%s: %s\n\tat %s:%d:%d",
      name, value->current->next->as.string,
      value->current->id,
      value->current->token.lineno,
      value->current->token.colno);
}

json_value_t *
json_parse (const char *filename, const char *src) {
  q_node_block_t *block = NULL;
  json_value_t *scope = NULL;
  json_value_t *value = NULL;
  json_value_t *root = NULL;
  q_parser_t parser;
  q_node_t *node = NULL;
  const char *key = NULL;
  int expecting = 0;
  int in_array = 0;
  int rc = 0;

  // alloc
  if (NULL == q_node_alloc(block, block)) {
    return NULL;
  }

  // init
  if ((rc = q_parser_init(&parser, filename, src)) > 0) {
    free(block);
    return NULL;
  }

  // parse
  if ((rc = q_parse(&parser, block)) > 0) {
    free(block);
    return NULL;
  }

parse:
  do {
    node = next(block);
    if (NULL == node) { break; }
    if (root) {
      root->current = node;
    }
    switch (node->type) {
      case QNODE_BLOCK:
      case QNODE_NULL:
        break;

      case QNODE_IDENTIFIER:
        enter("identifer");
        value->truthy = 1;

        if (EQ("null", node->as.string)) {
          value = json_new(JSON_NULL, "null");
          value->truthy = 0;
        } else if (EQ("true", node->as.string)) {
          value = json_new(JSON_BOOLEAN, "true");
        } else if (EQ("false", node->as.string)) {
          value = json_new(JSON_BOOLEAN, "false");
          value->truthy = 0;
        } else {
          root->errno = EJSON_TOKEN;
          goto error;
        }
        break;

      case QNODE_STRING:
        enter("string");
        if (!scope->arraylike && NULL == key) {
          key = node->as.string;
          if (QNODE_OPERATOR != node->next->type ||
              ':' != node->next->as.string[0]) {
            root->errno = EJSON_TOKEN;
            goto error;

          }
          goto parse;
        }

        if (!node->prev) {
          root->errno = EJSON_TOKEN;
          goto error;
        }

        if (node->next &&
            (',' != node->next->as.string[0] &&
             '}' != node->next->as.string[0] &&
             ']' != node->next->as.string[0])) {
          root->errno = EJSON_TOKEN;
          goto error;
        }

        value = json_new(JSON_STRING, node->as.string);
        break;

      case QNODE_NUMBER:
        enter("number");
        value = json_new(JSON_NUMBER, node->as.string);
        value->as.string = node->as.string;
        value->as.number = node->as.number;

        if (!node->prev) {
          root->errno = EJSON_TOKEN;
          goto error;
        }

        if (node->next &&
            (',' != node->next->as.string[0] &&
             '}' != node->next->as.string[0] &&
             ']' != node->next->as.string[0])) {
          root->errno = EJSON_TOKEN;
          goto error;
        }

        break;

      case QNODE_TOKEN:
        if ('{' == node->as.string[0]) {
          enter("object");
          value = json_new(JSON_OBJECT, NULL);
          value->arraylike = 0;
          value->as.string = "[Object]";
          if (NULL == root) {
            root = value;
            scope = value;
            root->id = filename;
            goto parse;
          }
        } else if ('[' == node->as.string[0]) {
          enter("array");
          value = json_new(JSON_ARRAY, NULL);
          value->arraylike = 1;
          value->as.string = "[Array]";
          if (NULL == root) {
            root = value;
            scope = value;
            root->id = filename;
            goto parse;
          }

        } else if ('}' == node->as.string[0]) {
          leave("object");
          if (node->next &&
              (',' != node->next->as.string[0] &&
               '}' != node->next->as.string[0] &&
               ']' != node->next->as.string[0])) {
            root->errno = EJSON_TOKEN;
            goto error;
          } else if (scope->parent) {
            scope = scope->parent;
          }
          goto parse;
        } else if (']' == node->as.string[0]) {
          leave("array");
          if (node->next &&
              (',' != node->next->as.string[0] &&
               '}' != node->next->as.string[0] &&
               ']' != node->next->as.string[0])) {
            root->errno = EJSON_TOKEN;
            goto error;
          } else if (scope->parent) {
            scope = scope->parent;
          }
          goto parse;
        } else if(',' == node->as.string[0]) {
          if (',' == node->next->as.string[0] ||
              ':' == node->next->as.string[0] ||
              ':' == node->prev->as.string[0]) {
            root->errno = EJSON_TOKEN;
            goto error;
          }
          goto parse;
        } else {
          root->errno = EJSON_TOKEN;
          goto error;
        }

        if (scope->arraylike) {
          value->id = scope->id;
        } else {
          value->id = key;
        }

        push(scope, value);
        value->parent = scope;
        scope = value;

        key = NULL;
        goto parse;

      case QNODE_OPERATOR:
        enter("operator");
        if (':' == node->as.string[0]) {
          if (!node->prev) {
            root->errno = EJSON_TOKEN;
            goto error;
          }
        }
        goto parse;
    }

    if (scope->arraylike && !value->arraylike) {
      if (node->next && QNODE_TOKEN != node->next->type) {
        root->errno = EJSON_TOKEN;
        goto error;
      }
    }

    if (scope->arraylike) {
      value->id = scope->id;
    } else {
      value->id = key;
    }

    push(scope, value);
    value->parent = scope;
    key = NULL;

  } while (node);

goto done;


error:
  // @TODO: handle error

done:
free(block);
return root;
}

char *
json_stringify (json_value_t *root) {
  json_value_t *value = NULL;
  char string[BUFSIZ];
  int type = root->type;
  int i = 0;

  memset(string, 0, sizeof(string));

  // scalar root types
  switch (type) {
    case JSON_NULL:
    case JSON_STRING:
    case JSON_NUMBER:
    case JSON_BOOLEAN:
      return (char *) root->as.string;

    case JSON_ARRAY:
      strcat(string, "[");
      break;

    case JSON_OBJECT:
      strcat(string, "{");
      break;
  }

  value = root->values[0];
  while (value) {
    char out[BUFSIZ];

    memset(out, 0, sizeof(out));

    switch (value->type) {
      case JSON_NUMBER:
        if (value->parent && value->parent->arraylike) {
          if (value->as.number == (int) value->as.number) {
            sprintf(out, "%d", (int) value->as.number);
          } else {
            sprintf(out, "%2.f", value->as.number);
          }
        } else {
          // int
          if (value->as.number == (int) value->as.number) {
            sprintf(out, "\"%s\":%d", value->id, (int) value->as.number);
          } else {
            sprintf(out, "\"%s\":%2.f", value->id, value->as.number);
          }
        }
        break;

      case JSON_NULL:
      case JSON_BOOLEAN:
        if (value->parent && value->parent->arraylike) {
          sprintf(out, "%s", value->as.string);
        } else {
          sprintf(out, "\"%s\":%s", value->id, value->as.string);
        }
        break;

      case JSON_STRING:
        if (value->parent && value->parent->arraylike) {
          sprintf(out, "\"%s\"", value->as.string);
        } else {
          sprintf(out, "\"%s\":\"%s\"", value->id, value->as.string);
        }
        break;

      case JSON_OBJECT:
      case JSON_ARRAY:
        if (value->parent && value->parent->arraylike) {
          sprintf(out, "%s", json_stringify(value));
        } else {
          sprintf(out, "\"%s\":%s", value->id, json_stringify(value));
        }
        break;
    }

    strcat(string, out);
    if (value->next) {
      strcat(string, ",");
    }

    value = value->next;
  }

  if (JSON_ARRAY == type) {
    strcat(string, "]");
  } else if (JSON_OBJECT == type) {
    strcat(string, "}");
  } else {
    return NULL;
  }

  return strdup(string);
}

json_value_t *
json_get (json_value_t *root, const char *id) {
  json_value_t *value = root->values[0];
  while (value) {
    char index[BUFSIZ];
    if (JSON_OBJECT == root->type) {
      if (EQ(value->id, id)) {
        return value;
      }
    } else if (JSON_ARRAY == root->type) {
      memset(index, 0, sizeof(index));
      sprintf(index, "%d", value->index);
      if (EQ(index, id)) {
        return value;
      }
    } else {
      return NULL;
    }

    value = value->next;
  }
  return NULL;
}

void
json_set (json_value_t *root, const char *id, json_value_t *value) {
  json_value_t *existing = json_get(root, id);
  if (NULL != existing) {
    existing->as.string = value->as.string;
    existing->as.number = value->as.number;
    existing->type = value->type;
  } else {
    value->id = id;
    push(root, value);
  }
}
