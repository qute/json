
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
  const char *file = "test/fixtures/invalid-array.json";
  const char *src = NULL;
  json_value_t *obj = NULL;
  json_value_t *value = NULL;

  src = fs_read(file);
  assert(src);

  obj = json_parse(file, src);
  assert(obj);

  assert(EJSON_TOKEN == obj->errno);

  ok_done();
  return 0;
}
