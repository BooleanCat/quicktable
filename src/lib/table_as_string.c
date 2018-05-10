#include <Python.h>
#include "table.h"
#include "result.h"

static ResultSize_tPtr qtb_table_as_string_paddings(QtbTable *self) {
  size_t *paddings;
  ResultSize_t result;

  paddings = (size_t *)malloc(self->width * sizeof(size_t));
  if (paddings == NULL) return ResultSize_tPtrFailure(PyExc_MemoryError, "memory error");

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
  if (string == NULL) return ResultCharPtrFailure(PyExc_MemoryError, "could not allocate memory");

  string[0] = '\0';

  return ResultCharPtrSuccess(string);
}

static size_t qtb_table_as_string_write_formatted(char *string, char *cell, size_t padding) {
  size_t cell_len;
  size_t spaces;

  cell_len = strlen(cell);
  spaces = 1 + padding - cell_len;

  strncpy(string, "| ", 2);
  strncpy(&string[2], cell, cell_len);

  for (size_t i = 0; i < spaces; i++)
    string[2 + cell_len + i] = ' ';

  return 2 + cell_len + spaces;
}

static ResultSize_t qtb_table_as_string_append_header(QtbTable *self, char *string, size_t *paddings) {
  ResultCharPtr header;
  size_t string_size = 0;

  for (size_t i = 0; i < (size_t)self->width; i++) {
    header = qtb_column_header_as_string(&self->columns[i]);
    if (ResultFailed(header)) return ResultSize_tFailureFromResult(header);

    string_size += qtb_table_as_string_write_formatted(&string[string_size], ResultValue(header), paddings[i]);
    free(ResultValue(header));
  }

  string[string_size] = '|';
  return ResultSize_tSuccess(string_size + 1);
}

static ResultSize_t qtb_table_as_string_append_row(QtbTable *self, size_t row, char *string, size_t *paddings) {
  ResultCharPtr cell;
  size_t string_size = 1;

  string[0] = '\n';

  for (size_t i = 0; i < (size_t)self->width; i++) {
    cell = qtb_column_cell_as_string(&self->columns[i], row);
    if (ResultFailed(cell)) return ResultSize_tFailureFromResult(cell);

    string_size += qtb_table_as_string_write_formatted(&string[string_size], ResultValue(cell), paddings[i]);
    free(ResultValue(cell));
  }

  string[string_size] = '|';
  return ResultSize_tSuccess(string_size + 1);
}

static ResultSize_t qtb_table_as_string_append_rows(QtbTable *self, char *string, size_t *paddings) {
  ResultSize_t written;
  size_t string_size = 0;

  for (Py_ssize_t i = 0; i < MIN(self->size, 5); i++) {
    written = qtb_table_as_string_append_row(self, (size_t)i, &string[string_size], paddings);
    if (ResultFailed(written)) return written;
    string_size += ResultValue(written);
  }

  return ResultSize_tSuccess(string_size);
}

static ResultPyObjectPtr qtb_table_as_string_empty() {
  PyObject *empty_string;

  empty_string = PyUnicode_FromString("");
  if (empty_string == NULL) return ResultPyObjectPtrFailureFromPyErr();

  return ResultPyObjectPtrSuccess(empty_string);
}

static ResultCharPtr qtb_table_as_string(QtbTable *self) {
  ResultSize_tPtr paddings;
  ResultSize_t written;
  size_t string_size = 0;
  ResultCharPtr string;

  paddings = qtb_table_as_string_paddings(self);
  if (ResultFailed(paddings)) return ResultCharPtrFailureFromResult(paddings);

  string = qtb_table_as_string_init(self, ResultValue(paddings));
  if (ResultFailed(string)) {
    free(ResultValue(paddings));
    return string;
  }

  written = qtb_table_as_string_append_header(self, ResultValue(string), ResultValue(paddings));
  if (ResultFailed(written)) {
    free(ResultValue(paddings));
    free(ResultValue(string));
    return ResultCharPtrFailureFromResult(written);
  }
  string_size += ResultValue(written);

  written = qtb_table_as_string_append_rows(self, &(ResultValue(string))[string_size], ResultValue(paddings));
  free(ResultValue(paddings));
  if (ResultFailed(written)) {
    free(ResultValue(string));
    return ResultCharPtrFailureFromResult(written);
  }
  string_size += ResultValue(written);
  ResultValue(string)[string_size] = '\0';

  return string;
}

ResultPyObjectPtr qtb_table_as_py_string_(QtbTable *self) {
  PyObject *py_string;
  ResultCharPtr string;

  if (self->width == 0) return qtb_table_as_string_empty();

  string = qtb_table_as_string(self);
  if (ResultFailed(string)) return ResultPyObjectPtrFailureFromResult(string);

  py_string = PyUnicode_FromString(ResultValue(string));
  free(ResultValue(string));

  if (py_string == NULL) return ResultPyObjectPtrFailureFromPyErr();
  return ResultPyObjectPtrSuccess(py_string);
}
