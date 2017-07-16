#include "table.h"

static PyObject *
qtab_Table_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
  return type->tp_alloc(type, 0);
}

static void qtab_Table_dealloc(qtab_Table *self) {
  Py_TYPE(self)->tp_free((PyObject *)self);
}

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
    0,  // tp_as_sequence
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
    0,  // tp_methods
    0,  // tp_members
    0,  // tp_getset
    0,  // tp_base
    0,  // tp_dict
    0,  // tp_descr_get
    0,  // tp_descr_set
    0,  // tp_dictoffset
    0,  // tp_init
    0,  // tp_alloc
    qtab_Table_new  // tp_new
};
