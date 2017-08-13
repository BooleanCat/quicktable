#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <Python.h>
#include "column.h"

typedef struct {
  PyGILState_STATE gstate;
  PyObject *descriptor;
} TestState;

static void *failing_malloc(size_t size) {
  return NULL;
}

static char *get_exception_string(void) {
  PyObject *exc_value;
  PyObject *exc_type;
  PyObject *exc_traceback;
  char *exc_string;

  if (PyErr_Occurred() == NULL)
    return NULL;

  PyErr_Fetch(&exc_type, &exc_value, &exc_traceback);
  exc_string = PyUnicode_AsUTF8(exc_value);
  PyErr_Restore(exc_type, exc_value, exc_traceback);

  return exc_string;
}

static int setup(void **state) {
  TestState *test_state = (TestState *)malloc(sizeof(TestState));

  test_state->gstate = PyGILState_Ensure();
  test_state->descriptor = PyTuple_New(2);
  assert_non_null(test_state);
  PyTuple_SET_ITEM(test_state->descriptor, 0, PyUnicode_FromString("Name"));
  PyTuple_SET_ITEM(test_state->descriptor, 1, PyUnicode_FromString("str"));

  *state = (void *)test_state;
  return 0;
}

static int teardown(void **state) {
  TestState *test_state = (TestState *)(*state);

  Py_DECREF(test_state->descriptor);
  PyGILState_Release(test_state->gstate);
  free(test_state);

  return 0;
}

static void test_qtab_Column_new(void **state) {
  qtab_Column *column;

  column = qtab_Column_new();
  assert_non_null(column);
  assert_ptr_equal(column->strdup, &strdup);

  free(column);
}

static void test_qtab_Column_new_fails(void **state) {
  qtab_Column *column;

  column = _qtab_Column_new(&failing_malloc);
  assert_null(column);
}

static void test_qtab_Column_new_many(void **state) {
  qtab_Column *columns;

  columns = qtab_Column_new_many(20);
  assert_non_null(columns);

  for (size_t i = 0; i < 20; i++) {
    assert_ptr_equal(columns[i].strdup, &strdup);
  }

  free(columns);
}

static void test_qtab_Column_new_many_fails(void **state) {
  qtab_Column *columns;

  columns = _qtab_Column_new_many(20, &failing_malloc);
  assert_null(columns);
}

static void test_qtab_Column_init_returns_true(void **state) {
  qtab_Column *column;
  bool success;

  column = qtab_Column_new();
  assert_non_null(column);

  success = qtab_Column_init(column, ((TestState *)(*state))->descriptor);
  assert_int_equal(success, true);

  qtab_Column_dealloc(column);
  free(column);
}

static void test_qtab_Column_init_sets_column_name(void **state) {
  qtab_Column *column;

  column = qtab_Column_new();
  assert_non_null(column);

  qtab_Column_init(column, ((TestState *)(*state))->descriptor);
  assert_string_equal("Name", column->name);

  qtab_Column_dealloc(column);
  free(column);
}

static void test_qtab_Column_init_sets_column_type(void **state) {
  qtab_Column *column;

  column = qtab_Column_new();
  assert_non_null(column);

  qtab_Column_init(column, ((TestState *)(*state))->descriptor);
  assert_string_equal("str", column->type);

  qtab_Column_dealloc(column);
  free(column);
}

static void test_qtab_Column_init_does_not_change_descriptor_refcount(void **state) {
  qtab_Column *column;
  PyObject *descriptor;
  Py_ssize_t descriptor_ref_count;

  column = qtab_Column_new();
  assert_non_null(column);

  descriptor = ((TestState *)(*state))->descriptor;
  descriptor_ref_count = Py_REFCNT(descriptor);

  qtab_Column_init(column, descriptor);
  qtab_Column_dealloc(column);
  free(column);

  assert_int_equal(descriptor_ref_count, Py_REFCNT(descriptor));
}

static void test_qtab_Column_init_does_not_change_name_refcount(void **state) {
  qtab_Column *column;
  PyObject *descriptor;
  Py_ssize_t name_ref_count;

  column = qtab_Column_new();
  assert_non_null(column);

  descriptor = ((TestState *)(*state))->descriptor;
  name_ref_count = Py_REFCNT(PyTuple_GET_ITEM(descriptor, 0));

  qtab_Column_init(column, descriptor);
  qtab_Column_dealloc(column);
  free(column);

  assert_int_equal(name_ref_count, Py_REFCNT(PyTuple_GET_ITEM(descriptor, 0)));
}

static void test_qtab_Column_init_does_not_change_type_refcount(void **state) {
  qtab_Column *column;
  PyObject *descriptor;
  Py_ssize_t type_ref_count;

  column = qtab_Column_new();
  assert_non_null(column);

  descriptor = ((TestState *)(*state))->descriptor;
  type_ref_count = Py_REFCNT(PyTuple_GET_ITEM(descriptor, 1));

  qtab_Column_init(column, descriptor);
  qtab_Column_dealloc(column);
  free(column);

  assert_int_equal(type_ref_count, Py_REFCNT(PyTuple_GET_ITEM(descriptor, 1)));
}

static void qtab_Column_init_descriptor_not_sequence(void **state) {
  qtab_Column *column;
  bool result;

  column = qtab_Column_new();
  assert_non_null(column);

  result = qtab_Column_init(column, Py_None);
  assert_int_equal(result, false);
  assert_non_null(PyErr_Occurred());

  assert_string_equal(get_exception_string(), "descriptor not a sequence");

  free(column);
}

static void test_qtab_Column_as_descriptor_creates_descriptor(void **state) {
  qtab_Column *column;
  PyObject *descriptor;
  PyObject *new_descriptor;

  column = qtab_Column_new();
  assert_non_null(column);

  descriptor = ((TestState *)(*state))->descriptor;

  qtab_Column_init(column, descriptor);
  new_descriptor = qtab_Column_as_descriptor(column);
  qtab_Column_dealloc(column);
  free(column);

  assert_int_equal(PyTuple_Size(new_descriptor), 2);
  assert_int_equal(1, PyUnicode_Check(PyTuple_GET_ITEM(new_descriptor, 0)));
  assert_int_equal(1, PyUnicode_Check(PyTuple_GET_ITEM(new_descriptor, 1)));

  assert_int_equal(
    0,
    PyUnicode_CompareWithASCIIString(PyTuple_GET_ITEM(new_descriptor, 0), "Name")
  );

  assert_int_equal(
    0,
    PyUnicode_CompareWithASCIIString(PyTuple_GET_ITEM(new_descriptor, 1), "str")
  );

  Py_DECREF(new_descriptor);
}

int main(void) {
  Py_Initialize();

  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_qtab_Column_new),
    cmocka_unit_test(test_qtab_Column_new_fails),
    cmocka_unit_test(test_qtab_Column_new_many),
    cmocka_unit_test(test_qtab_Column_new_many_fails),
    cmocka_unit_test_setup_teardown(test_qtab_Column_init_returns_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtab_Column_init_sets_column_name, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtab_Column_init_sets_column_type, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtab_Column_init_does_not_change_descriptor_refcount, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtab_Column_init_does_not_change_name_refcount, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtab_Column_init_does_not_change_type_refcount, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtab_Column_as_descriptor_creates_descriptor, setup, teardown),
    cmocka_unit_test_setup_teardown(qtab_Column_init_descriptor_not_sequence, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
