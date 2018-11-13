#ifndef QTB_COLUMN_H
#define QTB_COLUMN_H

#include <stdbool.h>
#include <string.h>
#include <Python.h>
#include "result.h"

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
  char       *(*strdup)           (const char *);
  void       *(*malloc)           (size_t);
  void       *(*realloc)          (void *, size_t);
  int         (*snprintf_)        (char *, size_t, const char *, ...);
  PyObject   *(*PyTuple_New)      (Py_ssize_t);
  const char *(*PyUnicode_AsUTF8) (PyObject *);

  // Methods
  ResultPyObjectPtr  (*get_as_pyobject) (struct _QtbColumn *, size_t);
  Result             (*append)          (struct _QtbColumn *, PyObject *);
  const char        *(*type_as_string)  (void);
  ResultCharPtr      (*cell_as_string)  (struct _QtbColumn *, size_t);
  void               (*dealloc)         (struct _QtbColumn *);

  char *name;
  QtbColumnType type;
  QtbColumnData *data;
  size_t size;
  size_t capacity;
} QtbColumn;

typedef union {
  QtbColumn *value;
  ResultError error;
} QtbColumnPtrValue;

typedef struct {
  Result_HEAD
  QtbColumnPtrValue value;
} ResultQtbColumnPtr;

#define ResultQtbColumnPtrSuccess(value) ResultRegisterSuccess(ResultQtbColumnPtr, value)
#define ResultQtbColumnPtrFailure(py_err, message) ResultRegisterFailure(ResultQtbColumnPtr, py_err, message)
#define ResultQtbColumnPtrFailureFromPyErr() ResultRegisterFailureFromPyErr(ResultQtbColumnPtr)
#define ResultQtbColumnPtrFailureFromResult(result) ResultRegisterFailureFromResult(ResultQtbColumnPtr, result)

ResultQtbColumnPtr _qtb_column_new_many(size_t n, mallocer m);
#define qtb_column_new_many(n) _qtb_column_new_many(n, &malloc);
#define _qtb_column_new(m) _qtb_column_new_many(1, m);
#define qtb_column_new() _qtb_column_new_many(1, &malloc)

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

Result qtb_column_init(QtbColumn *column, PyObject *descriptor);
Result qtb_column_init_many(QtbColumn *columns, PyObject *blueprint, Py_ssize_t n);
void qtb_column_dealloc(QtbColumn *column);
ResultPyObjectPtr qtb_column_as_descriptor(QtbColumn *column);
Result qtb_column_append(QtbColumn *column, PyObject *item);
ResultPyObjectPtr qtb_column_get_as_pyobject(QtbColumn *column, size_t i);
const char *qtb_column_type_as_string(QtbColumn *column);
ResultCharPtr qtb_column_header_as_string(QtbColumn *column);
ResultCharPtr qtb_column_cell_as_string(QtbColumn *column, size_t i);
ResultSize_t qtb_column_repr_longest_of_first_five(QtbColumn *column);

#endif
