#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <Python.h>

#define N_COLUMN_TESTS 12
#define N_BLUEPRINT_TESTS 1

extern const struct CMUnitTest column_tests[N_COLUMN_TESTS];
extern const struct CMUnitTest blueprint_tests[N_BLUEPRINT_TESTS];

int main(void) {
  Py_Initialize();

  return (
    cmocka_run_group_tests(column_tests, NULL, NULL)
    || cmocka_run_group_tests(blueprint_tests, NULL, NULL)
  );
}
