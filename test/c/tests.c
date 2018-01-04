#include <Python.h>
#include "tests.h"

int main(void) {
  Py_Initialize();

  return (
    test_append_run()
    || test_column_run()
    || test_column_as_string_run()
    || test_result_run()
    || test_table_run()
  );
}
