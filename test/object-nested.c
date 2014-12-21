
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
  const char *file = "test/fixtures/object-nested.json";
  const char *src = NULL;
  json_value_t *obj = NULL;
  json_value_t *value = NULL;

  src = fs_read(file);
  assert(src);

  obj = json_parse(file, src);
  assert(obj);

  value = json_get(obj, "a");
  assert(value);
  assert(JSON_OBJECT == value->type);
  ok("json_get");

  value = json_get(value, "b");
  assert(value);
  assert(JSON_OBJECT == value->type);
  ok("json_get");

  value = json_get(value, "c");
  assert(value);
  assert(JSON_NUMBER == value->type);
  assert(EQ("123", value->as.string));
  assert(123 == (int) value->as.number);

  ok_done();
  return 0;
}
