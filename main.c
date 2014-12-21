
#include <qute.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fs/fs.h>

#include "json.h"

static char *
read_stdin () {
  size_t bsize = 1024;
  size_t size = 1;
  char buf[bsize];
  char *res = (char *) malloc(sizeof(char));
  char *tmp = NULL;

  // memory issue
  if (NULL == res) { return NULL; }

  // cap
  res[0] = '\0';

  // read
  while (NULL != fgets(buf, bsize, stdin)) {
    // resize
    size += (size_t) strlen(buf);

    // realloc
    tmp = (char *) realloc(res, size);

    // memory issues
    if (NULL == tmp) {
      free(res);
      return NULL;
    }

    // reset
    res = tmp;

    // yield
    strcat(res, buf);

  }

  return res;
}

int
main (int argc, char **argv) {
  json_value_t *value = NULL;
  json_value_t *root = NULL;
  char *filename = NULL;
  char *key = NULL;
  char *src = NULL;

  if (argc < 2) {
    printf("usage: %s <file> [key]\n", argv[0]);
    return 1;
  }

  if (ferror(stdin)) {
    return 1;
  } else if (0 == isatty(0)) {
    filename = "<stdin>";
    src = read_stdin();
    if (NULL == src) { return 1; }
    if (argc > 1) {
      key = argv[1];
    }
  } else {
    filename = argv[1];
    if (0 != fs_exists(filename)) {
      printf("E: not found - `%s'\n", filename);
      return 1;
    }
    src = fs_read(filename);
    if (argc > 2) {
      key = argv[2];
    }
  }

  if (NULL == src) {
    return 1;
  }

  root = json_parse(filename, src);

  if (NULL == root) {
    return 1;
  }

  if (root->errno) {
    json_perror(root);
    return 1;
  }

  if (NULL != key) {
    value = json_get(root, key);
    if (NULL == value) {
      return 1;
    }

    printf("%s\n", json_stringify(value));
  }

  json_destroy(root);
  root = NULL;

  return 0;
}
