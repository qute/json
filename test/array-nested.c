
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
  const char *file = "test/fixtures/array-nested.json";
  const char *src = NULL;
  json_value_t *value = NULL;
  json_value_t *arr = NULL;
  json_value_t *row = NULL;

  src = fs_read(file);
  assert(src);

  arr = json_parse(file, src);
  assert(arr);
  assert(arr->size);
  ok("json_parse");

  {
    row = json_get(arr, "0");
    assert(row);
    assert(JSON_ARRAY == row->type);
    ok("json_get");

    value = json_get(row, "0");
    assert(value);
    assert(0 == value->as.number);
    assert(EQ("0", value->as.string));
    ok("json_get");

    value = json_get(row, "1");
    assert(value);
    assert(1 == value->as.number);
    assert(EQ("1", value->as.string));
    ok("json_get");

    value = json_get(row, "2");
    assert(value);
    assert(2 == value->as.number);
    assert(EQ("2", value->as.string));
    ok("json_get");
  }

  {
    row = json_get(arr, "1");
    assert(row);
    assert(JSON_ARRAY == row->type);
    ok("json_get");

    value = json_get(row, "0");
    assert(value);
    assert(EQ("a", value->as.string));
    ok("json_get");

    value = json_get(row, "1");
    assert(value);
    assert(EQ("b", value->as.string));
    ok("json_get");

    value = json_get(row, "2");
    assert(value);
    assert(EQ("c", value->as.string));
    ok("json_get");
  }

  {
    row = json_get(arr, "2");
    assert(row);
    assert(JSON_ARRAY == row->type);
    ok("json_get");

    value = json_get(row, "0");
    assert(value);
    assert(JSON_ARRAY == value->type);

    value = json_get(value, "0");
    assert(value);
    assert(0 == value->as.number);
    assert(EQ("0", value->as.string));
    ok("json_get");

    value = json_get(row, "1");
    assert(value);
    assert(JSON_ARRAY == value->type);

    value = json_get(value, "0");
    assert(value);
    assert(1 == value->as.number);
    assert(EQ("1", value->as.string));
    ok("json_get");

    value = json_get(row, "2");
    assert(value);
    assert(JSON_ARRAY == value->type);

    value = json_get(value, "0");
    assert(value);
    assert(2 == value->as.number);
    assert(EQ("2", value->as.string));
    ok("json_get");
  }

  json_destroy(arr);
  assert(0 == arr->size);
  ok_done();
  return 0;
}
