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
  PyObject *fast_descriptor;
  bool success = true;

  fast_descriptor = PySequence_Fast(descriptor, "descriptor not a sequence");
  if (fast_descriptor == NULL)
    return false;

  if (
    qtab_Column_init_name(column, PySequence_Fast_GET_ITEM(fast_descriptor, 0)) == false ||
    qtab_Column_init_type(column, PySequence_Fast_GET_ITEM(fast_descriptor, 1)) == false
  )
    success = false;

  Py_DECREF(fast_descriptor);
  return success;
}

void qtab_Column_dealloc(qtab_Column *column) {
  free(column->name);
  free(column->type);
}
