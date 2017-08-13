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
} qtab_Column;

qtab_Column *_qtab_Column_new(mallocer m);
#define qtab_Column_new() _qtab_Column_new(&malloc)

qtab_Column *_qtab_Column_new_many(size_t n, mallocer m);
#define qtab_Column_new_many(n) _qtab_Column_new_many(n, &malloc);

bool qtab_Column_init(qtab_Column *column, PyObject *descriptor);
bool qtab_Column_init_many(qtab_Column *columns, PyObject *blueprint, Py_ssize_t n);
void qtab_Column_dealloc(qtab_Column *column);
PyObject *qtab_Column_as_descriptor(qtab_Column *column);

#endif
