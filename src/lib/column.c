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

bool qtab_Column_init_many(qtab_Column *columns, PyObject *blueprint, Py_ssize_t n) {
  PyObject *fast_blueprint = NULL;
  bool success = true;
  Py_ssize_t i;

  if ((fast_blueprint = PySequence_Fast(blueprint, "failed to initialise table")) == NULL)
    return NULL;

  for (i = 0; i < n; i++) {
    if (qtab_Column_init(&columns[i], PySequence_Fast_GET_ITEM(fast_blueprint, i)) == false) {
      success = false;
      break;
    }
  }

  if (success == false) {
    for (Py_ssize_t j = i - 1; i >= 0; i--)
      qtab_Column_dealloc(&columns[j]);
  }

  Py_DECREF(fast_blueprint);
  return success;
}

void qtab_Column_dealloc(qtab_Column *column) {
  free(column->name);
  free(column->type);
}

PyObject *qtab_Column_as_descriptor(qtab_Column *column) {
  PyObject *descriptor = NULL;
  PyObject *name = NULL;
  PyObject *type = NULL;

  if (
    ((name = PyUnicode_FromString(column->name)) == NULL) ||
    ((type = PyUnicode_FromString(column->type)) == NULL) ||
    ((descriptor = PyTuple_New(2)) == NULL)
  ) {
    Py_XDECREF(name);
    Py_XDECREF(type);
    return NULL;
  }

  PyTuple_SET_ITEM(descriptor, 0, name);
  PyTuple_SET_ITEM(descriptor, 1, type);

  return descriptor;
}
