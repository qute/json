
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
  const char *file = "test/fixtures/object.json";
  const char *src = NULL;
  json_value_t *obj = NULL;
  json_value_t *value = NULL;

  src = fs_read(file);
  assert(src);

  printf("%s\n", src);

  obj = json_parse(file, src);
  assert(obj);

  value = json_get(obj, "key");
  assert(value);
  assert(EQ("key", value->id));
  assert(EQ("foo", value->as.string));
  ok("json_get");

  json_destroy(value);
  assert(1 == obj->size);
  ok("json_destroy");

  value = json_get(obj, "beep");
  assert(value);
  assert(EQ("beep", value->id));
  assert(EQ("boop", value->as.string));
  ok("json_get");

  json_destroy(value);
  assert(0 == obj->size);
  ok("json_destroy");

  ok("object");
  ok_done();
  return 0;
}
