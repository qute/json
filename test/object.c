
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <qute.h>
#include <ok/ok.h>
#include <fs/fs.h>

#include "json.h"

int
main (void) {
  const char *file = "test/fixtures/object.json";
  const char *src = fs_read(file);
  assert(src);


  ok("object");
  return 0;
}
