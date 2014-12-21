
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
  const char *src = "{\"name\": \"bradley\"}";
  json_value_t *obj = NULL;
  json_value_t *tmp = NULL;
  json_value_t *value = NULL;

  obj = json_parse(name, src);
  assert(obj);
  assert(0 == obj->errno);
  ok("json_parse");

  value = json_get(obj, "name");
  assert(value);
  assert(EQ("name", value->id));
  assert(EQ("bradley", value->as.string));

  tmp = json_new(JSON_STRING, "joseph");
  assert(tmp);
  ok("json_new");

  json_set(obj, "name", tmp);
  assert(0 == obj->errno);

  ok_done();
  return 0;
}
