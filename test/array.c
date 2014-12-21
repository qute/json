
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
  assert(arr->size);
  ok("json_parse");

  value = json_get(arr, "0");
  assert(value);
  assert(EQ("kinkajou", value->as.string));
  ok("json_get");

  value = json_get(arr, "1");
  assert(value);
  assert(EQ("bradley", value->as.string));
  ok("json_get");

  value = json_get(arr, "2");
  assert(value);
  assert(EQ("4", value->as.string));
  assert(4 == (int) value->as.number);
  ok("json_get");

  value = json_get(arr, "0");
  assert(value);
  ok("json_get");

  json_destroy(value);
  assert(0 == value->errno);
  assert(2 == arr->size);
  value = NULL;
  ok("json_destroy");

  value = json_get(arr, "0");
  assert(value);
  ok("json_get");
  assert(EQ("bradley", value->as.string));

  json_destroy(value);
  assert(0 == value->errno);
  assert(1 == arr->size);
  value = NULL;
  ok("json_destroy");

  value = json_get(arr, "0");
  assert(value);
  ok("json_get");
  assert(EQ("4", value->as.string));
  assert(4 == (int) value->as.number);

  json_destroy(value);
  assert(0 == value->errno);
  assert(0 == arr->size);
  value = NULL;
  ok("json_destroy");

  ok_done();
  return 0;
}
