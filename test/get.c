
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
  const char *name = "test set";
  const char *src = "{\"type\": \"kinkajou\"}";
  json_value_t *obj = NULL;
  json_value_t *tmp = NULL;
  json_value_t *value = NULL;

  obj = json_parse(name, src);
  assert(obj);
  assert(0 == obj->errno);
  ok("json_parse");

  value = json_get(obj, "type");
  assert(value);
  assert(EQ("type", value->id));
  assert(EQ("kinkajou", value->as.string));

  ok_done();
  return 0;
}
