#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <Python.h>
#include "column.h"

static void test_qtab_Column_init_returns_true(void **state) {
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

  qtab_Column column;
  bool success = qtab_Column_init(&column, Py_None);

  assert_int_equal(success, true);

  PyGILState_Release(gstate);
}

int main(void) {
  Py_Initialize();

  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_qtab_Column_init_returns_true),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
