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

  if (!PyArg_ParseTuple(args, "O", &blueprint))
    return -1;

  if (qtb_validate_blueprint(blueprint) == false)
    return -1;

  self->width = PySequence_Size(blueprint);
  if (self->width == -1)
    return -1;

  self->columns = qtb_column_new_many((size_t)self->width);
  if (self->columns == NULL) {
    PyErr_SetString(PyExc_MemoryError, "failed to initialise table");
    return -1;
  }

  if (qtb_column_init_many(self->columns, blueprint, self->width) == false) {
    free(self->columns);
    return -1;
  }

  return 0;
}

static Py_ssize_t qtb_table_length(QtbTable *self) {
  return self->size;
}

static PyObject *qtb_table_item(QtbTable *self, Py_ssize_t i) {
  PyObject *row;

  if (i >= self->size) {
    PyErr_SetString(PyExc_IndexError, "table index out of range");
    return NULL;
  }

  if ((row = PyList_New(self->width)) == NULL)
    return NULL;

  for (Py_ssize_t j = 0; j < self->width; j++) {
    PyList_SET_ITEM(row, j, qtb_column_get_as_pyobject(&self->columns[j], i));
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
  QtbResult result = QtbResultSuccess;

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
    if (QtbResultFailed(result))
      break;
  }

  Py_DECREF(fast_row);

  if (QtbResultFailed(result)) {
    QtbResultFailureRaise(result);
    return NULL;
  }

  self->size++;
  Py_RETURN_NONE;
}

static PyObject *qtb_table_pop(QtbTable *self) {
  if (self->size == 0) {
    PyErr_SetString(PyExc_IndexError, "pop from empty table");
    return NULL;
  }

  self->size--;
  Py_RETURN_NONE;
}

static PyMethodDef qtb_table_methods[] = {
  {"append", (PyCFunction)qtb_table_append, METH_O, "append"},
  {"pop", (PyCFunction)qtb_table_pop, METH_NOARGS, "pop"},
  {NULL, NULL}
};

static PyObject *qtb_table_blueprint(QtbTable *self, void *closure) {
  PyObject *blueprint = NULL;
  PyObject *descriptor = NULL;

  blueprint = PyList_New(self->width);
  if (blueprint == NULL)
    return NULL;

  for (Py_ssize_t i = 0; i < self->width; i++) {
    descriptor = qtb_column_as_descriptor(&self->columns[i]);
    if (descriptor == NULL) {
      Py_DECREF(blueprint);
      return NULL;
    }

    PyList_SET_ITEM(blueprint, i, descriptor);
  }

  return blueprint;
}

static PyGetSetDef qtb_table_getsetters[] = {
  {"blueprint", (getter)qtb_table_blueprint, NULL, "copy of table's blueprint", NULL},
  {NULL}
};

static int *qtb_table_repr_get_column_widths(QtbTable *self) {
  int *col_repr_widths;

  col_repr_widths = (int *)malloc(self->width * sizeof(int));
  if (col_repr_widths == NULL) {
    PyErr_SetString(PyExc_MemoryError, "could not allocate memory");
    return NULL;
  }

  for (size_t i = 0; i < (size_t)self->width; i++) {
    col_repr_widths[i] = qtb_column_repr_longest_of_first_five(&self->columns[i]);
    if (col_repr_widths[i] == -1) {
      free(col_repr_widths);
      return NULL;
    }
  }

  return col_repr_widths;
}

static char *qtb_table_repr_init(QtbTable *self, int *col_repr_widths) {
  size_t row_size = 2;
  char *repr;

  for (size_t i = 0; i < (size_t)self->width; i++)
    row_size += col_repr_widths[i] + 3;

  repr = (char *)malloc(row_size * (1 + MIN(self->size, 5)) * sizeof(char));
  if (repr == NULL) {
    PyErr_SetString(PyExc_MemoryError, "could not allocate memory");
    return NULL;
  }

  repr[0] = '\0';

  return repr;
}

static char *qtb_table_repr_formatted_cell(const char *repr) {
  char *formatted;
  size_t size;

  size = 3 + strlen(repr);

  formatted = (char *)malloc(1 + size * sizeof(char));
  if (formatted == NULL) {
    PyErr_SetString(PyExc_MemoryError, "could not allocate memory");
    return NULL;
  }

  if (sprintf(formatted, "| %s ", repr) != (int)size) {
    PyErr_SetString(PyExc_RuntimeError, "could not write format string");
    free(formatted);
    return NULL;
  }

  return formatted;
}

static bool qtb_table_repr_cat_header(QtbTable *self, char *repr) {
  char *cell_repr;
  char *cell_repr_formatted;

  for (size_t i = 0; i < (size_t)self->width; i++) {
    cell_repr = qtb_column_header_repr(&self->columns[i]);
    if (cell_repr == NULL)
      return false;

    cell_repr_formatted = qtb_table_repr_formatted_cell(cell_repr);
    free(cell_repr);
    if (cell_repr_formatted == NULL)
      return false;

    strcat(repr, cell_repr_formatted);
    free(cell_repr_formatted);
  }

  strcat(repr, "|");
  return true;
}

static PyObject *qtb_table_tp_repr(QtbTable *self) {
  int *col_repr_widths;
  char *repr;
  PyObject *repr_py;

  if (self->width == 0)
    return PyUnicode_FromString("");

  col_repr_widths = qtb_table_repr_get_column_widths(self);
  if (col_repr_widths == NULL)
    return NULL;

  repr = qtb_table_repr_init(self, col_repr_widths);
  free(col_repr_widths);
  if (repr == NULL)
    return NULL;

  if (!qtb_table_repr_cat_header(self, repr)) {
    free(repr);
    return NULL;
  }

  repr_py = PyUnicode_FromString(repr);
  free(repr);
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
