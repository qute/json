
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <ok/ok.h>
#include <json.h>

#define EQ(a, b) (0 == strcmp(a, b))

int
main (void) {
  const char *file = "parse test";
  const char *src = "{\"name\": \"Joseph\", \"nil\": null, \"age\": 24}";
  json_value_t *obj = NULL;
  json_value_t *value = NULL;

  obj = json_parse(file, src);
  ok("json_parse");
  assert(obj);

  json_destroy(obj);
  ok("json_destroy");
  assert(0 == obj->size);
  obj = NULL;

  ok_done();
  return 0;
}
