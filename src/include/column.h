#ifndef QTAB_BLUEPRINT_H
#define QTAB_BLUEPRINT_H

#include <stdbool.h>
#include <Python.h>

typedef struct {

} qtab_Column;

bool qtab_Column_init(qtab_Column *column, PyObject *descriptor);

#endif
