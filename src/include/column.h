#ifndef QTAB_COLUMN_H
#define QTAB_COLUMN_H

#include <stdbool.h>
#include <string.h>
#include <Python.h>

#define QTB_COLUMN_INITIAL_CAPACITY 20
#define QTB_COLUMN_GROWTH_COEFFICIENT 1.2

typedef void *(*mallocer)(size_t);

typedef enum {
  QTB_COLUMN_TYPE_STR,
  QTB_COLUMN_TYPE_INT,
  QTB_COLUMN_TYPE_FLOAT,
  QTB_COLUMN_TYPE_BOOL,
} QtbColumnType;

typedef union {
  char *s;
  long long i;
  double f;
  bool b;
} QtbColumnData;

typedef struct _QtbColumn {
  // Override implementation hooks
  char     *(*strdup)           (const char *);
  void     *(*realloc)          (void *, size_t);
  PyObject *(*PyTuple_New)      (Py_ssize_t);
  char     *(*PyUnicode_AsUTF8) (PyObject *);

  // Methods
  PyObject   *(*get_as_pyobject) (struct _QtbColumn *, size_t);
  bool        (*append)          (struct _QtbColumn *, PyObject *);
  const char *(*type_as_str)     (void);
  void        (*dealloc)         (struct _QtbColumn *);

  char *name;
  QtbColumnType type;
  QtbColumnData *data;
  size_t size;
  size_t capacity;
} QtbColumn;

QtbColumn *_qtb_column_new_many(size_t n, mallocer m);
#define qtb_column_new_many(n) _qtb_column_new_many(n, &malloc);
#define _qtb_column_new(m) _qtb_column_new_many(1, m);
#define qtb_column_new() _qtb_column_new_many(1, &malloc)

bool qtb_column_init(QtbColumn *column, PyObject *descriptor);
bool qtb_column_init_many(QtbColumn *columns, PyObject *blueprint, Py_ssize_t n);
void qtb_column_dealloc(QtbColumn *column);
PyObject *qtb_column_as_descriptor(QtbColumn *column);
bool qtb_column_append(QtbColumn *column, PyObject *item);
PyObject *qtb_column_get_as_pyobject(QtbColumn *column, size_t i);
const char *qtb_column_type_as_str(QtbColumn *column);

#endif
