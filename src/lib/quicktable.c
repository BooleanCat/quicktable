#include <Python.h>

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
  return PyModule_Create(&quicktable_module);
}
