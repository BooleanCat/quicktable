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

static char *failing_strdup(const char *s) {
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

static void test_qtb_column_new(void **state) {
  QtbColumn *column;

  column = qtb_column_new();
  assert_non_null(column);
  assert_ptr_equal(column->strdup, &strdup);

  free(column);
}

static void test_qtb_column_new_fails(void **state) {
  QtbColumn *column;

  column = _qtb_column_new(&failing_malloc);
  assert_null(column);
}

static void test_qtb_column_new_many(void **state) {
  QtbColumn *columns;

  columns = qtb_column_new_many(20);
  assert_non_null(columns);

  for (size_t i = 0; i < 20; i++) {
    assert_ptr_equal(columns[i].strdup, &strdup);
  }

  free(columns);
}

static void test_qtb_column_new_many_fails(void **state) {
  QtbColumn *columns;

  columns = _qtb_column_new_many(20, &failing_malloc);
  assert_null(columns);
}

static void test_qtb_column_init_returns_true(void **state) {
  QtbColumn *column;
  bool success;

  column = qtb_column_new();
  assert_non_null(column);

  success = qtb_column_init(column, ((TestState *)(*state))->descriptor);
  assert_int_equal(success, true);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_init_sets_column_name(void **state) {
  QtbColumn *column;

  column = qtb_column_new();
  assert_non_null(column);

  qtb_column_init(column, ((TestState *)(*state))->descriptor);
  assert_string_equal("Name", column->name);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_init_sets_column_type(void **state) {
  QtbColumn *column;

  column = qtb_column_new();
  assert_non_null(column);

  qtb_column_init(column, ((TestState *)(*state))->descriptor);
  assert_string_equal("str", column->type);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_init_does_not_change_descriptor_refcount(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  Py_ssize_t descriptor_ref_count;

  column = qtb_column_new();
  assert_non_null(column);

  descriptor = ((TestState *)(*state))->descriptor;
  descriptor_ref_count = Py_REFCNT(descriptor);

  qtb_column_init(column, descriptor);
  qtb_column_dealloc(column);
  free(column);

  assert_int_equal(descriptor_ref_count, Py_REFCNT(descriptor));
}

static void test_qtb_column_init_does_not_change_name_refcount(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  Py_ssize_t name_ref_count;

  column = qtb_column_new();
  assert_non_null(column);

  descriptor = ((TestState *)(*state))->descriptor;
  name_ref_count = Py_REFCNT(PyTuple_GET_ITEM(descriptor, 0));

  qtb_column_init(column, descriptor);
  qtb_column_dealloc(column);
  free(column);

  assert_int_equal(name_ref_count, Py_REFCNT(PyTuple_GET_ITEM(descriptor, 0)));
}

static void test_qtb_column_init_does_not_change_type_refcount(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  Py_ssize_t type_ref_count;

  column = qtb_column_new();
  assert_non_null(column);

  descriptor = ((TestState *)(*state))->descriptor;
  type_ref_count = Py_REFCNT(PyTuple_GET_ITEM(descriptor, 1));

  qtb_column_init(column, descriptor);
  qtb_column_dealloc(column);
  free(column);

  assert_int_equal(type_ref_count, Py_REFCNT(PyTuple_GET_ITEM(descriptor, 1)));
}

static void test_qtb_column_init_descriptor_not_sequence(void **state) {
  QtbColumn *column;
  bool result;

  column = qtb_column_new();
  assert_non_null(column);

  result = qtb_column_init(column, Py_None);
  assert_int_equal(result, false);
  assert_non_null(PyErr_Occurred());

  assert_string_equal(get_exception_string(), "descriptor not a sequence");

  free(column);
}

static void test_qtb_column_as_descriptor_creates_descriptor(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *new_descriptor;

  column = qtb_column_new();
  assert_non_null(column);

  descriptor = ((TestState *)(*state))->descriptor;

  qtb_column_init(column, descriptor);
  new_descriptor = qtb_column_as_descriptor(column);
  qtb_column_dealloc(column);
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

void test_qtb_column_init_strdup_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  bool success;

  column = qtb_column_new();
  assert_non_null(column);
  column->strdup = &failing_strdup;

  descriptor = ((TestState *)(*state))->descriptor;

  success = qtb_column_init(column, descriptor);
  qtb_column_dealloc(column);
  free(column);

  assert_int_equal(success, false);
  assert_string_equal(get_exception_string(), "failed to initialise column");
}

const struct CMUnitTest column_tests[] = {
  cmocka_unit_test(test_qtb_column_new),
  cmocka_unit_test(test_qtb_column_new_fails),
  cmocka_unit_test(test_qtb_column_new_many),
  cmocka_unit_test(test_qtb_column_new_many_fails),

  cmocka_unit_test_setup_teardown(test_qtb_column_init_returns_true, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_sets_column_name, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_sets_column_type, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_does_not_change_descriptor_refcount, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_does_not_change_name_refcount, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_does_not_change_type_refcount, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_as_descriptor_creates_descriptor, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_descriptor_not_sequence, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_strdup_fails, setup, teardown),
};
