
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <ok/ok.h>
#include <json.h>

#define EQ(a, b) (0 == strcmp(a, b))

int
main (void) {
  json_value_t *obj = json_new(JSON_OBJECT, "[object]");
  json_value_t *name = json_new(JSON_STRING, "joseph");
  assert(obj);
  ok("json_new");

  char *src = json_stringify(obj);
  ok("json_stringify");
  assert(src);

  json_destroy(obj);
  assert(0 == obj->size);
  ok("json_destroy");
  obj = NULL;

  ok_done();
  return 0;
}
