#ifndef QTB_COLUMN_AS_STRING_H
#define QTB_COLUMN_AS_STRING_H

#include "column.h"
#include "result.h"

ResultCharPtr qtb_column_str_cell_as_string(QtbColumn *column, size_t i);
ResultCharPtr qtb_column_int_cell_as_string(QtbColumn *column, size_t i);
ResultCharPtr qtb_column_float_cell_as_string(QtbColumn *column, size_t i);
ResultCharPtr qtb_column_bool_cell_as_string(QtbColumn *column, size_t i);

const char *qtb_column_str_type_as_string(void);
const char *qtb_column_int_type_as_string(void);
const char *qtb_column_float_type_as_string(void);
const char *qtb_column_bool_type_as_string(void);

ResultCharPtr qtb_column_header_as_string_(QtbColumn *column);

ResultSize_t qtb_column_repr_longest_of_first_five_(QtbColumn *column);

#endif
