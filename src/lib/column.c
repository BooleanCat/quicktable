#include "column.h"

static QtbColumnType qtb_column_type_from_pystring(PyObject *type) {
  if (PyUnicode_CompareWithASCIIString(type, "str") == 0) {
    return QTB_COLUMN_TYPE_STR;
  } else if (PyUnicode_CompareWithASCIIString(type, "int") == 0) {
    return QTB_COLUMN_TYPE_INT;
  } else if (PyUnicode_CompareWithASCIIString(type, "float") == 0) {
    return QTB_COLUMN_TYPE_FLOAT;
  } else if (PyUnicode_CompareWithASCIIString(type, "bool") == 0) {
    return QTB_COLUMN_TYPE_BOOL;
  }

  PyErr_SetString(PyExc_RuntimeError, "invalid column type");
  return QTB_COLUMN_TYPE_ERR;
}

static PyObject *qtb_column_type_as_pystring(QtbColumnType type) {
  switch (type) {
    case QTB_COLUMN_TYPE_STR:
      return PyUnicode_FromString("str");
    case QTB_COLUMN_TYPE_INT:
      return PyUnicode_FromString("int");
    case QTB_COLUMN_TYPE_FLOAT:
      return PyUnicode_FromString("float");
    case QTB_COLUMN_TYPE_BOOL:
      return PyUnicode_FromString("bool");
    default:
      return NULL;
  }
}

QtbColumn *_qtb_column_new_many(size_t n, mallocer m) {
  QtbColumn *columns;

  columns = (QtbColumn *)(*m)(sizeof(QtbColumn) * n);
  if (columns == NULL)
    return NULL;

  for (size_t i = 0; i < n; i++) {
    columns[i].strdup = &strdup;
    columns[i].name = NULL;
  }

  return columns;
}

static bool qtb_column_init_name(QtbColumn *column, PyObject *name) {
  char *name_s;

  name_s = PyUnicode_AsUTF8(name);
  if (name == NULL)
    return false;

  column->name = (*column->strdup)(name_s);
  if (column->name == NULL) {
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
    (column->type = qtb_column_type_from_pystring(PySequence_Fast_GET_ITEM(fast_descriptor, 1))) == QTB_COLUMN_TYPE_ERR
  ) {
    qtb_column_dealloc(column);
    success = false;
  }

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
  column->name = NULL;
}

PyObject *qtb_column_as_descriptor(QtbColumn *column) {
  PyObject *descriptor = NULL;
  PyObject *name = NULL;
  PyObject *type = NULL;

  if (
    ((name = PyUnicode_FromString(column->name)) == NULL) ||
    ((type = qtb_column_type_as_pystring(column->type)) == NULL) ||
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
