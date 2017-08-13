#include "column.h"

QtbColumn *_qtb_column_new_many(size_t n, mallocer m) {
  QtbColumn *columns;

  columns = (QtbColumn *)(*m)(sizeof(QtbColumn) * n);
  if (columns == NULL)
    return NULL;

  for (size_t i = 0; i < n; i++)
    columns[i].strdup = &strdup;

  return columns;
}

static bool qtb_column_init_name(QtbColumn *column, PyObject *name) {
  char *name_s;

  name_s = PyUnicode_AsUTF8AndSize(name, NULL);
  if (name == NULL)
    return false;

  column->name = (*column->strdup)(name_s);
  if (column->name == NULL) {
    PyErr_SetString(PyExc_MemoryError, "failed to initialise column");
    return false;
  }

  return true;
}

static bool qtb_column_init_type(QtbColumn *column, PyObject *type) {
  char *type_s;

  type_s = PyUnicode_AsUTF8AndSize(type, NULL);
  if (type == NULL)
    return false;

  column->type = (*column->strdup)(type_s);
  if (column->type == NULL) {
    PyErr_SetString(PyExc_MemoryError, "failed to initialise column");
    return false;
  }

  return true;
}

bool qtb_column_init(QtbColumn *column, PyObject *descriptor) {
  PyObject *fast_descriptor;
  bool success = true;

  fast_descriptor = PySequence_Fast(descriptor, "descriptor not a sequence");
  if (fast_descriptor == NULL)
    return false;

  if (
    qtb_column_init_name(column, PySequence_Fast_GET_ITEM(fast_descriptor, 0)) == false ||
    qtb_column_init_type(column, PySequence_Fast_GET_ITEM(fast_descriptor, 1)) == false
  )
    success = false;

  Py_DECREF(fast_descriptor);
  return success;
}

bool qtb_column_init_many(QtbColumn *columns, PyObject *blueprint, Py_ssize_t n) {
  PyObject *fast_blueprint = NULL;
  bool success = true;
  Py_ssize_t i;

  if ((fast_blueprint = PySequence_Fast(blueprint, "failed to initialise table")) == NULL)
    return NULL;

  for (i = 0; i < n; i++) {
    if (qtb_column_init(&columns[i], PySequence_Fast_GET_ITEM(fast_blueprint, i)) == false) {
      success = false;
      break;
    }
  }

  if (success == false) {
    for (Py_ssize_t j = i - 1; i >= 0; i--)
      qtb_column_dealloc(&columns[j]);
  }

  Py_DECREF(fast_blueprint);
  return success;
}

void qtb_column_dealloc(QtbColumn *column) {
  free(column->name);
  free(column->type);
}

PyObject *qtb_column_as_descriptor(QtbColumn *column) {
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
