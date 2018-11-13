#ifndef QTAB_HELPERS_H
#define QTAB_HELPERS_H

#include <stddef.h>
#include <Python.h>
#include "column.h"

void assert_exc_string_equal(const char *);

char *strdup_FAIL(const char *);
void *malloc_FAIL(size_t);
void *realloc_FAIL(void *, size_t);
int snprintf_FAIL(char *, size_t, const char *, ...);
const char *PyUnicode_AsUTF8_FAIL(PyObject *);
Py_ssize_t PySequence_Size_FAIL(PyObject *);
PyObject *PyList_New_FAIL(Py_ssize_t);
ResultQtbColumnPtr qtb_column_new_many_FAIL(size_t);
Result qtb_column_init_many_FAIL(QtbColumn *, PyObject *, Py_ssize_t);

PyObject *PyUnicode_FromString_SUCCESS(const char *);
PyObject *PyLong_FromLongLong_SUCCESS(long long n);
PyObject *PyFloat_FromDouble_SUCCESS(double n);
PyObject *PyBool_FromLong_SUCCESS(long n);
PyObject *PyList_New_SUCCESS(Py_ssize_t len);
PyObject *PyTuple_New_SUCCESS(Py_ssize_t len);
void qtb_column_init_SUCCESS(QtbColumn *, PyObject *);
QtbColumn *qtb_column_new_SUCCESS(void);
void qtb_column_append_SUCCESS(QtbColumn *, PyObject *);

PyObject *new_descriptor(const char *, const char *);

#endif
