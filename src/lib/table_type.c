#include <Python.h>
#include "table.h"
#include "table_repr.h"

static PyObject *qtb_table_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
  QtbTable *self;

  self = (QtbTable *)type->tp_alloc(type, 0);
  if (self != NULL) qtb_table_zero(self);

  return (PyObject *)self;
}

static void qtb_table_dealloc(QtbTable *self) {
  qtb_table_dealloc_(self);
  Py_TYPE(self)->tp_free((PyObject *)self);
}

static int qtb_table_init(QtbTable *self, PyObject *args, PyObject *kwargs) {
  PyObject *blueprint = NULL;
  Result result;

  if (!PyArg_ParseTuple(args, "O", &blueprint))
    return -1;

  result = qtb_table_init_(self, blueprint);
  if (ResultFailed(result)) {
    ResultFailureRaise(result);
    return -1;
  }

  return 0;
}

static PyObject *qtb_table_item(QtbTable *self, Py_ssize_t i) {
  ResultPyObjectPtr result;

  result = qtb_table_item_(self, i);
  if (ResultFailed(result)) {
    ResultFailureRaise(result);
    return NULL;
  }

  return ResultValue(result);
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
  Result result;

  result = qtb_table_append_(self, row);
  if (ResultFailed(result)) {
    ResultFailureRaise(result);
    return NULL;
  }

  Py_RETURN_NONE;
}

static PyObject *qtb_table_pop(QtbTable *self) {
  ResultPyObjectPtr result;

  result = qtb_table_pop_(self);
  if (ResultFailed(result)) {
    ResultFailureRaise(result);
    return NULL;
  }

  return ResultValue(result);
}

static PyMethodDef qtb_table_methods[] = {
  {"append", (PyCFunction)qtb_table_append, METH_O, "append"},
  {"pop", (PyCFunction)qtb_table_pop, METH_NOARGS, "pop"},
  {NULL, NULL}
};

static PyObject *qtb_table_blueprint(QtbTable *self, void *closure) {
  ResultPyObjectPtr result;

  result = qtb_table_blueprint_(self);
  if (ResultFailed(result)) {
    ResultFailureRaise(result);
    return NULL;
  }

  return ResultValue(result);
}

static PyGetSetDef qtb_table_getsetters[] = {
  {"blueprint", (getter)qtb_table_blueprint, NULL, "copy of table's blueprint", NULL},
  {NULL}
};

static PyObject *qtb_table_as_py_string(QtbTable *self) {
  ResultPyObjectPtr result;

  result = qtb_table_as_py_string_(self);
  if (ResultFailed(result)) {
    ResultFailureRaise(result);
    return NULL;
  }

  return ResultValue(result);
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
    (reprfunc)qtb_table_as_py_string,  // tp_repr
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
