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

static PyObject *new_descriptor(const char *name, const char* type) {
  PyObject *descriptor;

  descriptor = PyTuple_New(2);
  assert_non_null(descriptor);

  PyTuple_SET_ITEM(descriptor, 0, PyUnicode_FromString(name));
  assert_non_null(PyTuple_GET_ITEM(descriptor, 0));

  PyTuple_SET_ITEM(descriptor, 1, PyUnicode_FromString(type));
  assert_non_null(PyTuple_GET_ITEM(descriptor, 1));

  return descriptor;
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
  test_state->descriptor = new_descriptor("Name", "str");

  *state = (void *)test_state;
  return 0;
}

static int teardown(void **state) {
  TestState *test_state = (TestState *)(*state);

  Py_DECREF(test_state->descriptor);
  PyErr_Clear();
  PyGILState_Release(test_state->gstate);
  free(test_state);

  return 0;
}

static void test_qtb_column_new_fails(void **state) {
  QtbColumn *column;

  column = _qtb_column_new(&failing_malloc);
  assert_null(column);
}

static void test_qtb_column_new_many_fails(void **state) {
  QtbColumn *columns;

  columns = _qtb_column_new_many(20, &failing_malloc);
  assert_null(columns);
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

static void test_qtb_column_init_strdup_fails(void **state) {
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

static void test_qtb_column_init_free_on_fail(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  bool success;

  descriptor = new_descriptor("Name", "invalid");

  column = qtb_column_new();
  assert_non_null(column);

  success = qtb_column_init(column, descriptor);
  assert_int_equal(success, false);

  assert_null(column->name);
  assert_string_equal(get_exception_string(), "invalid column type");

  Py_DECREF(descriptor);
}

const struct CMUnitTest column_tests[] = {
  cmocka_unit_test(test_qtb_column_new_fails),
  cmocka_unit_test(test_qtb_column_new_many_fails),

  cmocka_unit_test_setup_teardown(test_qtb_column_init_does_not_change_descriptor_refcount, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_does_not_change_name_refcount, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_does_not_change_type_refcount, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_descriptor_not_sequence, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_strdup_fails, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_init_free_on_fail, setup, teardown),
};
