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

#### create a value

You can create a new JSON value with the `json_new` function. It accepts
a type and source string.

```c
json_value_t *null = json_new(JSON_NULL, "null");
json_value_t *one = json_new(JSON_NULL, "1");
json_value_t *string = json_new(JSON_STRING, "\"kinkajou\"");
```

#### destroying a value

You can destroy a created or parsed value with `json_destroy`. It
accepts a single `json_value_t` and frees all memory associated with it.
It will recursively destroy all child values found in the `.values[]`
member array.

```c
json_destroy(root);
root = NULL;
```

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

### error handling

```c
void
json_perror (json_value_t *value);
```

Error handling is done on a per `json_value_t` occurrence. Every
`json_value_t` instance has a `.errno` member property. When an error
occurs during parsing this property is set and the parser quits. The
only case when this is not true is when the parse fails to allocate
member to initialize. This ascertained when calling `json_parse` by
comparing the return value to `NULL`. An enumerated list of possible
errors are below:

```c
enum {
  EJSON_OK = 0,
  EJSON_MEM,
  EJSON_PARSE,
  EJSON_TOKEN,
  EJSON_PARSERMEM,
};
```

It is possible to print a predefined error message to `stderr` with the
`json_perror` function. It accepts `json_value_t` and will inspects its
`.errno` property and generated a printed error message.

```c
json_perror(value);
```

### set and get

```c
json_value_t *
json_get (json_value_t *root, const char *id);
```

```c
void
json_set (json_value_t *root, const char *id, json_value_t *value);
```

Values can be retrieved and modified with the `json_set` and `json_get`
functions.

You can set a `json_value_t` by name or with `json_set`. It
accept a root `json_value_t` value, an id, and a `json_value_t` to set.
You can check if an error occurs by inspecting the root's `.errno`
member property.

**set:**

```c
json_value_t *value = json_new(JSON_NULL, "null");
json_value_t *age = json_new(JSON_NUMBER, "24");

json_set(root, "name", value);
json_set(root, "age", age);
```

**get:**

```c
printf("%s\n", json_get(root, "name")); // null
printf("%s\n", json_get(root, "age")); // 24
```

## license

MIT
