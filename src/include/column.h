#ifndef QTAB_BLUEPRINT_H
#define QTAB_BLUEPRINT_H

#include <stdbool.h>
#include <Python.h>

typedef struct {
  char *name;
  char *type;
} qtab_Column;

bool qtab_Column_init(qtab_Column *column, PyObject *descriptor);
void qtab_Column_dealloc(qtab_Column *column);

#endif
