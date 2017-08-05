#include "column.h"

static bool qtab_Column_init_name(qtab_Column *column, PyObject *name) {
  char *name_s;

  name_s = PyUnicode_AsUTF8AndSize(name, NULL);
  if (name == NULL)
    return false;

  column->name = strdup(name_s);
  if (column->name == NULL) {
    PyErr_SetString(PyExc_MemoryError, "failed to initialise column");
    return false;
  }

  return true;
}

static bool qtab_Column_init_type(qtab_Column *column, PyObject *type) {
  char *type_s;

  type_s = PyUnicode_AsUTF8AndSize(type, NULL);
  if (type == NULL)
    return false;

  column->type = strdup(type_s);
  if (column->type == NULL) {
    PyErr_SetString(PyExc_MemoryError, "failed to initialise column");
    return false;
  }

  return true;
}

bool qtab_Column_init(qtab_Column *column, PyObject *descriptor) {
  PyObject *name;
  PyObject *type;
  bool success;

  name = PySequence_ITEM(descriptor, 0);
  if (name == NULL)
    return false;

  success = qtab_Column_init_name(column, name);
  Py_DECREF(name);
  if (success == false)
    return false;

  type = PySequence_ITEM(descriptor, 1);
  if (type == NULL)
    return false;

  success = qtab_Column_init_type(column, type);
  Py_DECREF(type);
  if (success == false)
    return false;

  return true;
}

void qtab_Column_dealloc(qtab_Column *column) {
  free(column->name);
  free(column->type);
}
