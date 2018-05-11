#include "table.h"
#include "result.h"

static ResultQtbColumnPtr column_new_many(size_t size) {
  return qtb_column_new_many(size);
}

void qtb_table_new_(QtbTable *self) {
  self->size = 0;
  self->width = 0;
  self->columns = NULL;

  self->PySequence_Size = &PySequence_Size;
  self->PyList_New = &PyList_New;
  self->column_new_many = &column_new_many;
  self->column_init_many = &qtb_column_init_many;
  self->column_as_descriptor = &qtb_column_as_descriptor;
}

void qtb_table_dealloc_(QtbTable *self) {
  for (Py_ssize_t i = 0; i < self->width; i++)
    qtb_column_dealloc(&self->columns[i]);

  free(self->columns);
}

Result qtb_table_init_(QtbTable *self, PyObject *blueprint) {
  Result result;
  ResultQtbColumnPtr columns;

  result = qtb_blueprint_validate(blueprint);
  if (ResultFailed(result)) return result;

  self->width = self->PySequence_Size(blueprint);
  if (self->width == -1) return ResultFailureFromPyErr();

  columns = self->column_new_many((size_t)self->width);
  if (ResultFailed(columns)) return ResultFailureFromResult(columns);
  self->columns = ResultValue(columns);

  result = self->column_init_many(self->columns, blueprint, self->width);
  if (ResultFailed(result)) free(self->columns);

  return result;
}

Py_ssize_t qtb_table_length(QtbTable *self) {
  return self->size;
}

ResultPyObjectPtr qtb_table_item_(QtbTable *self, Py_ssize_t i) {
  PyObject *row;
  ResultPyObjectPtr result;

  if (i >= self->size) return ResultPyObjectPtrFailure(PyExc_IndexError, "table index out of range");

  row = PyList_New(self->width);
  if (row == NULL) return ResultPyObjectPtrFailureFromPyErr();

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
  int row_size;
  Result result = ResultSuccess();

  if (PySequence_Check(row) != 1) return ResultFailure(PyExc_TypeError, "append with non-sequence");

  row_size = PySequence_Size(row);
  if (row_size < 0) return ResultFailureFromPyErr();
  if (row_size != self->width) return ResultFailure(PyExc_TypeError, "append with mismatching row length");

  fast_row = PySequence_Fast(row, "");
  if (fast_row == NULL) return ResultFailureFromPyErr();

  for (Py_ssize_t i = 0; i < self->width; i++) {
    result = qtb_column_append(&self->columns[i], PySequence_Fast_GET_ITEM(fast_row, i));
    if (ResultFailed(result)) break;
  }

  Py_DECREF(fast_row);
  if (ResultSuccessful(result)) self->size++;

  return result;
}

ResultPyObjectPtr qtb_table_pop_(QtbTable *self) {
  ResultPyObjectPtr result;

  if (self->size == 0) return ResultPyObjectPtrFailure(PyExc_IndexError, "pop from empty table");

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

  blueprint = self->PyList_New(self->width);
  if (blueprint == NULL) return ResultPyObjectPtrFailureFromPyErr();

  for (Py_ssize_t i = 0; i < self->width; i++) {
    result = self->column_as_descriptor(&self->columns[i]);
    if (ResultFailed(result)) {
      Py_DECREF(blueprint);
      return result;
    }

    PyList_SET_ITEM(blueprint, i, ResultValue(result));
  }

  return ResultPyObjectPtrSuccess(blueprint);
}
