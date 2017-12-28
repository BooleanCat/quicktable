#include <Python.h>
#include "table.h"
#include "result.h"

static ResultSize_tPtr qtb_table_as_string_paddings(QtbTable *self) {
  size_t *paddings;
  ResultSize_t result;

  paddings = (size_t *)malloc(self->width * sizeof(size_t));
  if (paddings == NULL)
    return ResultSize_tPtrFailure(PyExc_MemoryError, "memory error");

  for (size_t i = 0; i < (size_t)self->width; i++) {
    result = qtb_column_repr_longest_of_first_five(&self->columns[i]);
    if (ResultFailed(result)) {
      free(paddings);
      return ResultSize_tPtrFailureFromResult(result);
    }
    paddings[i] = ResultValue(result);
  }

  return ResultSize_tPtrSuccess(paddings);
}

static ResultCharPtr qtb_table_as_string_init(QtbTable *self, size_t *cell_widths) {
  size_t row_size = 2;
  char *string;

  for (size_t i = 0; i < (size_t)self->width; i++)
    row_size += cell_widths[i] + 3;

  string = (char *)malloc(row_size * (1 + MIN(self->size, 5)) * sizeof(char));
  if (string == NULL)
    return ResultCharPtrFailure(PyExc_MemoryError, "could not allocate memory");

  string[0] = '\0';

  return ResultCharPtrSuccess(string);
}

static void qtb_table_as_string_append_formatted(char *string, char *cell, size_t padding) {
  size_t end;
  size_t cell_len;
  size_t spaces;

  end = strlen(string);
  cell_len = strlen(cell);
  spaces = 1 + padding - cell_len;

  strncpy(&string[end], "| ", 2);
  strncpy(&string[end + 2], cell, cell_len);

  end += 2 + cell_len;

  for (size_t i = 0; i < spaces; i++)
    string[end + i] = ' ';
  string[end + spaces] = '\0';
}

static Result qtb_table_as_string_append_header(QtbTable *self, char *string, size_t *paddings) {
  ResultCharPtr header;

  for (size_t i = 0; i < (size_t)self->width; i++) {
    header = qtb_column_header_as_string(&self->columns[i]);
    if (ResultFailed(header))
      return ResultFailureFromResult(header);

    qtb_table_as_string_append_formatted(string, ResultValue(header), paddings[i]);
    free(ResultValue(header));
  }

  strcat(string, "|");
  return ResultSuccess();
}

static Result qtb_table_as_string_append_row(QtbTable *self, size_t row, char *string, size_t *paddings) {
  ResultCharPtr cell;

  strcat(string, "\n");

  for (size_t i = 0; i < (size_t)self->width; i++) {
    cell = qtb_column_cell_as_string(&self->columns[i], row);
    if (ResultFailed(cell))
      return ResultFailureFromResult(cell);

    qtb_table_as_string_append_formatted(string, ResultValue(cell), paddings[i]);
    free(ResultValue(cell));
  }

  strcat(string, "|");
  return ResultSuccess();
}

static Result qtb_table_as_string_append_rows(QtbTable *self, char *string, size_t *paddings) {
  Result result;

  for (Py_ssize_t i = 0; i < MIN(self->size, 5); i++) {
    result = qtb_table_as_string_append_row(self, (size_t)i, string, paddings);
    if (ResultFailed(result)) return result;
  }

  return ResultSuccess();
}

static ResultPyObjectPtr qtb_table_as_string_empty() {
  PyObject *empty_string;

  empty_string = PyUnicode_FromString("");
  if (empty_string == NULL) return ResultPyObjectPtrFailureFromPyErr();

  return ResultPyObjectPtrSuccess(empty_string);
}

ResultPyObjectPtr qtb_table_as_string_(QtbTable *self) {
  PyObject *string_py;
  ResultSize_tPtr paddings;
  Result result;
  ResultCharPtr string;

  if (self->width == 0) return qtb_table_as_string_empty();

  paddings = qtb_table_as_string_paddings(self);
  if (ResultFailed(paddings)) return ResultPyObjectPtrFailureFromResult(paddings);

  string = qtb_table_as_string_init(self, ResultValue(paddings));
  if (ResultFailed(string)) {
    free(ResultValue(paddings));
    return ResultPyObjectPtrFailureFromResult(string);
  }

  result = qtb_table_as_string_append_header(self, ResultValue(string), ResultValue(paddings));
  if (ResultFailed(result)) {
    free(ResultValue(paddings));
    free(ResultValue(string));
    return ResultPyObjectPtrFailureFromResult(result);
  }

  result = qtb_table_as_string_append_rows(self, ResultValue(string), ResultValue(paddings));
  if (ResultFailed(result)) {
    free(ResultValue(paddings));
    free(ResultValue(string));
    return ResultPyObjectPtrFailureFromResult(result);
  }

  free(ResultValue(paddings));
  string_py = PyUnicode_FromString(ResultValue(string));
  free(ResultValue(string));

  if (string_py == NULL) return ResultPyObjectPtrFailureFromPyErr();
  return ResultPyObjectPtrSuccess(string_py);
}
