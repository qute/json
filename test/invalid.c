
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <ok/ok.h>
#include <fs/fs.h>
#include <json.h>

#define EQ(a, b) (0 == strcmp(a, b))

void
test_file_fails(char * name) {
  printf("test=(%s)\n", name);
  char *path = "test/fixtures/";
  char *src = NULL;
  char file[strlen(path) + strlen(name)];
  sprintf(file, "%s/%s", path, name);
  json_value_t *obj = NULL;
  src = fs_read(file);
  assert(src);
  obj = json_parse(file, src);

  if (NULL == obj) {
    printf("\n\n\tFATAL PARSE FAILURE\n\n");
    exit(1);
  }

  assert(obj);
  assert(EJSON_TOKEN == obj->errno);

  json_destroy(obj);
  ok(name);
}

int
main (void) {
  test_file_fails("invalid-array.json");

  test_file_fails("invalid-object.json");
  test_file_fails("invalid-object-key.json");
  test_file_fails("invalid-object-value.json");

  test_file_fails("invalid-scalar.json");
  test_file_fails("invalid-string.json");
  test_file_fails("invalid-string-quotes.json");

  ok_done();
  return 0;
}
