#include "tests.h"

int main(void) {
  Py_Initialize();

  return (
    test_append_run()
    || test_column_run()
  );
}
