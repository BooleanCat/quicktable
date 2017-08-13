#include <Python.h>
#include "table.h"

extern PyTypeObject QtbTableType;

static PyModuleDef quicktable_module = {
  PyModuleDef_HEAD_INIT,
  "quicktable",
  "quicktable",
  -1,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC PyInit_quicktable() {
  PyObject *module;

  if (PyType_Ready(&QtbTableType) < 0)
    return NULL;

  module = PyModule_Create(&quicktable_module);
  if (module == NULL)
    return NULL;

  Py_INCREF(&QtbTableType);
  if (PyModule_AddObject(module, "Table", (PyObject *)&QtbTableType) == -1)
    return NULL;

  return module;
}
