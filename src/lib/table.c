#include "table.h"

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

  if (!PyArg_ParseTuple(args, "O|", &blueprint))
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

static PySequenceMethods qtb_table_as_sequence = {
  (lenfunc)qtb_table_length,  // sq_length
  0,  // sq_concat
  0,  // sq_repeat
  0,  // sq_item
  0,  // sq_slice
  0,  // sq_ass_item
  0,  // sq_ass_slice
  0,  // sq_contains
  0,  // sq_inplace_concat
  0,  // sq_inplace_repeat
};

static PyObject *qtb_table_append(QtbTable *self, PyObject *row) {
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
    0,  // tp_repr
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
