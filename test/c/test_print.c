#include <Python.h>
#include "column.h"
#include "print.h"
#include "helpers.h"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

static int setup(void **state) {
  PyGILState_STATE *gstate;

  gstate = (PyGILState_STATE *)malloc(sizeof(PyGILState_STATE));
  *gstate = PyGILState_Ensure();

  *state = (void *)gstate;
  return 0;
}

static int teardown(void **state) {
  PyGILState_STATE *gstate;

  gstate = (PyGILState_STATE *)(*state);
  PyErr_Clear();
  PyGILState_Release(*gstate);
  free(*state);

  return 0;
}

static void test_qtb_print_column_header_str(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  char *header_str;

  descriptor = new_descriptor("Name", "str");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  header_str = qtb_print_column_header(column);
  assert_string_equal("Name (str)", header_str);
  free(header_str);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_print_column_header_int(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  char *header_str;

  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  header_str = qtb_print_column_header(column);
  assert_string_equal("Level (int)", header_str);
  free(header_str);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_print_column_header_float(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  char *header_str;

  descriptor = new_descriptor("Power", "float");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  header_str = qtb_print_column_header(column);
  assert_string_equal("Power (float)", header_str);
  free(header_str);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_print_column_header_bool(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  char *header_str;

  descriptor = new_descriptor("Wild", "bool");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  header_str = qtb_print_column_header(column);
  assert_string_equal("Wild (bool)", header_str);
  free(header_str);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_print_column_header_malloc_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  char *header_str;

  descriptor = new_descriptor("Name", "str");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  header_str = _qtb_print_column_header(column, &failing_malloc);
  assert_null(header_str);
  assert_exc_string_equal("failed allocate memory for header repr");

  qtb_column_dealloc(column);
  free(column);
}

static const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_qtb_print_column_header_str, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_print_column_header_int, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_print_column_header_float, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_print_column_header_bool, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_print_column_header_malloc_fails, setup, teardown),
};

int test_print_run() {
  return cmocka_run_group_tests(tests, NULL, NULL);
}
