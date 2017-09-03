#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

void test_nothing(void **state) {

}

const struct CMUnitTest append_tests[] = {
  cmocka_unit_test(test_nothing),
};
