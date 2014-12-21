json
=========

JSON parser and serializer built on qute

## install

**As a [clib](https://github.com/clibs/clib) package:**

```sh
$ clib install qute/json
```

**As a system library and executable:**

With [ghi](https://github.com/stephenmathieson/ghi):

```sh
$ ghi install qute/json
```

**Or from source:**

```sh
$ make
$ make install
```

## usage

```c
include <json.h>
```

or

```c
include <json/json.h>
```

```c
const char *filename = "data.json";
const char *src = fs_read("/path/to/data.json"); // {"key":"foo"}
json_value_t *root = json_parse(filename, src);
json_value_t *value = json_get(root, "key");
printf("%s = %s", value->id, value->as.string); // key = foo
```

## api

### values

```c
typedef struct json_value json_value_t;
```

All valus are a `json_value_t`. It is a simple type that represents all
types in JSON. It is also a node in a doubly linked list like structure.

```c
struct json_value {
  int type;
  int truthy;
  int errno;
  int arraylike;
  size_t size;
  const char *id;
  q_node_t *current;
  struct json_value *next;
  struct json_value *prev;
  struct json_value *parent;
  struct json_value *values[JSON_MAX_VALUES];
  struct {
    const char *string;
    float number;
  } as;
} json_value_t;
```

Every value can have children. They are present on the `.values` member
property. However, scalar values like `true`, `null`, `123`,  and `"abc"`
cannot. The member still exists on those values but the parser will not
populate it as only an object (`{}`) or an array (`[]`) can have child
values.

#### types

All `json_value_t` instances are one of the following enumerated types:

```c
enum {
  JSON_NULL,
  JSON_ARRAY,
  JSON_STRING,
  JSON_NUMBER,
  JSON_OBJECT,
  JSON_BOOLEAN,
};
```

These values are et on the `.type` member property.

### parsing

```c
json_value_t *
json_parse (const char *filename, const char *src);
```

The parser accepts a filename and source string to parse. It returns a
`json_value_t` that is the root of the JSON structure. This value can be
scalar (`null`, `123`, `"abc"`, `true`) or an object (`{}`, `[]`). You
can check the type returned by comparing the structure's `.type` member
property with that of `JSON_NULL`, `JSON_ARRAY`, `JSON_STRING`,
`JSON_NUMBER`, `JSON_OBJECT`, or `JSON_BOOLEAN`.

If a memory error occurs, the return value will be `NULL`. If a parser
error occurs, the parser will quit with an imcomplete parser state and
return a `json_value_t *` with the `.errno` member propery set. You can
check the state of a parser error by comparing that property to that
`EJSON_OK`, `EJSON_MEM`, `EJSON_PARSE`, `EJSON_TOKEN`, or `EJSON_PARSERMEM`.


```c
json_value_t *root = json_parse(filename, src);

if (NULL == root) {
  fprintf(stderr, "memory error\n");
} else {
  if (0 != root->errno) {
    switch (root->errno) {
      case EJSON_TOKEN:
        break;

        ...
    }
  } else {
    // handle json
  }
}
```

### serialization

```c
char *
json_stringify (json_value_t *);
```

The serializer accepts a single `json_value_t *` and returns a `char *`
representation of the structure. Any `json_value_t` type is allowed.

```c
printf("%s\n", json_stringify(value));
```

```c
printf("%s\n", json_stringify(json_parse("number", "12345")); // "12345"
printf("%s\n", json_stringify(json_parse("object", "{\"foo\": null}")); // {"foo":null}
```

## license

MIT
