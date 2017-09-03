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
    columns[i].data = NULL;
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
    qtb_column_init_name(column, PySequence_Fast_GET_ITEM(fast_descriptor, 0)) == false
    || (column->type = qtb_column_type_from_pystring(PySequence_Fast_GET_ITEM(fast_descriptor, 1))) == QTB_COLUMN_TYPE_ERR
    || (column->data = (QtbColumnData *)malloc(sizeof(QtbColumnData) * QTB_COLUMN_INITIAL_CAPACITY)) == NULL
  ) {
    qtb_column_dealloc(column);
    success = false;
  }

  column->size = 0;
  column->capacity = QTB_COLUMN_INITIAL_CAPACITY;

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

  free(column->data);
  column->data = NULL;
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

static bool qtb_column_append_str(QtbColumn *column, PyObject *item) {
  char *s;

  if (PyUnicode_Check(item) == 0) {
    PyErr_SetString(PyExc_TypeError, "non-str entry for str column");
    return false;
  }

  if ((s = PyUnicode_AsUTF8(item)) == NULL)
    return false;

  column->data[column->size].s = (*column->strdup)(s);
  if (column->data[column->size].s == NULL) {
    PyErr_SetString(PyExc_MemoryError, "could not create PyUnicodeobject");
    return false;
  }

  return true;
}

static bool qtb_column_append_int(QtbColumn *column, PyObject *item) {
  if (PyLong_Check(item) == 0) {
    PyErr_SetString(PyExc_TypeError, "non-int entry for int column");
    return false;
  }

  column->data[column->size].i = PyLong_AsLongLong(item);
  return true;
}

static bool qtb_column_append_float(QtbColumn *column, PyObject *item) {
  if(PyFloat_Check(item) == 0) {
    PyErr_SetString(PyExc_TypeError, "non-float entry for float column");
    return false;
  }

  column->data[column->size].f = PyFloat_AsDouble(item);
  return true;
}

static bool qtb_column_append_bool(QtbColumn *column, PyObject *item) {
  if (PyBool_Check(item) == 0) {
    PyErr_SetString(PyExc_TypeError, "non-bool entry for bool column");
    return false;
  }

  column->data[column->size].b = (bool)PyLong_AsLong(item);
  return true;
}

bool qtb_column_append(QtbColumn *column, PyObject *item) {
  bool success = true;

  switch (column->type) {
    case QTB_COLUMN_TYPE_STR:
      success = qtb_column_append_str(column, item);
      break;
    case QTB_COLUMN_TYPE_INT:
      success = qtb_column_append_int(column, item);
      break;
    case QTB_COLUMN_TYPE_FLOAT:
      success = qtb_column_append_float(column, item);
      break;
    case QTB_COLUMN_TYPE_BOOL:
      success = qtb_column_append_bool(column, item);
      break;
    default:
      PyErr_SetString(PyExc_TypeError, "append to unknown column type");
      success = false;
      break;
  }

  if (success == true)
    column->size++;

  return success;
}

PyObject *qtb_column_get_as_pyobject(QtbColumn *column, size_t i) {
  // TODO: test failure of all of these in C tests
  switch (column->type) {
    case QTB_COLUMN_TYPE_STR:
      return PyUnicode_FromString(column->data[i].s);
    case QTB_COLUMN_TYPE_INT:
      return PyLong_FromLongLong(column->data[i].i);
    case QTB_COLUMN_TYPE_FLOAT:
      return PyFloat_FromDouble(column->data[i].f);
    case QTB_COLUMN_TYPE_BOOL:
      return PyBool_FromLong((long)column->data[i].b);
    case QTB_COLUMN_TYPE_ERR:
      PyErr_SetString(PyExc_TypeError, "");
      return false;
  }
}
