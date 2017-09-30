#ifndef TEST_APPEND_H
#define TEST_APPEND_H

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <Python.h>
#include "column.h"
#include "helpers.h"

int test_append_run(void);
int test_column_run(void);

#endif
