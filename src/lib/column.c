#include "column.h"
#include "result.h"

// ===== qtb_column_str =====

static ResultPyObjectPtr qtb_column_get_as_pyobject_str(QtbColumn *column, size_t i) {
  PyObject *str;

  str = PyUnicode_FromString(column->data[i].s);
  if (str == NULL)
    return ResultPyObjectPtrFailureFromPyErr();

  return ResultPyObjectPtrSuccess(str);
}

static Result qtb_column_append_str(QtbColumn *column, PyObject *item) {
  char *s;

  if (PyUnicode_Check(item) == 0)
    return ResultFailure(PyExc_TypeError, "non-str entry for str column");

  if ((s = (*column->PyUnicode_AsUTF8)(item)) == NULL)
    return ResultFailureFromPyErr();

  column->data[column->size].s = (*column->strdup)(s);
  if (column->data[column->size].s == NULL)
    return ResultFailure(PyExc_MemoryError, "could not create PyUnicodeobject");

  return ResultSuccess();
}

void qtb_column_dealloc_str(QtbColumn *column) {
  for (size_t i = 0; i < column->size; i++)
    free(column->data[i].s);
}

static const char *qtb_column_type_as_str_str() {
  return "str";
}

static ResultCharPtr qtb_column_cell_repr_str(QtbColumn *column, size_t i) {
  char *copy;

  copy = column->strdup(column->data[i].s);
  if (copy == NULL)
    return ResultCharPtrFailure(PyExc_MemoryError, "failed allocate memory for cell repr");

  return ResultCharPtrSuccess(copy);
}

// ===== qtb_column_int =====

static ResultPyObjectPtr qtb_column_get_as_pyobject_int(QtbColumn *column, size_t i) {
  PyObject *str;

  str = PyLong_FromLongLong(column->data[i].i);
  if (str == NULL)
    return ResultPyObjectPtrFailureFromPyErr();

  return ResultPyObjectPtrSuccess(str);
}

static Result qtb_column_append_int(QtbColumn *column, PyObject *item) {
  if (PyLong_Check(item) == 0)
    return ResultFailure(PyExc_TypeError, "non-int entry for int column");

  column->data[column->size].i = PyLong_AsLongLong(item);
  return ResultSuccess();
}

static const char *qtb_column_type_as_str_int() {
  return "int";
}

static ResultCharPtr qtb_column_cell_repr_int(QtbColumn *column, size_t i) {
  char *repr;
  size_t size;

  size = snprintf(NULL, 0, "%lld", column->data[i].i);

  repr = (char *)column->malloc(sizeof(char) * size + 1);
  if (repr == NULL)
    return ResultCharPtrFailure(PyExc_MemoryError, "failed allocate memory for cell repr");

  snprintf(repr, size + 1, "%lld", column->data[i].i);
  return ResultCharPtrSuccess(repr);
}

// ===== qtb_column_float =====

static ResultPyObjectPtr qtb_column_get_as_pyobject_float(QtbColumn *column, size_t i) {
  PyObject *str;

  str = PyFloat_FromDouble(column->data[i].f);
  if (str == NULL)
    return ResultPyObjectPtrFailureFromPyErr();

  return ResultPyObjectPtrSuccess(str);
}

static Result qtb_column_append_float(QtbColumn *column, PyObject *item) {
  if(PyFloat_Check(item) == 0)
    return ResultFailure(PyExc_TypeError, "non-float entry for float column");

  column->data[column->size].f = PyFloat_AsDouble(item);
  return ResultSuccess();
}

static const char *qtb_column_type_as_str_float() {
  return "float";
}

static ResultCharPtr qtb_column_cell_repr_float(QtbColumn *column, size_t i) {
  char *repr;
  size_t size;

  size = snprintf(NULL, 0, "%.2f", column->data[i].f);

  repr = (char *)column->malloc(sizeof(char) * size + 1);
  if (repr == NULL)
    return ResultCharPtrFailure(PyExc_MemoryError, "failed allocate memory for cell repr");

  snprintf(repr, size + 1, "%.2f", column->data[i].f);
  return ResultCharPtrSuccess(repr);
}

// ===== qtb_column_bool =====

static ResultPyObjectPtr qtb_column_get_as_pyobject_bool(QtbColumn *column, size_t i) {
  PyObject *str;

  str = PyBool_FromLong(column->data[i].b);
  if (str == NULL)
    return ResultPyObjectPtrFailureFromPyErr();

  return ResultPyObjectPtrSuccess(str);
}

static Result qtb_column_append_bool(QtbColumn *column, PyObject *item) {
  if (PyBool_Check(item) == 0)
    return ResultFailure(PyExc_TypeError, "non-bool entry for bool column");

  column->data[column->size].b = (bool)PyLong_AsLong(item);
  return ResultSuccess();
}

static const char *qtb_column_type_as_str_bool() {
  return "bool";
}

static ResultCharPtr qtb_column_cell_repr_bool(QtbColumn *column, size_t i) {
  char *repr;

  repr = column->strdup(column->data[i].b ? "True" : "False");
  if (repr == NULL)
    return ResultCharPtrFailure(PyExc_MemoryError, "failed allocate memory for cell repr");

  return ResultCharPtrSuccess(repr);
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

ResultPyObjectPtr qtb_column_get_as_pyobject(QtbColumn *column, size_t i) {
  return column->get_as_pyobject(column, i);
}

static Result qtb_column_grow(QtbColumn *column) {
  size_t new_capacity;
  QtbColumnData *new_data;

  new_capacity = QTB_COLUMN_GROWTH_COEFFICIENT * column->capacity;
  new_data = (QtbColumnData *)column->realloc(column->data, new_capacity * sizeof(QtbColumnData));
  if (new_data == NULL)
    return ResultFailure(PyExc_MemoryError, "failed to grow column");

  column->data = new_data;
  column->capacity = new_capacity;

  return ResultSuccess();
}

Result qtb_column_append(QtbColumn *column, PyObject *item) {
  Result result;

  if (column->capacity == column->size) {
    result = qtb_column_grow(column);
    if (ResultFailed(result))
      return result;
  }

  result = column->append(column, item);
  if (ResultFailed(result))
    return result;

  column->size++;
  return ResultSuccess();
}

const char *qtb_column_type_as_str(QtbColumn *column) {
  return column->type_as_str();
}

static Result qtb_column_type_init(QtbColumn *column, PyObject *type) {
  if (PyUnicode_CompareWithASCIIString(type, "str") == 0)
    column->type = QTB_COLUMN_TYPE_STR;
  else if (PyUnicode_CompareWithASCIIString(type, "int") == 0)
    column->type = QTB_COLUMN_TYPE_INT;
  else if (PyUnicode_CompareWithASCIIString(type, "float") == 0)
    column->type = QTB_COLUMN_TYPE_FLOAT;
  else if (PyUnicode_CompareWithASCIIString(type, "bool") == 0)
    column->type = QTB_COLUMN_TYPE_BOOL;
  else {
    return ResultFailure(PyExc_RuntimeError, "invalid column type");
  }

  return ResultSuccess();
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

static Result qtb_column_init_name(QtbColumn *column, PyObject *name) {
  char *name_s;

  name_s = (*column->PyUnicode_AsUTF8)(name);
  if (name == NULL)
    return ResultFailureFromPyErr();

  column->name = (*column->strdup)(name_s);
  if (column->name == NULL)
    return ResultFailure(PyExc_MemoryError, "failed to initialise column");

  return ResultSuccess();
}

Result qtb_column_init(QtbColumn *column, PyObject *descriptor) {
  PyObject *fast_descriptor;
  Result result;

  column->size = 0;
  column->capacity = QTB_COLUMN_INITIAL_CAPACITY;

  fast_descriptor = PySequence_Fast(descriptor, "descriptor not a sequence");
  if (fast_descriptor == NULL)
    return ResultFailureFromPyErr();

  result = qtb_column_init_name(column, PySequence_Fast_GET_ITEM(fast_descriptor, 0));
  if (ResultFailed(result)) {
    qtb_column_dealloc(column);
    Py_DECREF(fast_descriptor);
    return result;
  }

  result = qtb_column_type_init(column, PySequence_Fast_GET_ITEM(fast_descriptor, 1));
  if (ResultFailed(result)) {
    qtb_column_dealloc(column);
    Py_DECREF(fast_descriptor);
    return result;
  }

  column->data = (QtbColumnData *)malloc(sizeof(QtbColumnData) * QTB_COLUMN_INITIAL_CAPACITY);
  Py_DECREF(fast_descriptor);
  if (column->data == NULL) {
    qtb_column_dealloc(column);
    return ResultFailure(PyExc_MemoryError, "failed to initialise column");
  }

  qtb_column_init_methods(column);
  return ResultSuccess();
}

Result qtb_column_init_many(QtbColumn *columns, PyObject *blueprint, Py_ssize_t n) {
  PyObject *fast_blueprint = NULL;
  Result result;
  Py_ssize_t i;

  if ((fast_blueprint = PySequence_Fast(blueprint, "failed to initialise table")) == NULL)
    return ResultFailureFromPyErr();

  for (i = 0; i < n; i++) {
    result = qtb_column_init(&columns[i], PySequence_Fast_GET_ITEM(fast_blueprint, i));
    if (ResultFailed(result)) {
      Py_DECREF(fast_blueprint);
      for (Py_ssize_t j = i - 1; i >= 0; i--)
        qtb_column_dealloc(&columns[j]);
      return result;
    }
  }

  Py_DECREF(fast_blueprint);
  return ResultSuccess();
}

void qtb_column_dealloc(QtbColumn *column) {
  free(column->name);
  column->name = NULL;

  if (column->size > 0)
    column->dealloc(column);

  free(column->data);
  column->data = NULL;
}

ResultPyObjectPtr qtb_column_as_descriptor(QtbColumn *column) {
  PyObject *descriptor;
  PyObject *name;
  PyObject *type;

  name = PyUnicode_FromString(column->name);
  if (name == NULL)
    return ResultPyObjectPtrFailureFromPyErr();

  type = PyUnicode_FromString(qtb_column_type_as_str(column));
  if (type == NULL) {
    Py_DECREF(name);
    return ResultPyObjectPtrFailureFromPyErr();
  }

  descriptor = column->PyTuple_New(2);
  if (descriptor == NULL) {
    Py_DECREF(name);
    Py_DECREF(type);
    return ResultPyObjectPtrFailureFromPyErr();
  }

  PyTuple_SET_ITEM(descriptor, 0, name);
  PyTuple_SET_ITEM(descriptor, 1, type);

  return ResultPyObjectPtrSuccess(descriptor);
}

ResultCharPtr qtb_column_header_repr(QtbColumn *column) {
  int size;
  char *repr;

  size = 4 + strlen(column->name) + strlen(qtb_column_type_as_str(column));
  repr = (char *)column->malloc(sizeof(char) * (size_t)size);
  if (repr == NULL)
    return ResultCharPtrFailure(PyExc_MemoryError, "failed allocate memory for header repr");

  sprintf(repr, "%s (%s)", column->name, qtb_column_type_as_str(column));
  return ResultCharPtrSuccess(repr);
}

ResultCharPtr qtb_column_cell_repr(QtbColumn *column, size_t i) {
  return column->cell_repr(column, i);
}

ResultSize_t qtb_column_repr_longest_of_first_five(QtbColumn *column) {
  size_t size;
  ResultCharPtr result;

  result = qtb_column_header_repr(column);
  if (ResultFailed(result))
    return ResultSize_tFailureFromResult(result);

  size = strlen(ResultValue(result));
  free(ResultValue(result));

  for (size_t i = 0; i < MIN(column->size, 5); i++) {
    result = qtb_column_cell_repr(column, i);
    if (ResultFailed(result))
      return ResultSize_tFailureFromResult(result);

    size = MAX(strlen(ResultValue(result)), size);
    free(ResultValue(result));
  }

  return ResultSize_tSuccess(size);
}
