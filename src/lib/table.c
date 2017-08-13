#include "table.h"

static PyObject *
qtab_Table_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
  qtab_Table *self;

  self = (qtab_Table *)type->tp_alloc(type, 0);
  if (self != NULL) {
    self->size = 0;
    self->width = 0;
    self->columns = NULL;
  }

  return (PyObject *)self;
}

static void qtab_Table_dealloc(qtab_Table *self) {
  for (Py_ssize_t i = 0; i < self->width; i++)
    qtb_column_dealloc(&self->columns[i]);

  free(self->columns);
  Py_TYPE(self)->tp_free((PyObject *)self);
}

static int qtab_Table_init(qtab_Table *self, PyObject *args, PyObject *kwargs) {
  PyObject *blueprint = NULL;

  if (!PyArg_ParseTuple(args, "O|", &blueprint))
    return -1;

  if (qtab_validate_blueprint(blueprint) == false)
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

static Py_ssize_t qtab_Table_length(qtab_Table *self) {
  return self->size;
}

static PySequenceMethods qtab_Table_as_sequence = {
  (lenfunc)qtab_Table_length,  // sq_length
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

static PyObject *qtab_Table_append(qtab_Table *self) {
  self->size++;
  Py_RETURN_NONE;
}

static PyObject *qtab_Table_pop(qtab_Table *self) {
  if (self->size == 0) {
    PyErr_SetString(PyExc_IndexError, "pop from empty table");
    return NULL;
  }

  self->size--;
  Py_RETURN_NONE;
}

static PyMethodDef qtab_Table_methods[] = {
  {"append", (PyCFunction)qtab_Table_append, METH_NOARGS, "append"},
  {"pop", (PyCFunction)qtab_Table_pop, METH_NOARGS, "pop"},
  {NULL, NULL}
};

static PyObject *qtab_Table_blueprint(qtab_Table *self, void *closure) {
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

static PyGetSetDef qtab_table_getsetters[] = {
  {"blueprint", (getter)qtab_Table_blueprint, NULL, "copy of table's blueprint", NULL},
  {NULL}
};

PyTypeObject qtab_TableType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "quicktable.Table",  // tp_name
    sizeof(qtab_Table),  // tp_basicsize
    0,  // tp_itemsize
    (destructor)qtab_Table_dealloc,  // tp_dealloc
    0,  // tp_print
    0,  // tp_getattr
    0,  // tp_setattr
    0,  // tp_reserved
    0,  // tp_repr
    0,  // tp_as_number
    &qtab_Table_as_sequence,  // tp_as_sequence
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
    qtab_Table_methods,  // tp_methods
    0,  // tp_members
    qtab_table_getsetters,  // tp_getset
    0,  // tp_base
    0,  // tp_dict
    0,  // tp_descr_get
    0,  // tp_descr_set
    0,  // tp_dictoffset
    (initproc)qtab_Table_init,  // tp_init
    0,  // tp_alloc
    qtab_Table_new  // tp_new
};
