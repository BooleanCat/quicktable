#include "blueprint.h"
#include "result.h"
#include <stdbool.h>

static const char *qtb_valid_column_types[] = {
  "str",
  "int",
  "bool",
  "float",
};

static Result qtb_validate_column_name(PyObject *descriptor) {
  PyObject *name;
  int name_check;

  if ((name = PySequence_ITEM(descriptor, 0)) == NULL)
    return ResultFailureFromPyErr();

  name_check = PyUnicode_Check(name);
  Py_DECREF(name);

  if (name_check == 0)
    return ResultFailure(PyExc_TypeError, "invalid blueprint");

  return ResultSuccess;
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

static Result qtb_validate_column_type(PyObject *descriptor) {
  PyObject *type;
  bool contains;
  int type_check;

  if ((type = PySequence_ITEM(descriptor, 1)) == NULL)
    return ResultFailureFromPyErr();

  type_check = PyUnicode_Check(type);
  if (type_check == 0) {
    Py_DECREF(type);
    return ResultFailure(PyExc_TypeError, "invalid blueprint");
  }

  contains = qtb_valid_column_types_contains(type);
  Py_DECREF(type);
  if (!contains)
    return ResultFailure(PyExc_TypeError, "invalid blueprint");

  return ResultSuccess;
}

static Result qtb_validate_descriptor(PyObject *descriptor) {
  Py_ssize_t len;
  Result result;

  if (PySequence_Check(descriptor) != 1)
    return ResultFailure(PyExc_TypeError, "invalid blueprint");

  if ((len = PySequence_Size(descriptor)) == -1)
    return ResultFailure(PyExc_TypeError, "invalid blueprint");

  if (len != 2)
    return ResultFailure(PyExc_TypeError, "invalid blueprint");

  result = qtb_validate_column_name(descriptor);
  if (ResultFailed(result))
    return result;

  return qtb_validate_column_type(descriptor);
}

Result qtb_validate_blueprint(PyObject *blueprint) {
  Py_ssize_t len;
  PyObject *fast_blueprint;
  Result result;

  if ((len = PySequence_Size(blueprint)) == -1)
    return ResultFailure(PyExc_TypeError, "invalid blueprint");

  fast_blueprint = PySequence_Fast(blueprint, "invalid blueprint");
  if (fast_blueprint == NULL)
    return ResultFailureFromPyErr();

  for (Py_ssize_t i = 0; i < len; i++) {
    result = qtb_validate_descriptor(PySequence_Fast_GET_ITEM(fast_blueprint, i));
    if (ResultFailed(result)) {
      Py_DECREF(fast_blueprint);
      return result;
    }
  }

  Py_DECREF(fast_blueprint);
  return ResultSuccess;
}
