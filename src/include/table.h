#ifndef QTB_TABLE_H
#define QTB_TABLE_H

#include <Python.h>
#include "structmember.h"
#include "blueprint.h"
#include "column.h"
#include <stdbool.h>

typedef struct {
    PyObject_HEAD
    Py_ssize_t size;
    Py_ssize_t width;
    QtbColumn *columns;
} QtbTable;

void qtb_table_zero(QtbTable *self);
void qtb_table_dealloc_(QtbTable *self);
Result qtb_table_init_(QtbTable *self, PyObject *blueprint);
Py_ssize_t qtb_table_length(QtbTable *self);
ResultPyObjectPtr qtb_table_item_(QtbTable *self, Py_ssize_t i);
Result qtb_table_append_(QtbTable *self, PyObject *row);
ResultPyObjectPtr qtb_table_pop_(QtbTable *self);
ResultPyObjectPtr qtb_table_blueprint_(QtbTable *self);
ResultPyObjectPtr qtb_table_as_string_(QtbTable *self);

#endif
