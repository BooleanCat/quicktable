#include <Python.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "table.h"
#include "helpers.h"

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

static PyObject *blueprint_level_name_new_succeeds() {
  PyObject *blueprint;

  blueprint = PyList_New_SUCCESS(1);
  PyList_SET_ITEM(blueprint, 0, new_descriptor("Name", "str"));
  PyList_SET_ITEM(blueprint, 1, new_descriptor("Level", "int"));

  return blueprint;
}

static QtbTable *table_malloc_and_new_succeeds() {
  QtbTable *table;

  table = (QtbTable *)malloc(sizeof(QtbTable));
  assert_non_null(table);
  qtb_table_new_(table);
  return table;
}

static void test_qtb_table_new_(void **state) {
  QtbTable table;

  qtb_table_new_(&table);

  assert_int_equal(table.size, 0);
  assert_int_equal(table.width, 0);
  assert_null(table.columns);
}

static void test_qtb_table_init_(void **state) {
  QtbTable *table;
  Result result;
  PyObject *blueprint;

  blueprint = blueprint_level_name_new_succeeds();
  table = table_malloc_and_new_succeeds();

  result = qtb_table_init_(table, blueprint);
  Py_DECREF(blueprint);
  assert_true(ResultSuccessful(result));

  qtb_table_dealloc_(table);
  free(table);
}

static void test_qtb_table_init_blueprint_invalid(void **state) {
  QtbTable *table;
  Result result;
  PyObject *blueprint;

  blueprint = blueprint_level_name_new_succeeds();
  Py_INCREF(Py_None);
  assert_int_equal(PyList_SetItem(blueprint, 0, Py_None), 0);
  table = table_malloc_and_new_succeeds();

  result = qtb_table_init_(table, blueprint);
  Py_DECREF(blueprint);
  assert_true(ResultFailed(result));

  free(table);
}

static void test_qtb_table_init_failing_pysequence_size(void **state) {
  QtbTable *table;
  Result result;
  PyObject *blueprint;

  blueprint = blueprint_level_name_new_succeeds();
  table = table_malloc_and_new_succeeds();
  table->PySequence_Size = &PySequence_Size_FAIL;

  result = qtb_table_init_(table, blueprint);
  Py_DECREF(blueprint);
  assert_true(ResultFailed(result));
  ResultFailureRaise(result);
  assert_exc_string_equal("Oh no.");

  free(table);
}

static void test_qtb_table_init_failing_qtb_column_new_many(void **state) {
  QtbTable *table;
  Result result;
  PyObject *blueprint;

  blueprint = blueprint_level_name_new_succeeds();
  table = table_malloc_and_new_succeeds();
  table->column_new_many = &qtb_column_new_many_FAIL;

  result = qtb_table_init_(table, blueprint);
  Py_DECREF(blueprint);

  assert_true(ResultFailed(result));
  ResultFailureRaise(result);
  assert_exc_string_equal("Totally no memory left.");

  free(table);
}

static void test_qtb_table_init_failing_qtb_column_init_many(void **state) {
  QtbTable *table;
  Result result;
  PyObject *blueprint;

  blueprint = blueprint_level_name_new_succeeds();
  table = table_malloc_and_new_succeeds();
  table->column_init_many = &qtb_column_init_many_FAIL;

  result = qtb_table_init_(table, blueprint);
  Py_DECREF(blueprint);

  assert_true(ResultFailed(result));
  ResultFailureRaise(result);
  assert_exc_string_equal("No column init for you.");

  free(table);
}

static void test_qtb_table_length(void **state) {
  QtbTable table;

  table.size = 42;
  assert_int_equal(42, qtb_table_length(&table));
}

static void test_qtb_table_blueprint_(void **state) {
  QtbTable *table;
  Result result;
  PyObject *blueprint;
  ResultPyObjectPtr table_blueprint;
  int blueprint_comparison;

  blueprint = blueprint_level_name_new_succeeds();
  table = table_malloc_and_new_succeeds();
  result = qtb_table_init_(table, blueprint);

  assert_true(ResultSuccessful(result));

  table_blueprint = qtb_table_blueprint_(table);
  assert_true(ResultSuccessful(table_blueprint));

  blueprint_comparison = PyObject_RichCompareBool(blueprint, ResultValue(table_blueprint), Py_EQ);
  Py_DECREF(blueprint);
  Py_DECREF(ResultValue(table_blueprint));
  assert_true(blueprint_comparison);

  free(table);
}

static void test_qtb_table_blueprint_failing_pylist_new(void **state) {
  QtbTable *table;
  Result result;
  PyObject *blueprint;
  ResultPyObjectPtr table_blueprint;

  blueprint = blueprint_level_name_new_succeeds();
  table = table_malloc_and_new_succeeds();
  result = qtb_table_init_(table, blueprint);
  assert_true(ResultSuccessful(result));
  Py_DECREF(blueprint);

  table->PyList_New = &PyList_New_FAIL;
  table_blueprint = qtb_table_blueprint_(table);
  assert_true(ResultFailed(table_blueprint));

  ResultFailureRaise(table_blueprint);
  assert_exc_string_equal("Couldn't make that list for you.");

  free(table);
}

#define register_test(test) cmocka_unit_test_setup_teardown(test, setup, teardown)

static const struct CMUnitTest tests[] = {
    register_test(test_qtb_table_new_),

    register_test(test_qtb_table_init_),
    register_test(test_qtb_table_init_blueprint_invalid),
    register_test(test_qtb_table_init_failing_pysequence_size),
    register_test(test_qtb_table_init_failing_qtb_column_new_many),
    register_test(test_qtb_table_init_failing_qtb_column_init_many),

    register_test(test_qtb_table_length),

    register_test(test_qtb_table_blueprint_),
    register_test(test_qtb_table_blueprint_failing_pylist_new),
};

int test_table_run() {
  return cmocka_run_group_tests(tests, NULL, NULL);
}
