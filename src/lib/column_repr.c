#include <stdlib.h>
#include "column_repr.h"

ResultCharPtr qtb_column_str_cell_as_string(QtbColumn *column, size_t i) {
  char *copy;

  copy = column->strdup(column->data[i].s);
  if (copy == NULL)
    return ResultCharPtrFailure(PyExc_MemoryError, "memory error");

  return ResultCharPtrSuccess(copy);
}

ResultCharPtr qtb_column_int_cell_as_string(QtbColumn *column, size_t i) {
  char *repr;
  int size;

  size = column->snprintf_(NULL, 0, "%lld", column->data[i].i);
  if (size < 0)
    return ResultCharPtrFailure(PyExc_RuntimeError, "failed to get string length of cell");

  repr = (char *)column->malloc(sizeof(char) * (size + 1));
  if (repr == NULL)
    return ResultCharPtrFailure(PyExc_MemoryError, "memory error");

  if (column->snprintf_(repr, size + 1, "%lld", column->data[i].i) != size) {
    free(repr);
    return ResultCharPtrFailure(PyExc_RuntimeError, "failed to write cell as string");
  }

  return ResultCharPtrSuccess(repr);
}

ResultCharPtr qtb_column_float_cell_as_string(QtbColumn *column, size_t i) {
  char *repr;
  int size;

  size = column->snprintf_(NULL, 0, "%.2f", column->data[i].f);
  if (size < 0)
    return ResultCharPtrFailure(PyExc_RuntimeError, "failed to get string length of cell");

  repr = (char *)column->malloc(sizeof(char) * (size + 1));
  if (repr == NULL)
    return ResultCharPtrFailure(PyExc_MemoryError, "memory error");

  if (column->snprintf_(repr, size + 1, "%.2f", column->data[i].f) != size) {
    free(repr);
    return ResultCharPtrFailure(PyExc_RuntimeError, "failed to write cell as string");
  }

  return ResultCharPtrSuccess(repr);
}

ResultCharPtr qtb_column_bool_cell_as_string(QtbColumn *column, size_t i) {
  char *repr;

  repr = column->strdup(column->data[i].b ? "True" : "False");
  if (repr == NULL)
    return ResultCharPtrFailure(PyExc_MemoryError, "memory error");

  return ResultCharPtrSuccess(repr);
}

const char *qtb_column_str_type_as_string() {
  return "str";
}

const char *qtb_column_int_type_as_string() {
  return "int";
}

const char *qtb_column_float_type_as_string() {
  return "float";
}

const char *qtb_column_bool_type_as_string() {
  return "bool";
}

ResultCharPtr qtb_column_header_as_string_(QtbColumn *column) {
  int size;
  char *string;

  size = 3 + strlen(column->name) + strlen(qtb_column_type_as_string(column));
  string = (char *)column->malloc(sizeof(char) * (size + 1));
  if (string == NULL)
    return ResultCharPtrFailure(PyExc_MemoryError, "memory error");

  if (column->snprintf_(string, size + 1, "%s (%s)", column->name, qtb_column_type_as_string(column)) != size) {
    free(string);
    return ResultCharPtrFailure(PyExc_RuntimeError, "failed to write header as string");
  }
  return ResultCharPtrSuccess(string);
}

ResultSize_t qtb_column_repr_longest_of_first_five_(QtbColumn *column) {
  size_t size;
  ResultCharPtr result;

  result = qtb_column_header_as_string(column);
  if (ResultFailed(result))
    return ResultSize_tFailureFromResult(result);

  size = strlen(ResultValue(result));
  free(ResultValue(result));

  for (size_t i = 0; i < MIN(column->size, 5); i++) {
    result = qtb_column_cell_as_string(column, i);
    if (ResultFailed(result))
      return ResultSize_tFailureFromResult(result);

    size = MAX(strlen(ResultValue(result)), size);
    free(ResultValue(result));
  }

  return ResultSize_tSuccess(size);
}
