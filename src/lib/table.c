#include "table.h"
#include "result.h"

void qtb_table_zero(QtbTable *self) {
  self->size = 0;
  self->width = 0;
  self->columns = NULL;
}

void qtb_table_dealloc_(QtbTable *self) {
  for (Py_ssize_t i = 0; i < self->width; i++)
    qtb_column_dealloc(&self->columns[i]);

  free(self->columns);
}

Result qtb_table_init_(QtbTable *self, PyObject *blueprint) {
  Result result;

  result = qtb_validate_blueprint(blueprint);
  if (ResultFailed(result)) return result;

  self->width = PySequence_Size(blueprint);
  if (self->width == -1) return ResultFailureFromPyErr();

  self->columns = qtb_column_new_many((size_t)self->width);
  if (self->columns == NULL)
    return ResultFailure(PyExc_MemoryError, "failed to initialise table");

  result = qtb_column_init_many(self->columns, blueprint, self->width);
  if (ResultFailed(result)) free(self->columns);

  return result;
}

Py_ssize_t qtb_table_length(QtbTable *self) {
  return self->size;
}

ResultPyObjectPtr qtb_table_item_(QtbTable *self, Py_ssize_t i) {
  PyObject *row;
  ResultPyObjectPtr result;

  if (i >= self->size)
    return ResultPyObjectPtrFailure(PyExc_IndexError, "table index out of range");

  if ((row = PyList_New(self->width)) == NULL)
    return ResultPyObjectPtrFailureFromPyErr();

  for (Py_ssize_t j = 0; j < self->width; j++) {
    result = qtb_column_get_as_pyobject(&self->columns[j], i);
    if (ResultFailed(result)) {
      Py_DECREF(row);
      return result;
    }
    PyList_SET_ITEM(row, j, ResultValue(result));
  }

  return ResultPyObjectPtrSuccess(row);
}

Result qtb_table_append_(QtbTable *self, PyObject *row) {
  PyObject *fast_row;
  Result result = ResultSuccess();

  if (PySequence_Check(row) != 1)
    return ResultFailure(PyExc_TypeError, "append with non-sequence");

  if (PySequence_Size(row) != self->width) {
    if (PyErr_Occurred() == NULL)
      return ResultFailure(PyExc_TypeError, "append with mismatching row length");

    return ResultFailureFromPyErr();
  }

  fast_row = PySequence_Fast(row, "");
  if (row == NULL) return ResultFailureFromPyErr();

  for (Py_ssize_t i = 0; i < self->width; i++) {
    result = qtb_column_append(&self->columns[i], PySequence_Fast_GET_ITEM(fast_row, i));
    if (ResultFailed(result))
      break;
  }

  Py_DECREF(fast_row);
  if (ResultSuccessful(result)) self->size++;

  return result;
}

ResultPyObjectPtr qtb_table_pop_(QtbTable *self) {
  ResultPyObjectPtr result;

  if (self->size == 0)
    return ResultPyObjectPtrFailure(PyExc_IndexError, "pop from empty table");

  result = qtb_table_item_(self, self->size - 1);
  if (ResultFailed(result)) return result;

  self->size--;
  for (Py_ssize_t i = 0; i < self->width; i++)
    self->columns[i].size--;

  return result;
}

ResultPyObjectPtr qtb_table_blueprint_(QtbTable *self) {
  PyObject *blueprint;
  ResultPyObjectPtr result;

  blueprint = PyList_New(self->width);
  if (blueprint == NULL) return ResultPyObjectPtrFailureFromPyErr();

  for (Py_ssize_t i = 0; i < self->width; i++) {
    result = qtb_column_as_descriptor(&self->columns[i]);
    if (ResultFailed(result)) {
      Py_DECREF(blueprint);
      return result;
    }

    PyList_SET_ITEM(blueprint, i, ResultValue(result));
  }

  return ResultPyObjectPtrSuccess(blueprint);
}

static ResultSize_tPtr qtb_table_repr_get_column_widths(QtbTable *self) {
  size_t *widths;
  ResultSize_t result;

  widths = (size_t *)malloc(self->width * sizeof(size_t));
  if (widths == NULL)
    return ResultSize_tPtrFailure(PyExc_MemoryError, "could not allocate memory");

  for (size_t i = 0; i < (size_t)self->width; i++) {
    result = qtb_column_repr_longest_of_first_five(&self->columns[i]);
    if (ResultFailed(result)) {
      free(widths);
      return ResultSize_tPtrFailureFromResult(result);
    }
    widths[i] = ResultValue(result);
  }

  return ResultSize_tPtrSuccess(widths);
}

static ResultCharPtr qtb_table_repr_init(QtbTable *self, size_t *col_repr_widths) {
  size_t row_size = 2;
  char *repr;

  for (size_t i = 0; i < (size_t)self->width; i++)
    row_size += col_repr_widths[i] + 3;

  repr = (char *)malloc(row_size * (1 + MIN(self->size, 5)) * sizeof(char));
  if (repr == NULL)
    return ResultCharPtrFailure(PyExc_MemoryError, "could not allocate memory");

  repr[0] = '\0';

  return ResultCharPtrSuccess(repr);
}

static ResultCharPtr qtb_table_repr_formatted_cell(const char *repr) {
  char *formatted;
  size_t size;

  size = 3 + strlen(repr);

  formatted = (char *)malloc(1 + size * sizeof(char));
  if (formatted == NULL)
    return ResultCharPtrFailure(PyExc_MemoryError, "could not allocate memory");

  if (sprintf(formatted, "| %s ", repr) != (int)size) {
    free(formatted);
    return ResultCharPtrFailure(PyExc_RuntimeError, "could not write format string");
  }

  return ResultCharPtrSuccess(formatted);
}

static Result qtb_table_repr_cat_header(QtbTable *self, char *repr, size_t *paddings) {
  ResultCharPtr header_result;
  ResultCharPtr formatted_result;
  size_t header_len;

  for (size_t i = 0; i < (size_t)self->width; i++) {
    header_result = qtb_column_header_as_string(&self->columns[i]);
    if (ResultFailed(header_result))
      return ResultFailureFromResult(header_result);
    header_len = strlen(ResultValue(header_result));

    formatted_result = qtb_table_repr_formatted_cell(ResultValue(header_result));
    free(ResultValue(header_result));
    if (ResultFailed(formatted_result))
      return ResultFailureFromResult(formatted_result);

    strcat(repr, ResultValue(formatted_result));

    for (size_t j = 0; j < paddings[i] - header_len; j++)
      strcat(repr, " ");

    free(ResultValue(formatted_result));
  }

  strcat(repr, "|");
  return ResultSuccess();
}

static Result qtb_table_repr_cat_row(QtbTable *self, size_t row, char *repr, size_t *paddings) {
  ResultCharPtr cell_result;
  ResultCharPtr formatted_result;
  size_t cell_len;

  strcat(repr, "\n");

  for (size_t i = 0; i < (size_t)self->width; i++) {
    cell_result = qtb_column_cell_as_string(&self->columns[i], row);
    if (ResultFailed(cell_result))
      return ResultFailureFromResult(cell_result);
    cell_len = strlen(ResultValue(cell_result));

    formatted_result = qtb_table_repr_formatted_cell(ResultValue(cell_result));
    free(ResultValue(cell_result));
    if (ResultFailed(formatted_result))
      return ResultFailureFromResult(formatted_result);

    strcat(repr, ResultValue(formatted_result));

    for (size_t j = 0; j < paddings[i] - cell_len; j++)
      strcat(repr, " ");

    free(ResultValue(formatted_result));
  }

  strcat(repr, "|");
  return ResultSuccess();
}

static ResultPyObjectPtr qtb_empty_py_string() {
  PyObject *empty_string;

  empty_string = PyUnicode_FromString("");
  if (empty_string == NULL) return ResultPyObjectPtrFailureFromPyErr();

  return ResultPyObjectPtrSuccess(empty_string);
}

ResultPyObjectPtr qtb_table_as_string_(QtbTable *self) {
  PyObject *repr_py;
  ResultSize_tPtr widths_result;
  Result cat_result;
  ResultCharPtr repr_result;

  if (self->width == 0) return qtb_empty_py_string();

  widths_result = qtb_table_repr_get_column_widths(self);
  if (ResultFailed(widths_result)) return ResultPyObjectPtrFailureFromResult(widths_result);

  repr_result = qtb_table_repr_init(self, ResultValue(widths_result));
  if (ResultFailed(repr_result)) {
    free(ResultValue(widths_result));
    return ResultPyObjectPtrFailureFromResult(repr_result);
  }

  cat_result = qtb_table_repr_cat_header(self, ResultValue(repr_result), ResultValue(widths_result));
  if (ResultFailed(cat_result)) {
    free(ResultValue(widths_result));
    free(ResultValue(repr_result));
    return ResultPyObjectPtrFailureFromResult(cat_result);
  }

  for (Py_ssize_t i = 0; i < MIN(self->size, 5); i++) {
    cat_result = qtb_table_repr_cat_row(self, (size_t)i, ResultValue(repr_result), ResultValue(widths_result));
    if (ResultFailed(cat_result)) {
      free(ResultValue(widths_result));
      free(ResultValue(repr_result));
      return ResultPyObjectPtrFailureFromResult(cat_result);
    }
  }
  free(ResultValue(widths_result));
  repr_py = PyUnicode_FromString(ResultValue(repr_result));
  free(ResultValue(repr_result));

  if (repr_py == NULL) return ResultPyObjectPtrFailureFromPyErr();
  return ResultPyObjectPtrSuccess(repr_py);
}
