#ifndef QTAB_HELPERS_H
#define QTAB_HELPERS_H

#include <stddef.h>
#include <Python.h>
#include "column.h"

void assert_exc_string_equal(const char *);

char *failing_strdup(const char *);
void *failing_malloc(size_t);
void *failing_realloc(void *, size_t);
char *failing_PyUnicode_AsUTF8(PyObject *);

PyObject *PyUnicode_FromString_succeeds(const char *);
void qtb_column_init_succeeds(QtbColumn *, PyObject *);
QtbColumn *qtb_column_new_succeeds(void);
void qtb_column_append_succeeds(QtbColumn *, PyObject *);
PyObject *new_descriptor(const char *, const char *);

#endif
