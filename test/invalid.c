
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <ok/ok.h>
#include <fs/fs.h>
#include <json.h>

#define EQ(a, b) (0 == strcmp(a, b))

void
test_file_fails(const char * name) {
  char *path = "test/fixtures/";
  char *src = NULL;
  char file[strlen(path) + strlen(name)];
  sprintf(file, "%s/%s", path, name);
  json_value_t *obj = NULL;
  json_value_t *value = NULL;
  src = fs_read(file);
  assert(src);
  obj = json_parse(file, src);
  assert(obj);
  assert(EJSON_TOKEN == obj->errno);
}

void
test_invalid_scalar () {
  test_file_fails("invalid-scalar.json");
}

void
test_invalid_array () {
  test_file_fails("invalid-array.json");
}

int
main (void) {
  test_invalid_scalar();
  test_invalid_array();

  ok_done();
  return 0;
}
