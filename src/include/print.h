#ifndef QTB_PRINT_H
#define QTB_PRINT_H

#include "column.h"

typedef void *(*mallocer)(size_t);

char *_qtb_print_column_header(QtbColumn *column, mallocer m);
#define qtb_print_column_header(column) _qtb_print_column_header(column, &malloc);

#endif
