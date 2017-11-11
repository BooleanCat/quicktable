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

static void test_qtb_column_header_repr_str(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  char *header_str;

  descriptor = new_descriptor("Name", "str");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  header_str = qtb_column_header_repr(column);
  assert_string_equal("Name (str)", header_str);
  free(header_str);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_header_repr_int(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  char *header_str;

  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  header_str = qtb_column_header_repr(column);
  assert_string_equal("Level (int)", header_str);
  free(header_str);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_header_repr_float(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  char *header_str;

  descriptor = new_descriptor("Power", "float");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  header_str = qtb_column_header_repr(column);
  assert_string_equal("Power (float)", header_str);
  free(header_str);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_header_repr_bool(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  char *header_str;

  descriptor = new_descriptor("Wild", "bool");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  header_str = qtb_column_header_repr(column);
  assert_string_equal("Wild (bool)", header_str);
  free(header_str);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_header_repr_malloc_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  char *header_str;

  descriptor = new_descriptor("Name", "str");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  column->malloc = &failing_malloc;
  header_str = qtb_column_header_repr(column);
  assert_null(header_str);
  assert_exc_string_equal("failed allocate memory for header repr");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_cell_repr_str(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *name;
  char *cell_str;

  descriptor = new_descriptor("Name", "str");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  name = PyUnicode_FromString_succeeds("Pikachu");

  qtb_column_append_succeeds(column, name);
  Py_DECREF(name);

  cell_str = qtb_column_cell_repr(column, 0);
  assert_string_equal("Pikachu", cell_str);
  free(cell_str);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_cell_repr_str_strdup_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *name;
  char *cell_str;

  descriptor = new_descriptor("Name", "str");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  name = PyUnicode_FromString_succeeds("Pikachu");

  qtb_column_append_succeeds(column, name);
  Py_DECREF(name);

  column->strdup = &failing_strdup;
  cell_str = qtb_column_cell_repr(column, 0);
  assert_null(cell_str);
  assert_exc_string_equal("failed allocate memory for cell repr");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_cell_repr_int(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *level;
  char *cell_str;

  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  level = PyLong_FromLongLong_succeeds(16);

  qtb_column_append_succeeds(column, level);
  Py_DECREF(level);

  cell_str = qtb_column_cell_repr(column, 0);
  assert_string_equal("16", cell_str);
  free(cell_str);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_cell_repr_int_malloc_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *level;
  char *cell_str;

  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  level = PyLong_FromLongLong_succeeds(16);

  qtb_column_append_succeeds(column, level);
  Py_DECREF(level);

  column->malloc = &failing_malloc;
  cell_str = qtb_column_cell_repr(column, 0);
  assert_null(cell_str);
  assert_exc_string_equal("failed allocate memory for cell repr");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_cell_repr_float(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *power;
  char *cell_str;

  descriptor = new_descriptor("Power", "float");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  power = PyFloat_FromDouble_succeeds(42.12);

  qtb_column_append_succeeds(column, power);
  Py_DECREF(power);

  cell_str = qtb_column_cell_repr(column, 0);
  assert_string_equal("42.12", cell_str);
  free(cell_str);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_cell_repr_float_malloc_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *power;
  char *cell_str;

  descriptor = new_descriptor("Power", "float");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  power = PyFloat_FromDouble_succeeds(42.12);

  qtb_column_append_succeeds(column, power);
  Py_DECREF(power);

  column->malloc = &failing_malloc;
  cell_str = qtb_column_cell_repr(column, 0);
  assert_null(cell_str);
  assert_exc_string_equal("failed allocate memory for cell repr");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_cell_repr_bool_true(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *wild;
  char *cell_str;

  descriptor = new_descriptor("Wild", "bool");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  wild = PyBool_FromLong_succeeds(1);

  qtb_column_append_succeeds(column, wild);
  Py_DECREF(wild);

  cell_str = qtb_column_cell_repr(column, 0);
  assert_string_equal("True", cell_str);
  free(cell_str);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_cell_repr_bool_false(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *wild;
  char *cell_str;

  descriptor = new_descriptor("Wild", "bool");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  wild = PyBool_FromLong_succeeds(0);

  qtb_column_append_succeeds(column, wild);
  Py_DECREF(wild);

  cell_str = qtb_column_cell_repr(column, 0);
  assert_string_equal("False", cell_str);
  free(cell_str);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_cell_repr_bool_strdup_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *wild;
  char *cell_str;

  descriptor = new_descriptor("Wild", "bool");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  wild = PyBool_FromLong_succeeds(1);

  qtb_column_append_succeeds(column, wild);
  Py_DECREF(wild);

  column->strdup = &failing_strdup;
  cell_str = qtb_column_cell_repr(column, 0);
  assert_null(cell_str);
  assert_exc_string_equal("failed allocate memory for cell repr");

  qtb_column_dealloc(column);
  free(column);
}

static const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_qtb_column_header_repr_str, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_header_repr_int, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_header_repr_float, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_header_repr_bool, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_header_repr_malloc_fails, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_cell_repr_str, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_cell_repr_str_strdup_fails, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_cell_repr_int, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_cell_repr_int_malloc_fails, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_cell_repr_float, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_cell_repr_float_malloc_fails, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_cell_repr_bool_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_cell_repr_bool_false, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_cell_repr_bool_strdup_fails, setup, teardown),
};

int test_print_run() {
  return cmocka_run_group_tests(tests, NULL, NULL);
}
