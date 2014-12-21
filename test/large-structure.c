
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
  const char *file = "test/fixtures/large-structure.json";
  const char *src = NULL;
  json_value_t *value = NULL;
  json_value_t *arr = NULL;
  json_value_t *obj = NULL;

  src = fs_read(file);
  assert(src);

  arr = json_parse(file, src);
  assert(arr);
  assert(arr->size);

  obj = json_get(arr, "0");
  assert(obj);

  value = json_get(obj, "_id");
  assert(value);
  assert(EQ("549754844fc63f4e60cf27b0", value->as.string));

  value = json_get(obj, "friends");
  assert(value);
  assert(JSON_ARRAY == value->type);

  value = json_get(value, "0");
  assert(value);
  assert(JSON_OBJECT == value->type);

  value = json_get(value, "name");
  assert(value);
  assert(EQ("Cora Burns", value->as.string));

  ok_done();
  return 0;
}
