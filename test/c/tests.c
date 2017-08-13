#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <Python.h>

#define N_COLUMN_TESTS 12

extern const struct CMUnitTest column_tests[N_COLUMN_TESTS];

int main(void) {
  Py_Initialize();

  return cmocka_run_group_tests(column_tests, NULL, NULL);
}
