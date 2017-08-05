#include "column.h"

bool qtab_Column_init(qtab_Column *column, PyObject *descriptor) {
  PyObject *name;
  PyObject *type;
  char *name_s;
  char *type_s;

  name = PySequence_ITEM(descriptor, 0);
  if (name == NULL)
    return false;

  name_s = PyUnicode_AsUTF8AndSize(name, NULL);
  if (name == NULL)
    return false;

  column->name = strdup(name_s);
  Py_DECREF(name);

  if (column->name == NULL)
    return false;

  type = PySequence_ITEM(descriptor, 1);
  if (type == NULL)
    return false;

  type_s = PyUnicode_AsUTF8AndSize(type, NULL);
  if (type_s == NULL)
    return false;

  column->type = strdup(type_s);
  Py_DECREF(type);

  if (column->type == NULL)
    return false;

  return true;
}

void qtab_Column_dealloc(qtab_Column *column) {
  free(column->name);
  free(column->type);
}
