#ifndef QTAB_TABLE_H
#define QTAB_TABLE_H

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
} qtab_Table;

#endif
