#include "table.h"
#include "result.h"

static PyObject *
qtb_table_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
  QtbTable *self;

  self = (QtbTable *)type->tp_alloc(type, 0);
  if (self != NULL) {
    self->size = 0;
    self->width = 0;
    self->columns = NULL;
  }

  return (PyObject *)self;
}

static void qtb_table_dealloc(QtbTable *self) {
  for (Py_ssize_t i = 0; i < self->width; i++)
    qtb_column_dealloc(&self->columns[i]);

  free(self->columns);
  Py_TYPE(self)->tp_free((PyObject *)self);
}

static int qtb_table_init(QtbTable *self, PyObject *args, PyObject *kwargs) {
  PyObject *blueprint = NULL;
  Result result;

  if (!PyArg_ParseTuple(args, "O", &blueprint))
    return -1;

  result = qtb_validate_blueprint(blueprint);
  if (ResultFailed(result)) {
    ResultFailureRaise(result);
    return -1;
  }

  self->width = PySequence_Size(blueprint);
  if (self->width == -1)
    return -1;

  self->columns = qtb_column_new_many((size_t)self->width);
  if (self->columns == NULL) {
    PyErr_SetString(PyExc_MemoryError, "failed to initialise table");
    return -1;
  }

  result = qtb_column_init_many(self->columns, blueprint, self->width);
  if (ResultFailed(result)) {
    free(self->columns);
    ResultFailureRaise(result);
    return -1;
  }

  return 0;
}

static Py_ssize_t qtb_table_length(QtbTable *self) {
  return self->size;
}

static PyObject *qtb_table_item(QtbTable *self, Py_ssize_t i) {
  PyObject *row;
  ResultPyObjectPtr result;

  if (i >= self->size) {
    PyErr_SetString(PyExc_IndexError, "table index out of range");
    return NULL;
  }

  if ((row = PyList_New(self->width)) == NULL)
    return NULL;

  for (Py_ssize_t j = 0; j < self->width; j++) {
    result = qtb_column_get_as_pyobject(&self->columns[j], i);
    if (ResultFailed(result)) {
      Py_DECREF(row);
      ResultFailureRaise(result);
      return NULL;
    }
    PyList_SET_ITEM(row, j, ResultValue(result));
  }

  return row;
}

static PySequenceMethods qtb_table_as_sequence = {
  (lenfunc)qtb_table_length,  // sq_length
  0,  // sq_concat
  0,  // sq_repeat
  (ssizeargfunc)qtb_table_item,  // sq_item
  0,  // sq_slice
  0,  // sq_ass_item
  0,  // sq_ass_slice
  0,  // sq_contains
  0,  // sq_inplace_concat
  0,  // sq_inplace_repeat
};

static PyObject *qtb_table_append(QtbTable *self, PyObject *row) {
  PyObject *fast_row;
  Result result = ResultSuccess();

  if (PySequence_Check(row) != 1) {
    PyErr_SetString(PyExc_TypeError, "append with non-sequence");
    return NULL;
  }

  if (PySequence_Size(row) != self->width) {
    if (PyErr_Occurred() == NULL)
      PyErr_SetString(PyExc_TypeError, "append with mismatching row length");

    return NULL;
  }

  fast_row = PySequence_Fast(row, "");
  if (row == NULL)
    return NULL;

  for (Py_ssize_t i = 0; i < self->width; i++) {
    result = qtb_column_append(&self->columns[i], PySequence_Fast_GET_ITEM(fast_row, i));
    if (ResultFailed(result))
      break;
  }

  Py_DECREF(fast_row);

  if (ResultFailed(result)) {
    ResultFailureRaise(result);
    return NULL;
  }

  self->size++;
  Py_RETURN_NONE;
}

static PyObject *qtb_table_pop(QtbTable *self) {
  PyObject *row;

  if (self->size == 0) {
    PyErr_SetString(PyExc_IndexError, "pop from empty table");
    return NULL;
  }

  row = qtb_table_item(self, self->size - 1);
  if (row == NULL)
    return NULL;

  self->size--;
  for (Py_ssize_t i = 0; i < self->width; i++)
    self->columns[i].size--;

  return row;
}

static PyMethodDef qtb_table_methods[] = {
  {"append", (PyCFunction)qtb_table_append, METH_O, "append"},
  {"pop", (PyCFunction)qtb_table_pop, METH_NOARGS, "pop"},
  {NULL, NULL}
};

static PyObject *qtb_table_blueprint(QtbTable *self, void *closure) {
  PyObject *blueprint;
  ResultPyObjectPtr result;

  blueprint = PyList_New(self->width);
  if (blueprint == NULL)
    return NULL;

  for (Py_ssize_t i = 0; i < self->width; i++) {
    result = qtb_column_as_descriptor(&self->columns[i]);
    if (ResultFailed(result)) {
      Py_DECREF(blueprint);
      ResultFailureRaise(result);
      return NULL;
    }

    PyList_SET_ITEM(blueprint, i, ResultValue(result));
  }

  return blueprint;
}

static PyGetSetDef qtb_table_getsetters[] = {
  {"blueprint", (getter)qtb_table_blueprint, NULL, "copy of table's blueprint", NULL},
  {NULL}
};

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

static PyObject *qtb_table_tp_repr(QtbTable *self) {
  PyObject *repr_py;
  ResultSize_tPtr widths_result;
  Result cat_result;
  ResultCharPtr repr_result;

  if (self->width == 0)
    return PyUnicode_FromString("");

  widths_result = qtb_table_repr_get_column_widths(self);
  if (ResultFailed(widths_result)) {
    ResultFailureRaise(widths_result);
    return NULL;
  }

  repr_result = qtb_table_repr_init(self, ResultValue(widths_result));
  if (ResultFailed(repr_result)) {
    ResultFailureRaise(repr_result);
    return NULL;
  }

  cat_result = qtb_table_repr_cat_header(self, ResultValue(repr_result), ResultValue(widths_result));
  if (ResultFailed(cat_result)) {
    free(ResultValue(repr_result));
    ResultFailureRaise(cat_result);
    return NULL;
  }

  for (Py_ssize_t i = 0; i < MIN(self->size, 5); i++) {
    cat_result = qtb_table_repr_cat_row(self, (size_t)i, ResultValue(repr_result), ResultValue(widths_result));
    if (ResultFailed(cat_result)) {
      free(ResultValue(repr_result));
      ResultFailureRaise(cat_result);
      return NULL;
    }
  }
  free(ResultValue(widths_result));

  repr_py = PyUnicode_FromString(ResultValue(repr_result));
  free(ResultValue(repr_result));
  return repr_py;
}

PyTypeObject QtbTableType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "quicktable.Table",  // tp_name
    sizeof(QtbTable),  // tp_basicsize
    0,  // tp_itemsize
    (destructor)qtb_table_dealloc,  // tp_dealloc
    0,  // tp_print
    0,  // tp_getattr
    0,  // tp_setattr
    0,  // tp_reserved
    (reprfunc)qtb_table_tp_repr,  // tp_repr
    0,  // tp_as_number
    &qtb_table_as_sequence,  // tp_as_sequence
    0,  // tp_as_mapping
    0,  // tp_hash
    0,  // tp_call
    0,  // tp_str
    0,  // tp_getattro
    0,  // tp_setattro
    0,  // tp_as_buffer
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,  // tp_flags
    "Table",  // tp_doc
    0,  // tp_traverse
    0,  // tp_clear
    0,  // tp_richcompare
    0,  // tp_weaklistoffset
    0,  // tp_iter
    0,  // tp_iternext
    qtb_table_methods,  // tp_methods
    0,  // tp_members
    qtb_table_getsetters,  // tp_getset
    0,  // tp_base
    0,  // tp_dict
    0,  // tp_descr_get
    0,  // tp_descr_set
    0,  // tp_dictoffset
    (initproc)qtb_table_init,  // tp_init
    0,  // tp_alloc
    qtb_table_new  // tp_new
};
