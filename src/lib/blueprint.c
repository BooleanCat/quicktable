#include "blueprint.h"

static const char *qtb_valid_column_types[] = {
  "str",
  "int",
  "bool",
  "float",
};

static bool qtb_validate_column_name(PyObject *descriptor) {
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

static bool qtb_valid_column_types_contains(PyObject *type) {
  bool contained = false;

  for (size_t i = 0; i < 4; i++) {
    if(PyUnicode_CompareWithASCIIString(type, qtb_valid_column_types[i]) == 0) {
      contained = true;
      break;
    }
  }

  return contained;
}

static bool qtb_validate_column_type(PyObject *descriptor) {
  PyObject *type;
  bool valid = true;

  if ((type = PySequence_ITEM(descriptor, 1)) == NULL)
    return false;

  if (PyUnicode_Check(type) == 0 || !qtb_valid_column_types_contains(type)) {
    PyErr_SetString(PyExc_TypeError, "invalid blueprint");
    valid = false;
  }

  Py_DECREF(type);
  return valid;
}

static bool qtb_validate_descriptor(PyObject *descriptor) {
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

  if (qtb_validate_column_name(descriptor) == false)
    return false;

  return qtb_validate_column_type(descriptor);
}

bool qtb_validate_blueprint(PyObject *blueprint) {
  Py_ssize_t len;
  PyObject *fast_blueprint;
  bool valid_descriptor = true;

  if ((len = PySequence_Size(blueprint)) == -1) {
    PyErr_SetString(PyExc_TypeError, "invalid blueprint");
    return false;
  }

  fast_blueprint = PySequence_Fast(blueprint, "invalid blueprint");
  if (fast_blueprint == NULL)
    return false;

  for (Py_ssize_t i = 0; i < len; i++) {
    valid_descriptor = qtb_validate_descriptor(PySequence_Fast_GET_ITEM(fast_blueprint, i));
    if (valid_descriptor == false)
      break;
  }

  Py_DECREF(fast_blueprint);
  return valid_descriptor;
}
