#ifndef QTAB_COLUMN_H
#define QTAB_COLUMN_H

#include <stdbool.h>
#include <Python.h>

typedef void *(*mallocer)(size_t size);
typedef char *(*strduper)(const char *);

typedef struct {
  char *name;
  char *type;
  strduper strdup;
} QtbColumn;

QtbColumn *_qtb_column_new(mallocer m);
#define qtb_column_new() _qtb_column_new(&malloc)

QtbColumn *_qtb_column_new_many(size_t n, mallocer m);
#define qtb_column_new_many(n) _qtb_column_new_many(n, &malloc);

bool qtb_column_init(QtbColumn *column, PyObject *descriptor);
bool qtb_column_init_many(QtbColumn *columns, PyObject *blueprint, Py_ssize_t n);
void qtb_column_dealloc(QtbColumn *column);
PyObject *qtb_column_as_descriptor(QtbColumn *column);

#endif
