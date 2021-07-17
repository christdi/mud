#include <assert.h>
#include <stdlib.h>

#include "mud/data/deallocate.h"

void deallocate(void* value) {
  assert(value);

  free(value);
}
