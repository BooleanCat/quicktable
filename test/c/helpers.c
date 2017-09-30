#include <Python.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "column.h"

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

void assert_exc_string_equal(const char *test_string) {
  char *exc_string;

  exc_string = copy_exception_string();
  assert_string_equal(exc_string, test_string);

  free(exc_string);
}

char *failing_strdup(const char *s) {
  return NULL;
}

char *failing_PyUnicode_AsUTF8(PyObject *s) {
  PyErr_SetString(PyExc_RuntimeError, "PyUnicode_AsUTF8 failed");
  return NULL;
}

void *failing_malloc(size_t size) {
  return NULL;
}

PyObject *PyUnicode_FromString_succeeds(const char *s) {
  PyObject *unicode;

  unicode = PyUnicode_FromString(s);
  assert_non_null(unicode);
  return unicode;
}

void qtb_column_init_succeeds(QtbColumn *column, PyObject *descriptor) {
  assert_int_equal(qtb_column_init(column, descriptor), true);
}

QtbColumn *qtb_column_new_succeeds() {
  QtbColumn *column;

  column = qtb_column_new();
  assert_non_null(column);

  return column;
}

PyObject *new_descriptor(const char *name, const char *type) {
  PyObject *descriptor;

  descriptor = PyTuple_New(2);
  assert_non_null(descriptor);

  PyTuple_SET_ITEM(descriptor, 0, PyUnicode_FromString_succeeds(name));
  PyTuple_SET_ITEM(descriptor, 1, PyUnicode_FromString_succeeds(type));

  return descriptor;
}
