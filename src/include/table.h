#ifndef QTAB_TABLE_H
#define QTAB_TABLE_H

#include <Python.h>
#include "structmember.h"
#include "blueprint.h"
#include <stdbool.h>

typedef struct {
    PyObject_HEAD
    Py_ssize_t size;
} qtab_Table;

#endif
