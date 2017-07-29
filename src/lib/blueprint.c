#include "blueprint.h"

static bool qtab_validate_descriptor(PyObject *descriptor) {
  Py_ssize_t len;
  PyObject *name;

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

  if ((name = PySequence_ITEM(descriptor, 0)) == NULL)
    return false;

  if (PyUnicode_Check(name) == 0) {
    Py_DECREF(name);
    PyErr_SetString(PyExc_TypeError, "invalid blueprint");
    return false;
  }

  Py_DECREF(name);

  return true;
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
