#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <Python.h>

static void null_test_success(void **state) {
  (void) state;
}

int main(void) {
  Py_Initialize();
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();

  PyObject *list = PyList_New(0);
  Py_DECREF(list);

  PyGILState_Release(gstate);

  const struct CMUnitTest tests[] = {
    cmocka_unit_test(null_test_success),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
