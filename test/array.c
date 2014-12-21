
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <ok/ok.h>
#include <fs/fs.h>
#include <json.h>

#define EQ(a, b) (0 == strcmp(a, b))

int
main (void) {
  const char *file = "test/fixtures/array.json";
  const char *src = NULL;
  json_value_t *arr = NULL;
  json_value_t *value = NULL;

  src = fs_read(file);
  assert(src);

  arr = json_parse(file, src);
  assert(arr);

  return 0;
}
