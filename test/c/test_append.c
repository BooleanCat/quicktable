#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <Python.h>
#include "column.h"

static char *failing_strdup(const char *s) {
  return NULL;
}

static char *failing_PyUnicode_AsUTF8(PyObject *s) {
  PyErr_SetString(PyExc_RuntimeError, "PyUnicode_AsUTF8 failed");
  return NULL;
}

static PyObject *PyUnicode_FromString_succeeds(const char *s) {
  PyObject *unicode;

  unicode = PyUnicode_FromString(s);
  assert_non_null(unicode);
  return unicode;
}

static PyObject *new_descriptor(const char *name, const char* type) {
  PyObject *descriptor;

  descriptor = PyTuple_New(2);
  assert_non_null(descriptor);

  PyTuple_SET_ITEM(descriptor, 0, PyUnicode_FromString_succeeds(name));
  PyTuple_SET_ITEM(descriptor, 1, PyUnicode_FromString_succeeds(type));

  return descriptor;
}

static void qtb_column_init_succeeds(QtbColumn *column, PyObject *descriptor) {
  assert_int_equal(qtb_column_init(column, descriptor), true);
}

static QtbColumn *qtb_column_new_succeeds() {
  QtbColumn *column;

  column = qtb_column_new();
  assert_non_null(column);

  return column;
}

static char *copy_exception_string(void) {
  PyObject *exc_value;
  PyObject *exc_type;
  PyObject *exc_traceback;
  char *exc_string;

  assert_non_null(PyErr_Occurred());

  PyErr_Fetch(&exc_type, &exc_value, &exc_traceback);
  exc_string = strdup(PyUnicode_AsUTF8(exc_value));
  assert_non_null(exc_string);
  PyErr_Restore(exc_type, exc_value, exc_traceback);

  return exc_string;
}

static void assert_exc_string_equal(const char *test_string) {
  char *exc_string;

  exc_string = copy_exception_string();
  assert_string_equal(exc_string, test_string);

  free(exc_string);
}

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

void test_qtb_column_append(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *name;
  bool success;

  descriptor = new_descriptor("Name", "str");
  name = PyUnicode_FromString_succeeds("Pikachu");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  success = qtb_column_append(column, name);
  Py_DECREF(name);

  assert_int_equal(success, true);
  assert_int_equal(column->size, 1);
  assert_string_equal("Pikachu", column->data[0].s);

  qtb_column_dealloc(column);
  free(column);
}

void test_qtb_column_append_str_strdup_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *name;
  bool success;

  column = qtb_column_new_succeeds();
  descriptor = new_descriptor("Name", "str");
  name = PyUnicode_FromString_succeeds("Pikachu");

  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);
  column->strdup = &failing_strdup;

  success = qtb_column_append(column, name);
  Py_DECREF(name);
  qtb_column_dealloc(column);
  free(column);

  assert_int_equal(success, false);
  assert_exc_string_equal("could not create PyUnicodeobject");
  assert_int_equal(column->size, 0);
}

void test_qtb_column_append_str_PyUnicode_AsUTF8_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *name;
  bool success;

  column = qtb_column_new_succeeds();
  descriptor = new_descriptor("Name", "str");
  name = PyUnicode_FromString_succeeds("Pikachu");
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);
  column->PyUnicode_AsUTF8 = &failing_PyUnicode_AsUTF8;

  success = qtb_column_append(column, name);
  assert_int_equal(success, false);
  assert_exc_string_equal("PyUnicode_AsUTF8 failed");
  assert_int_equal(column->size, 0);

  Py_DECREF(name);
  qtb_column_dealloc(column);
  free(column);
}

const struct CMUnitTest append_tests[] = {
  cmocka_unit_test_setup_teardown(test_qtb_column_append, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_append_str_strdup_fails, setup, teardown),
  cmocka_unit_test_setup_teardown(test_qtb_column_append_str_PyUnicode_AsUTF8_fails, setup, teardown),
};
