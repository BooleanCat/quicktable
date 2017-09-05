#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <Python.h>

#define N_COLUMN_TESTS 8
#define N_APPEND_TESTS 1

extern const struct CMUnitTest column_tests[N_COLUMN_TESTS];
extern const struct CMUnitTest append_tests[N_APPEND_TESTS];

int main(void) {
  Py_Initialize();

  return (
    cmocka_run_group_tests(column_tests, NULL, NULL)
    || cmocka_run_group_tests(append_tests, NULL, NULL)
  );
}
