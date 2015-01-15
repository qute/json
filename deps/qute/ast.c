
/**
 * `ast.c' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strdup/strdup.h"
#include "qute.h"

int
q_node_init (q_node_t *self) {
  if (NULL == self) {
    return QE_ASTNODEMEM;
  }

  self->id = "";
  self->type = QNODE_NULL;
  self->prev = NULL;
  self->next = NULL;
  self->as.number = 0.0;
  self->as.string = "";

  return 0;
}

int
q_node_hex_init (q_node_hex_t *self, const char *hex) {
  int rc = q_node_init((q_node_t *) self);

  if (rc > 0) {
    return rc;
  }

  if (NULL == self) {
    return QE_NODE_NULL;
  }

  self->type = QNODE_HEX;
  self->as.string = hex;

  return 0;
}

int
q_node_block_init (q_node_block_t *self) {
  int rc = q_node_init((q_node_t *) self);

  if (rc > 0) {
    return rc;
  }

  if (NULL == self) {
    return QE_NODE_NULL;
  }

  self->type = QNODE_BLOCK;
  self->length = 0;

  return 0;
}

int
q_node_string_init (q_node_string_t *self, const char *string) {
  int rc = q_node_init((q_node_t *) self);

  if (rc > 0) {
    return rc;
  }

  if (NULL == self) {
    return QE_NODE_NULL;
  }

  self->type = QNODE_STRING;
  if (NULL != string) {
    self->length = strlen(string);
    self->as.string = string;
  }

  return 0;
}

int
q_node_number_init (q_node_number_t *self, float number) {
  int rc = q_node_init((q_node_t *) self);
  char str[BUFSIZ];

  if (rc > 0) {
    return rc;
  }

  if (NULL == self) {
    return QE_NODE_NULL;
  }

  sprintf(str, "%f", number);

  self->type = QNODE_NUMBER;
  self->as.number = number;
  self->as.string = strdup(str);

  return 0;
}

/**
 * initializes parser operator node.
 */

int
q_node_operator_init (q_node_operator_t *self) {
  int rc = q_node_init((q_node_t *) self);

  if (rc > 0) {
    return rc;
  }

  self->type = QNODE_OPERATOR;

  return 0;
}

/**
 * initializes parser identifier node.
 */

int
q_node_identifier_init (q_node_identifier_t *self) {
  int rc = q_node_init((q_node_t *) self);

  if (rc > 0) {
    return rc;
  }

  self->type = QNODE_IDENTIFIER;
  return 0;
}

int
q_node_token_init (q_node_token_t *self) {
  int rc = q_node_init((q_node_t *) self);

  if (rc > 0) {
    return rc;
  }

  self->type = QNODE_TOKEN;

  return 0;
}

int
q_node_block_push (q_node_block_t *self, q_node_t *node) {
  if (self->length < QMAX_BLOCK_NODES) {

    // push
    self->nodes[self->length++] = node;

    // link
    if (self->length > 1) {
      self->nodes[self->length -2]->next = node;
      node->prev = self->nodes[self->length -2];
    }

    return self->length;
  }

  return QE_OVERFLOW;
}
