#include <stdio.h>
#include "print.h"
#include "column.h"

char *_qtb_print_column_header(QtbColumn *column, mallocer m) {
  int size;
  char *header;

  size = 4 + strlen(column->name) + strlen(qtb_column_type_as_str(column));
  header = (char *)(*m)(sizeof(char) * (size_t)size);
  if (header == NULL) {
    PyErr_SetString(PyExc_MemoryError, "failed allocate memory for header repr");
    return NULL;
  }

  sprintf(header, "%s (%s)", column->name, qtb_column_type_as_str(column));
  return header;
}
