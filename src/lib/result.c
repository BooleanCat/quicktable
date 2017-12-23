#include <Python.h>
#include "result.h"

ResultError package_py_err(void) {
  ResultError error;
  error.type = RESULT_ERROR_STORED;
  PyErr_Fetch(
    &error.value.stored.exc_value,
    &error.value.stored.exc_type,
    &error.value.stored.exc_traceback
  );
  return error;
}

void unpackage_py_err(ResultError error) {
  switch (error.type) {
    case RESULT_ERROR_STORED:
      PyErr_Restore(
        error.value.stored.exc_value,
        error.value.stored.exc_type,
        error.value.stored.exc_traceback
      );
      break;
    case RESULT_ERROR_NEW:
      PyErr_SetString(error.value.new.py_err_class, error.value.new.message);
      break;
  }
}
