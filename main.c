
#include <qute.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fs/fs.h>

#include "json.h"

int
main (int argc, char **argv) {
  char *filename = NULL;
  char *string = NULL;
  char *src = NULL;

  if (argc < 2) {
    printf("usage: %s <file>\n", argv[0]);
    return 1;
  }

  filename = argv[1];

  if (0 != fs_exists(filename)) {
    printf("E: not found - `%s'\n", filename);
    return 1;
  }

  src = fs_read(filename);

  if (NULL == src) {
    return 1;
  }

  json_value_t *object = json_parse(filename, src);

  if (NULL == object) {
    return 1;
  }

  if (object->errno) {
    json_perror(object);
    return 1;
  }

  json_destroy(object);
  object = NULL;

  return 0;
}
