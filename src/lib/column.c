#include "column.h"

// ===== qtb_column_str =====

static PyObject *qtb_column_get_as_pyobject_str(QtbColumn *column, size_t i) {
  return PyUnicode_FromString(column->data[i].s);
}

static bool qtb_column_append_str(QtbColumn *column, PyObject *item) {
  char *s;

  if (PyUnicode_Check(item) == 0) {
    PyErr_SetString(PyExc_TypeError, "non-str entry for str column");
    return false;
  }

  if ((s = (*column->PyUnicode_AsUTF8)(item)) == NULL)
    return false;

  column->data[column->size].s = (*column->strdup)(s);
  if (column->data[column->size].s == NULL) {
    PyErr_SetString(PyExc_MemoryError, "could not create PyUnicodeobject");
    return false;
  }

  return true;
}

void qtb_column_dealloc_str(QtbColumn *column) {
  for (size_t i = 0; i < column->size; i++)
    free(column->data[i].s);
}

static const char *qtb_column_type_as_str_str() {
  return "str";
}

static char *qtb_column_cell_repr_str(QtbColumn *column, size_t i) {
  char *copy;

  copy = column->strdup(column->data[i].s);
  if (copy == NULL)
    PyErr_SetString(PyExc_MemoryError, "failed allocate memory for cell repr");

  return copy;
}

// ===== qtb_column_int =====

static PyObject *qtb_column_get_as_pyobject_int(QtbColumn *column, size_t i) {
  return PyLong_FromLongLong(column->data[i].i);
}

static bool qtb_column_append_int(QtbColumn *column, PyObject *item) {
  if (PyLong_Check(item) == 0) {
    PyErr_SetString(PyExc_TypeError, "non-int entry for int column");
    return false;
  }

  column->data[column->size].i = PyLong_AsLongLong(item);
  return true;
}

static const char *qtb_column_type_as_str_int() {
  return "int";
}

static char *qtb_column_cell_repr_int(QtbColumn *column, size_t i) {
  char *cell_repr;
  size_t size;

  size = snprintf(NULL, 0, "%lld", column->data[i].i);

  cell_repr = (char *)column->malloc(sizeof(char) * size + 1);
  if (cell_repr == NULL) {
    PyErr_SetString(PyExc_MemoryError, "failed allocate memory for cell repr");
    return NULL;
  }

  snprintf(cell_repr, size + 1, "%lld", column->data[i].i);
  return cell_repr;
}

// ===== qtb_column_float =====

static PyObject *qtb_column_get_as_pyobject_float(QtbColumn *column, size_t i) {
  return PyFloat_FromDouble(column->data[i].f);
}

static bool qtb_column_append_float(QtbColumn *column, PyObject *item) {
  if(PyFloat_Check(item) == 0) {
    PyErr_SetString(PyExc_TypeError, "non-float entry for float column");
    return false;
  }

  column->data[column->size].f = PyFloat_AsDouble(item);
  return true;
}

static const char *qtb_column_type_as_str_float() {
  return "float";
}

static char *qtb_column_cell_repr_float(QtbColumn *column, size_t i) {
  char *cell_repr;
  size_t size;

  size = snprintf(NULL, 0, "%.2f", column->data[i].f);

  cell_repr = (char *)column->malloc(sizeof(char) * size + 1);
  if (cell_repr == NULL) {
    PyErr_SetString(PyExc_MemoryError, "failed allocate memory for cell repr");
    return NULL;
  }

  snprintf(cell_repr, size + 1, "%.2f", column->data[i].f);
  return cell_repr;
}

// ===== qtb_column_bool =====

static PyObject *qtb_column_get_as_pyobject_bool(QtbColumn *column, size_t i) {
  return PyBool_FromLong(column->data[i].b);
}

static bool qtb_column_append_bool(QtbColumn *column, PyObject *item) {
  if (PyBool_Check(item) == 0) {
    PyErr_SetString(PyExc_TypeError, "non-bool entry for bool column");
    return false;
  }

  column->data[column->size].b = (bool)PyLong_AsLong(item);
  return true;
}

static const char *qtb_column_type_as_str_bool() {
  return "bool";
}

static char *qtb_column_cell_repr_bool(QtbColumn *column, size_t i) {
  char *cell_repr;

  cell_repr = column->strdup(column->data[i].b ? "True" : "False");
  if (cell_repr == NULL)
    PyErr_SetString(PyExc_MemoryError, "failed allocate memory for cell repr");

  return cell_repr;
}

// ===== qtb_column_default =====

void qtb_column_dealloc_default(QtbColumn *column) {}

void qtb_column_init_methods(QtbColumn *column) {
  switch (column->type) {
    case QTB_COLUMN_TYPE_STR:
      column->get_as_pyobject = &qtb_column_get_as_pyobject_str;
      column->append = &qtb_column_append_str;
      column->type_as_str = &qtb_column_type_as_str_str;
      column->cell_repr = &qtb_column_cell_repr_str;
      column->dealloc = &qtb_column_dealloc_str;
      break;
    case QTB_COLUMN_TYPE_INT:
      column->get_as_pyobject = &qtb_column_get_as_pyobject_int;
      column->append = &qtb_column_append_int;
      column->type_as_str = &qtb_column_type_as_str_int;
      column->cell_repr = &qtb_column_cell_repr_int;
      column->dealloc = &qtb_column_dealloc_default;
      break;
    case QTB_COLUMN_TYPE_FLOAT:
      column->get_as_pyobject = &qtb_column_get_as_pyobject_float;
      column->append = &qtb_column_append_float;
      column->type_as_str = &qtb_column_type_as_str_float;
      column->cell_repr = &qtb_column_cell_repr_float;
      column->dealloc = &qtb_column_dealloc_default;
      break;
    case QTB_COLUMN_TYPE_BOOL:
      column->get_as_pyobject = &qtb_column_get_as_pyobject_bool;
      column->append = &qtb_column_append_bool;
      column->type_as_str = &qtb_column_type_as_str_bool;
      column->cell_repr = &qtb_column_cell_repr_bool;
      column->dealloc = &qtb_column_dealloc_default;
      break;
  }
}

PyObject *qtb_column_get_as_pyobject(QtbColumn *column, size_t i) {
  return column->get_as_pyobject(column, i);
}

static bool qtb_column_grow(QtbColumn *column) {
  size_t new_capacity;
  QtbColumnData *new_data;

  new_capacity = QTB_COLUMN_GROWTH_COEFFICIENT * column->capacity;
  new_data = (QtbColumnData *)column->realloc(column->data, new_capacity * sizeof(QtbColumnData));
  if (new_data == NULL) {
    PyErr_SetString(PyExc_MemoryError, "failed to growth column");
    return false;
  }

  column->data = new_data;
  column->capacity = new_capacity;

  return true;
}

bool qtb_column_append(QtbColumn *column, PyObject *item) {
  if (column->capacity == column->size) {
    if (!qtb_column_grow(column))
      return false;
  }

  if (!column->append(column, item))
    return false;

  column->size++;
  return true;
}

const char *qtb_column_type_as_str(QtbColumn *column) {
  return column->type_as_str();
}

static bool qtb_column_type_init(QtbColumn *column, PyObject *type) {
  if (PyUnicode_CompareWithASCIIString(type, "str") == 0)
    column->type = QTB_COLUMN_TYPE_STR;
  else if (PyUnicode_CompareWithASCIIString(type, "int") == 0)
    column->type = QTB_COLUMN_TYPE_INT;
  else if (PyUnicode_CompareWithASCIIString(type, "float") == 0)
    column->type = QTB_COLUMN_TYPE_FLOAT;
  else if (PyUnicode_CompareWithASCIIString(type, "bool") == 0)
    column->type = QTB_COLUMN_TYPE_BOOL;
  else {
    PyErr_SetString(PyExc_RuntimeError, "invalid column type");
    return false;
  }

  return true;
}

QtbColumn *_qtb_column_new_many(size_t n, mallocer m) {
  QtbColumn *columns;

  columns = (QtbColumn *)(*m)(sizeof(QtbColumn) * n);
  if (columns == NULL)
    return NULL;

  for (size_t i = 0; i < n; i++) {
    columns[i].strdup = &strdup;
    columns[i].malloc = &malloc;
    columns[i].realloc = &realloc;
    columns[i].PyTuple_New = &PyTuple_New;
    columns[i].PyUnicode_AsUTF8 = &PyUnicode_AsUTF8;
    columns[i].name = NULL;
    columns[i].data = NULL;
  }

  return columns;
}

static bool qtb_column_init_name(QtbColumn *column, PyObject *name) {
  char *name_s;

  name_s = (*column->PyUnicode_AsUTF8)(name);
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

  column->size = 0;
  column->capacity = QTB_COLUMN_INITIAL_CAPACITY;

  fast_descriptor = PySequence_Fast(descriptor, "descriptor not a sequence");
  if (fast_descriptor == NULL)
    return false;

  if (
    !qtb_column_init_name(column, PySequence_Fast_GET_ITEM(fast_descriptor, 0))
    || !qtb_column_type_init(column, PySequence_Fast_GET_ITEM(fast_descriptor, 1))
    || (column->data = (QtbColumnData *)malloc(sizeof(QtbColumnData) * QTB_COLUMN_INITIAL_CAPACITY)) == NULL
  ) {
    qtb_column_dealloc(column);
    success = false;
  }

  qtb_column_init_methods(column);

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

  if (column->size > 0)
    column->dealloc(column);

  free(column->data);
  column->data = NULL;
}

PyObject *qtb_column_as_descriptor(QtbColumn *column) {
  PyObject *descriptor = NULL;
  PyObject *name = NULL;
  PyObject *type = NULL;

  if (
    ((name = PyUnicode_FromString(column->name)) == NULL) ||
    ((type = PyUnicode_FromString(qtb_column_type_as_str(column))) == NULL) ||
    ((descriptor = column->PyTuple_New(2)) == NULL)
  ) {
    Py_XDECREF(name);
    Py_XDECREF(type);
    return NULL;
  }

  PyTuple_SET_ITEM(descriptor, 0, name);
  PyTuple_SET_ITEM(descriptor, 1, type);

  return descriptor;
}

char *qtb_column_header_repr(QtbColumn *column) {
  int size;
  char *header;

  size = 4 + strlen(column->name) + strlen(qtb_column_type_as_str(column));
  header = (char *)column->malloc(sizeof(char) * (size_t)size);
  if (header == NULL) {
    PyErr_SetString(PyExc_MemoryError, "failed allocate memory for header repr");
    return NULL;
  }

  sprintf(header, "%s (%s)", column->name, qtb_column_type_as_str(column));
  return header;
}

char *qtb_column_cell_repr(QtbColumn *column, size_t i) {
  return column->cell_repr(column, i);
}
