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

static PyObject *PyUnicode_FromString_fixture(const char *s) {
  PyObject *unicode;

  unicode = PyUnicode_FromString(s);
  assert_non_null(unicode);
  return unicode;
}

static void qtb_column_init_succeeds(QtbColumn *column, PyObject *descriptor) {
  bool success;

  success = qtb_column_init(column, descriptor);
  assert_int_equal(success, true);
}

static QtbColumn *qtb_column_new_succeeds() {
  QtbColumn *column;

  column = qtb_column_new();
  assert_non_null(column);

  return column;
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

void test_qtb_column_append_str_strdup_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  bool success;

  column = qtb_column_new_succeeds();
  descriptor = ((TestState *)(*state))->descriptor;
  qtb_column_init_succeeds(column, descriptor);

  column->strdup = &failing_strdup;
  success = qtb_column_append(column, PyUnicode_FromString_fixture("Pikachu"));
  assert_int_equal(success, false);
  assert_string_equal(get_exception_string(), "could not create PyUnicodeobject");

  qtb_column_dealloc(column);
  free(column);
}

const struct CMUnitTest append_tests[] = {
  cmocka_unit_test_setup_teardown(test_qtb_column_append_str_strdup_fails, setup, teardown),
};
