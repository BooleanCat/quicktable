#ifndef QTB_TABLE_H
#define QTB_TABLE_H

#include <Python.h>
#include "structmember.h"
#include "blueprint.h"
#include "column.h"
#include <stdbool.h>

typedef struct {
    PyObject_HEAD

    // Override implementation hooks
    Py_ssize_t          (*PySequence_Size)      (PyObject *);
    PyObject           *(*PyList_New)           (Py_ssize_t);
    ResultQtbColumnPtr  (*column_new_many)      (size_t);
    Result              (*column_init_many)     (QtbColumn *, PyObject *, Py_ssize_t);
    ResultPyObjectPtr   (*column_as_descriptor) (QtbColumn *);

    Py_ssize_t size;
    Py_ssize_t width;
    QtbColumn *columns;
} QtbTable;

void qtb_table_new_(QtbTable *self);
void qtb_table_dealloc_(QtbTable *self);
Result qtb_table_init_(QtbTable *self, PyObject *blueprint);
Py_ssize_t qtb_table_length(QtbTable *self);
ResultPyObjectPtr qtb_table_item_(QtbTable *self, Py_ssize_t i);
Result qtb_table_append_(QtbTable *self, PyObject *row);
ResultPyObjectPtr qtb_table_pop_(QtbTable *self);
ResultPyObjectPtr qtb_table_blueprint_(QtbTable *self);

#endif
