#include <Python.h>
#include "column.h"
#include "helpers.h"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "result.h"

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

static void test_qtb_column_new_fails(void **state) {
  ResultQtbColumnPtr column;

  column = _qtb_column_new(&malloc_FAIL);
  assert_true(ResultFailed(column));
}

static void test_qtb_column_new_many_fails(void **state) {
  ResultQtbColumnPtr columns;

  columns = _qtb_column_new_many(20, &malloc_FAIL);
  assert_true(ResultFailed(columns));
}

static void test_qtb_column_init_does_not_change_descriptor_refcount(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  Py_ssize_t descriptor_ref_count;

  column = qtb_column_new_SUCCESS();

  descriptor = new_descriptor("Name", "str");
  descriptor_ref_count = Py_REFCNT(descriptor);

  qtb_column_init(column, descriptor);
  qtb_column_dealloc(column);
  free(column);

  assert_int_equal(descriptor_ref_count, Py_REFCNT(descriptor));
  Py_DECREF(descriptor);
}

static void test_qtb_column_init_does_not_change_name_refcount(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  Py_ssize_t name_ref_count;

  column = qtb_column_new_SUCCESS();

  descriptor = new_descriptor("Name", "str");
  name_ref_count = Py_REFCNT(PyTuple_GET_ITEM(descriptor, 0));

  qtb_column_init(column, descriptor);
  qtb_column_dealloc(column);
  free(column);

  assert_int_equal(name_ref_count, Py_REFCNT(PyTuple_GET_ITEM(descriptor, 0)));
  Py_DECREF(descriptor);
}

static void test_qtb_column_init_does_not_change_type_refcount(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  Py_ssize_t type_ref_count;

  column = qtb_column_new_SUCCESS();

  descriptor = new_descriptor("Name", "str");
  type_ref_count = Py_REFCNT(PyTuple_GET_ITEM(descriptor, 1));

  qtb_column_init(column, descriptor);
  qtb_column_dealloc(column);
  free(column);

  assert_int_equal(type_ref_count, Py_REFCNT(PyTuple_GET_ITEM(descriptor, 1)));
  Py_DECREF(descriptor);
}

static void test_qtb_column_init_descriptor_not_sequence(void **state) {
  QtbColumn *column;
  Result result;

  column = qtb_column_new_SUCCESS();

  result = qtb_column_init(column, Py_None);
  assert_true(ResultFailed(result));
  ResultFailureRaise(result);
  assert_exc_string_equal("descriptor not a sequence");

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_init_strdup_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  Result result;

  column = qtb_column_new_SUCCESS();
  column->strdup = &strdup_FAIL;

  descriptor = new_descriptor("Name", "str");

  result = qtb_column_init(column, descriptor);
  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "failed to initialise column");

  qtb_column_dealloc(column);
  free(column);
  Py_DECREF(descriptor);
}

static void test_qtb_column_init_free_on_fail(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  Result result;

  descriptor = new_descriptor("Name", "invalid");
  column = qtb_column_new_SUCCESS();

  result = qtb_column_init(column, descriptor);
  assert_true(ResultFailed(result));
  assert_null(column->name);
  assert_string_equal(result.value.error.value.new.message, "invalid column type");

  qtb_column_dealloc(column);
  free(column);
  Py_DECREF(descriptor);
}

static void test_qtb_column_repr_longest_of_first_five(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  ResultSize_t result;

  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_SUCCESS();

  qtb_column_init_SUCCESS(column, descriptor);
  Py_DECREF(descriptor);

  result = qtb_column_repr_longest_of_first_five(column);
  assert_true(ResultSuccessful(result));
  assert_int_equal(11, ResultValue(result));  // strlen("Level (int)")

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_repr_longest_of_first_five_malloc_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  ResultSize_t result;

  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_SUCCESS();

  qtb_column_init_SUCCESS(column, descriptor);
  Py_DECREF(descriptor);

  column->malloc = &malloc_FAIL;
  result = qtb_column_repr_longest_of_first_five(column);
  assert_true(ResultFailed(result));

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_repr_longest_of_first_five_shorter_row(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *level;
  ResultSize_t result;

  level = PyLong_FromLongLong_SUCCESS(0);
  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_SUCCESS();

  qtb_column_init_SUCCESS(column, descriptor);
  Py_DECREF(descriptor);

  qtb_column_append_SUCCESS(column, level);
  Py_DECREF(level);

  result = qtb_column_repr_longest_of_first_five(column);
  assert_true(ResultSuccessful(result));
  assert_int_equal(11, ResultValue(result));  // strlen("Level (int)")

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_repr_longest_of_first_five_longer_row(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *level;
  ResultSize_t result;

  level = PyLong_FromLongLong_SUCCESS(111111111111);
  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_SUCCESS();

  qtb_column_init_SUCCESS(column, descriptor);
  Py_DECREF(descriptor);

  qtb_column_append_SUCCESS(column, level);
  Py_DECREF(level);

  result = qtb_column_repr_longest_of_first_five(column);
  assert_true(ResultSuccessful(result));
  assert_int_equal(12, ResultValue(result));  // strlen("111111111111")

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_repr_longest_of_first_five_fifth_longer_row(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *level;
  ResultSize_t result;

  level = PyLong_FromLongLong_SUCCESS(0);
  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_SUCCESS();

  qtb_column_init_SUCCESS(column, descriptor);
  Py_DECREF(descriptor);

  for (size_t i = 0; i < 4; i++)
    qtb_column_append_SUCCESS(column, level);
  Py_DECREF(level);

  level = PyLong_FromLongLong_SUCCESS(111111111111);
  qtb_column_append_SUCCESS(column, level);
  Py_DECREF(level);

  result = qtb_column_repr_longest_of_first_five(column);
  assert_true(ResultSuccessful(result));
  assert_int_equal(12, ResultValue(result));  // strlen("111111111111")

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_repr_longest_of_first_five_sixth_ignored(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *level;
  ResultSize_t result;

  level = PyLong_FromLongLong_SUCCESS(0);
  descriptor = new_descriptor("Level", "int");
  column = qtb_column_new_SUCCESS();

  qtb_column_init_SUCCESS(column, descriptor);
  Py_DECREF(descriptor);

  for (size_t i = 0; i < 5; i++)
    qtb_column_append_SUCCESS(column, level);
  Py_DECREF(level);

  level = PyLong_FromLongLong_SUCCESS(111111111111);
  qtb_column_append_SUCCESS(column, level);
  Py_DECREF(level);

  result = qtb_column_repr_longest_of_first_five(column);
  assert_true(ResultSuccessful(result));
  assert_int_equal(11, ResultValue(result));  // strlen("Level (int)")

  qtb_column_dealloc(column);
  free(column);
}

static const struct CMUnitTest tests[] = {
  cmocka_unit_test(test_qtb_column_new_fails),
  cmocka_unit_test(test_qtb_column_new_many_fails),

  cmocka_unit_test_setup_teardown(test_qtb_column_init_does_not_change_descriptor_refcount, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_does_not_change_name_refcount, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_does_not_change_type_refcount, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_descriptor_not_sequence, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_strdup_fails, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_free_on_fail, setup, teardown),

  cmocka_unit_test_setup_teardown(test_qtb_column_repr_longest_of_first_five, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_repr_longest_of_first_five_malloc_fails, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_repr_longest_of_first_five_shorter_row, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_repr_longest_of_first_five_longer_row, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_repr_longest_of_first_five_fifth_longer_row, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_repr_longest_of_first_five_sixth_ignored, setup, teardown),
};

int test_column_run() {
  return cmocka_run_group_tests(tests, NULL, NULL);
}
