#include <Python.h>
#include "column.h"
#include "column_as_string.h"
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

static int snprintf_return_n_then_n_minus_1(char *str, size_t size, const char *format, ...) {
  static int i = 1000;
  return --i;
}

static void test_qtb_column_str_header_as_string(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  ResultCharPtr result;

  descriptor = new_descriptor("Name", "str");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  result = qtb_column_header_as_string(column);
  assert_true(ResultSuccessful(result));
  assert_string_equal("Name (str)", ResultValue(result));
  free(ResultValue(result));

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_int_header_as_string(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  ResultCharPtr result;

  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  result = qtb_column_header_as_string(column);
  assert_true(ResultSuccessful(result));
  assert_string_equal("Level (int)", ResultValue(result));
  free(ResultValue(result));

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_float_header_as_string(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  ResultCharPtr result;

  descriptor = new_descriptor("Power", "float");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  result = qtb_column_header_as_string(column);
  assert_true(ResultSuccessful(result));
  assert_string_equal("Power (float)", ResultValue(result));
  free(ResultValue(result));

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_bool_header_as_string(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  ResultCharPtr result;

  descriptor = new_descriptor("Wild", "bool");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  result = qtb_column_header_as_string(column);
  assert_true(ResultSuccessful(result));
  assert_string_equal("Wild (bool)", ResultValue(result));
  free(ResultValue(result));

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_header_as_string_malloc_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  ResultCharPtr result;

  descriptor = new_descriptor("Name", "str");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  column->malloc = &failing_malloc;
  result = qtb_column_header_as_string(column);
  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "memory error");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_header_as_string_snprintf_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  ResultCharPtr result;

  descriptor = new_descriptor("Name", "str");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  column->snprintf_ = &failing_snprintf;
  result = qtb_column_header_as_string(column);
  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "failed to write header as string");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_str_cell_as_string(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *name;
  ResultCharPtr result;

  descriptor = new_descriptor("Name", "str");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  name = PyUnicode_FromString_succeeds("Pikachu");

  qtb_column_append_succeeds(column, name);
  Py_DECREF(name);

  result = qtb_column_cell_as_string(column, 0);
  assert_true(ResultSuccessful(result));
  assert_string_equal("Pikachu", ResultValue(result));
  free(ResultValue(result));

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_str_cell_as_string_strdup_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *name;
  ResultCharPtr result;

  descriptor = new_descriptor("Name", "str");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  name = PyUnicode_FromString_succeeds("Pikachu");

  qtb_column_append_succeeds(column, name);
  Py_DECREF(name);

  column->strdup = &failing_strdup;
  result = qtb_column_cell_as_string(column, 0);
  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "memory error");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_int_cell_as_string(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *level;
  ResultCharPtr result;

  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  level = PyLong_FromLongLong_succeeds(16);

  qtb_column_append_succeeds(column, level);
  Py_DECREF(level);

  result = qtb_column_cell_as_string(column, 0);
  assert_true(ResultSuccessful(result));
  assert_string_equal("16", ResultValue(result));
  free(ResultValue(result));

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_int_cell_as_string_snprintf_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *level;
  ResultCharPtr result;

  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  level = PyLong_FromLongLong_succeeds(16);

  qtb_column_append_succeeds(column, level);
  Py_DECREF(level);

  column->snprintf_ = &failing_snprintf;
  result = qtb_column_cell_as_string(column, 0);
  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "failed to get string length of cell");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_int_cell_as_string_malloc_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *level;
  ResultCharPtr result;

  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  level = PyLong_FromLongLong_succeeds(16);

  qtb_column_append_succeeds(column, level);
  Py_DECREF(level);

  column->malloc = &failing_malloc;
  result = qtb_column_cell_as_string(column, 0);
  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "memory error");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_int_cell_as_string_snprintf_fails_later(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *level;
  ResultCharPtr result;

  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  level = PyLong_FromLongLong_succeeds(16);

  qtb_column_append_succeeds(column, level);
  Py_DECREF(level);

  column->snprintf_ = &snprintf_return_n_then_n_minus_1;
  result = qtb_column_cell_as_string(column, 0);
  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "failed to write cell as string");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_float_cell_as_string(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *power;
  ResultCharPtr result;

  descriptor = new_descriptor("Power", "float");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  power = PyFloat_FromDouble_succeeds(42.12);

  qtb_column_append_succeeds(column, power);
  Py_DECREF(power);

  result = qtb_column_cell_as_string(column, 0);
  assert_true(ResultSuccessful(result));
  assert_string_equal("42.12", ResultValue(result));
  free(ResultValue(result));

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_float_cell_as_string_snprintf_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *power;
  ResultCharPtr result;

  descriptor = new_descriptor("Power", "float");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  power = PyFloat_FromDouble_succeeds(42.12);

  qtb_column_append_succeeds(column, power);
  Py_DECREF(power);

  column->snprintf_ = &failing_snprintf;
  result = qtb_column_cell_as_string(column, 0);
  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "failed to get string length of cell");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_float_cell_as_string_malloc_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *power;
  ResultCharPtr result;

  descriptor = new_descriptor("Power", "float");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  power = PyFloat_FromDouble_succeeds(42.12);

  qtb_column_append_succeeds(column, power);
  Py_DECREF(power);

  column->malloc = &failing_malloc;
  result = qtb_column_cell_as_string(column, 0);
  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "memory error");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_float_cell_as_string_snprintf_fails_later(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *power;
  ResultCharPtr result;

  descriptor = new_descriptor("Power", "float");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  power = PyFloat_FromDouble_succeeds(42.12);

  qtb_column_append_succeeds(column, power);
  Py_DECREF(power);

  column->snprintf_ = &snprintf_return_n_then_n_minus_1;
  result = qtb_column_cell_as_string(column, 0);
  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "failed to write cell as string");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_bool_cell_as_string_true(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *wild;
  ResultCharPtr result;

  descriptor = new_descriptor("Wild", "bool");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  wild = PyBool_FromLong_succeeds(1);

  qtb_column_append_succeeds(column, wild);
  Py_DECREF(wild);

  result = qtb_column_cell_as_string(column, 0);
  assert_true(ResultSuccessful(result));
  assert_string_equal("True", ResultValue(result));
  free(ResultValue(result));

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_bool_cell_as_string_false(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *wild;
  ResultCharPtr result;

  descriptor = new_descriptor("Wild", "bool");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  wild = PyBool_FromLong_succeeds(0);

  qtb_column_append_succeeds(column, wild);
  Py_DECREF(wild);

  result = qtb_column_cell_as_string(column, 0);
  assert_true(ResultSuccessful(result));
  assert_string_equal("False", ResultValue(result));
  free(ResultValue(result));

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_bool_cell_as_string_strdup_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *wild;
  ResultCharPtr result;

  descriptor = new_descriptor("Wild", "bool");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  wild = PyBool_FromLong_succeeds(1);

  qtb_column_append_succeeds(column, wild);
  Py_DECREF(wild);

  column->strdup = &failing_strdup;
  result = qtb_column_cell_as_string(column, 0);
  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "memory error");

  qtb_column_dealloc(column);
  free(column);
}

#define register_test(test) cmocka_unit_test_setup_teardown(test, setup, teardown)

static const struct CMUnitTest tests[] = {
    register_test(test_qtb_column_str_header_as_string),
    register_test(test_qtb_column_int_header_as_string),
    register_test(test_qtb_column_float_header_as_string),
    register_test(test_qtb_column_bool_header_as_string),
    register_test(test_qtb_column_header_as_string_malloc_fails),
    register_test(test_qtb_column_header_as_string_snprintf_fails),

    register_test(test_qtb_column_str_cell_as_string),
    register_test(test_qtb_column_str_cell_as_string_strdup_fails),

    register_test(test_qtb_column_int_cell_as_string),
    register_test(test_qtb_column_int_cell_as_string_snprintf_fails),
    register_test(test_qtb_column_int_cell_as_string_malloc_fails),
    register_test(test_qtb_column_int_cell_as_string_snprintf_fails_later),

    register_test(test_qtb_column_float_cell_as_string),
    register_test(test_qtb_column_float_cell_as_string_snprintf_fails),
    register_test(test_qtb_column_float_cell_as_string_malloc_fails),
    register_test(test_qtb_column_float_cell_as_string_snprintf_fails_later),

    register_test(test_qtb_column_bool_cell_as_string_true),
    register_test(test_qtb_column_bool_cell_as_string_false),
    register_test(test_qtb_column_bool_cell_as_string_strdup_fails),
};

int test_print_run() {
  return cmocka_run_group_tests(tests, NULL, NULL);
}
