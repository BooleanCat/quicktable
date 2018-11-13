#include <Python.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "column.h"
#include "result.h"

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

char *strdup_FAIL(const char *s) {
  return NULL;
}

const char *PyUnicode_AsUTF8_FAIL(PyObject *s) {
  PyErr_SetString(PyExc_RuntimeError, "PyUnicode_AsUTF8 failed");
  return NULL;
}

Py_ssize_t PySequence_Size_FAIL(PyObject *sequence) {
  PyErr_SetString(PyExc_RuntimeError, "Oh no.");
  return -1;
}

PyObject *PyList_New_FAIL(Py_ssize_t size) {
  PyErr_SetString(PyExc_RuntimeError, "Couldn't make that list for you.");
  return NULL;
}

void *malloc_FAIL(size_t size) {
  return NULL;
}

int snprintf_FAIL(char *str, size_t size, const char *format, ...) {
  return -1;
}

void *realloc_FAIL(void *ptr, size_t size) {
  return NULL;
}

ResultQtbColumnPtr qtb_column_new_many_FAIL(size_t size) {
  return ResultQtbColumnPtrFailure(PyExc_MemoryError, "Totally no memory left.");
}

Result qtb_column_init_many_FAIL(QtbColumn *columns, PyObject *blueprint, Py_ssize_t size) {
  return ResultFailure(PyExc_RuntimeError, "No column init for you.");
}

PyObject *PyUnicode_FromString_SUCCESS(const char *s) {
  PyObject *unicode;

  unicode = PyUnicode_FromString(s);
  assert_non_null(unicode);
  return unicode;
}

PyObject *PyLong_FromLongLong_SUCCESS(long long n) {
  PyObject *py_long;

  py_long = PyLong_FromLongLong(n);
  assert_non_null(py_long);
  return py_long;
}

PyObject *PyFloat_FromDouble_SUCCESS(double n) {
  PyObject *py_float;

  py_float = PyFloat_FromDouble(n);
  assert_non_null(py_float);
  return py_float;
}

PyObject *PyBool_FromLong_SUCCESS(long n) {
  PyObject *py_bool;

  py_bool = PyBool_FromLong(n);
  assert_non_null(py_bool);
  return py_bool;
}

PyObject *PyList_New_SUCCESS(Py_ssize_t len) {
  PyObject *list;

  list = PyList_New(len);
  assert_non_null(list);
  return list;
}

PyObject *PyTuple_New_SUCCESS(Py_ssize_t len) {
  PyObject *tuple;

  tuple = PyTuple_New(len);
  assert_non_null(tuple);
  return tuple;
}

void qtb_column_init_SUCCESS(QtbColumn *column, PyObject *descriptor) {
  assert_true(ResultSuccessful(qtb_column_init(column, descriptor)));
}

QtbColumn *qtb_column_new_SUCCESS() {
  ResultQtbColumnPtr column;

  column = qtb_column_new();
  assert_true(ResultSuccessful(column));

  return ResultValue(column);
}

void qtb_column_append_SUCCESS(QtbColumn *column, PyObject *item) {
  assert_true(ResultSuccessful(qtb_column_append(column, item)));
}

PyObject *new_descriptor(const char *name, const char *type) {
  PyObject *descriptor;

  descriptor = PyTuple_New_SUCCESS(2);
  PyTuple_SET_ITEM(descriptor, 0, PyUnicode_FromString_SUCCESS(name));
  PyTuple_SET_ITEM(descriptor, 1, PyUnicode_FromString_SUCCESS(type));

  return descriptor;
}
