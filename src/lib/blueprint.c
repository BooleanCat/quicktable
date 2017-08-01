#include "blueprint.h"

static const char *qtab_valid_column_types[] = {
  "str",
  "int",
  "bool",
  "float",
};

static bool qtab_validate_column_name(PyObject *descriptor) {
  PyObject *name;
  bool valid = true;

  if ((name = PySequence_ITEM(descriptor, 0)) == NULL)
    return false;

  if (PyUnicode_Check(name) == 0) {
    PyErr_SetString(PyExc_TypeError, "invalid blueprint");
    valid = false;
  }

  Py_DECREF(name);
  return valid;
}

static bool qtab_valid_column_types_contains(PyObject *type) {
  bool contained = false;

  for (size_t i = 0; i < 4; i++) {
    if(PyUnicode_CompareWithASCIIString(type, qtab_valid_column_types[i]) == 0) {
      contained = true;
      break;
    }
  }

  return contained;
}

static bool qtab_validate_column_type(PyObject *descriptor) {
  PyObject *type;
  bool valid = true;

  if ((type = PySequence_ITEM(descriptor, 1)) == NULL)
    return false;

  if (PyUnicode_Check(type) == 0 || !qtab_valid_column_types_contains(type)) {
    PyErr_SetString(PyExc_TypeError, "invalid blueprint");
    valid = false;
  }

  Py_DECREF(type);
  return valid;
}

static bool qtab_validate_descriptor(PyObject *descriptor) {
  Py_ssize_t len;

  if (PySequence_Check(descriptor) != 1) {
    PyErr_SetString(PyExc_TypeError, "invalid blueprint");
    return false;
  }

  if ((len = PySequence_Size(descriptor)) == -1)
    return false;

  if (len != 2) {
    PyErr_SetString(PyExc_TypeError, "invalid blueprint");
    return false;
  }

  if (qtab_validate_column_name(descriptor) == false)
    return false;

  return qtab_validate_column_type(descriptor);
}

bool qtab_validate_blueprint(PyObject *blueprint) {
  Py_ssize_t i;
  Py_ssize_t len;
  PyObject *descriptor;
  bool valid_descriptor;

  if (PySequence_Check(blueprint) != 1) {
    PyErr_SetString(PyExc_TypeError, "invalid blueprint");
    return false;
  }

  if ((len = PySequence_Size(blueprint)) == -1)
    return -1;

  for (i = 0; i < len; i++) {
    if ((descriptor = PySequence_ITEM(blueprint, i)) == NULL)
      return false;

    valid_descriptor = qtab_validate_descriptor(descriptor);
    Py_DECREF(descriptor);

    if (valid_descriptor == false)
      return false;
  }

  return true;
}
