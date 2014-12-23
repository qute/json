
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <qute/qute.h>
#include <fs/fs.h>
#include <buffer/buffer.h>

#include "json.h"

#define EQ(a, b) (0 == strcmp(a, b))

static int stream = 0;
static int each = 0;
static int tty = 0;

static char *
read_stdin () {
  buffer_t *buf = buffer_new();
  char *ret = NULL;
  char *tmp = NULL;
  size_t size = 0;

  if (NULL == buf) { return NULL; }

  // read
  while (getline(&tmp, &size, stdin) > 0) {
    tmp[size] = '\0';
    buffer_append(buf, tmp);

    if (stream) {
      break;
    }
  }

  ret = buf->data;
  return ret;
}

int
main (int argc, char **argv) {
  json_value_t *value = NULL;
  json_value_t *root = NULL;
  char *filename = NULL;
  char *key = NULL;
  char *src = NULL;
  int i = 0;

#define usage() printf("usage: %s <file> [key]\n", argv[0]);

  if (argc < 2) {
    usage();
    return 1;
  }

  // parse opts
  for (i = 1; i < argc; ++i) {
    if (EQ(argv[i], "--each")) {
      each = 1;
    } else if (EQ(argv[i], "--stream")) {
      stream = 1;
    } else if (EQ(argv[i], "--help")) {
      usage();
      return 0;
    }
  }

  if (ferror(stdin)) {
    return 1;
  } else if (0 == isatty(0)) {
    filename = "<stdin>";
    tty = 1;
    if (argc > 1 && '-' != argv[1][0]) {
      key = argv[1];
    }
  } else {
    filename = argv[1];
    if (0 != fs_exists(filename)) {
      printf("E: not found - `%s'\n", filename);
      return 1;
    }

    src = fs_read(filename);
    if (NULL == src) {
      return 1;
    }

    if (argc > 2 && '-' !=  argv[2][0]) {
      key = argv[2];
    }
  }

  do {
    if (tty) {
      src = read_stdin();
    }

    if (NULL == src || 0 == strlen(src)) {
      break;
    }

    // proxy source if just streaming stdin
    // without a key lookup
    if (NULL == key && stream) {
      printf("%s", src);
      continue;
    }

    root = json_parse(filename, src);

    if (NULL == root) {
      return 1;
    } else if (root->errno) {
      json_perror(root);
      return 1;
    }

    if (NULL != key) {
      value = json_get(root, key);
      if (NULL == value) {
        return 1;
      }

      free(root);
      root = value;
    } else {
      value = root;
    }

    if (1 == each && JSON_ARRAY == value->type) {
      value = value->values[0];
      while (value) {
        printf("%s\n", json_stringify(value));
        value = value->next;
      }
    } else {
      printf("%s\n", json_stringify(value));
    }

    json_destroy(root);
    free(src);
    value = NULL;
    root = NULL;
    src = NULL;

  } while (tty);

  return 0;
}
