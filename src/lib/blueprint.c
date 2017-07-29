#include "blueprint.h"

bool qtab_validate_blueprint(PyObject *blueprint) {
  Py_ssize_t i;
  Py_ssize_t len;
  PyObject *descriptor;
  int is_sequence;

  if (PySequence_Check(blueprint) != 1) {
    PyErr_SetString(PyExc_TypeError, "invalid blueprint");
    return false;
  }

  if ((len = PySequence_Size(blueprint)) == -1)
    return -1;

  for (i = 0; i < len; i++) {
    if ((descriptor = PySequence_ITEM(blueprint, i)) == NULL)
      return false;

    is_sequence = PySequence_Check(descriptor);
    Py_DECREF(descriptor);

    if (is_sequence != 1) {
      PyErr_SetString(PyExc_TypeError, "invalid blueprint");
      return false;
    }
  }

  return true;
}
