#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <Python.h>
#include "column.h"

typedef struct {
  PyGILState_STATE gstate;
  PyObject *descriptor;
} TestState;

static int setup(void **state) {
  TestState *test_state = (TestState *)malloc(sizeof(TestState));

  test_state->gstate = PyGILState_Ensure();
  test_state->descriptor = PyTuple_New(2);
  assert_non_null(test_state);
  PyTuple_SET_ITEM(test_state->descriptor, 0, PyUnicode_FromString("Name"));
  PyTuple_SET_ITEM(test_state->descriptor, 1, PyUnicode_FromString("str"));

  *state = (void *)test_state;
  return 0;
}

static int teardown(void **state) {
  TestState *test_state = (TestState *)(*state);

  Py_DECREF(test_state->descriptor);
  PyGILState_Release(test_state->gstate);
  free(test_state);

  return 0;
}

static void test_qtab_Column_init_returns_true(void **state) {
  qtab_Column column;
  bool success;

  success = qtab_Column_init(&column, ((TestState *)(*state))->descriptor);
  assert_int_equal(success, true);

  qtab_Column_dealloc(&column);
}

static void test_qtab_Column_init_sets_column_name(void **state) {
  qtab_Column column;

  qtab_Column_init(&column, ((TestState *)(*state))->descriptor);
  assert_string_equal("Name", column.name);

  qtab_Column_dealloc(&column);
}

static void test_qtab_Column_init_sets_column_type(void **state) {
  qtab_Column column;

  qtab_Column_init(&column, ((TestState *)(*state))->descriptor);
  assert_string_equal("str", column.type);

  qtab_Column_dealloc(&column);
}

static void test_qtab_Column_init_does_not_change_descriptor_refcount(void **state) {
  qtab_Column column;
  PyObject *descriptor;
  Py_ssize_t descriptor_ref_count;

  descriptor = ((TestState *)(*state))->descriptor;
  descriptor_ref_count = Py_REFCNT(descriptor);

  qtab_Column_init(&column, descriptor);
  qtab_Column_dealloc(&column);

  assert_int_equal(descriptor_ref_count, Py_REFCNT(descriptor));
}

static void test_qtab_Column_init_does_not_change_name_refcount(void **state) {
  qtab_Column column;
  PyObject *descriptor;
  Py_ssize_t name_ref_count;

  descriptor = ((TestState *)(*state))->descriptor;
  name_ref_count = Py_REFCNT(PyTuple_GET_ITEM(descriptor, 0));

  qtab_Column_init(&column, descriptor);
  qtab_Column_dealloc(&column);

  assert_int_equal(name_ref_count, Py_REFCNT(PyTuple_GET_ITEM(descriptor, 0)));
}

static void test_qtab_Column_init_does_not_change_type_refcount(void **state) {
  qtab_Column column;
  PyObject *descriptor;
  Py_ssize_t type_ref_count;

  descriptor = ((TestState *)(*state))->descriptor;
  type_ref_count = Py_REFCNT(PyTuple_GET_ITEM(descriptor, 1));

  qtab_Column_init(&column, descriptor);
  qtab_Column_dealloc(&column);

  assert_int_equal(type_ref_count, Py_REFCNT(PyTuple_GET_ITEM(descriptor, 1)));
}

int main(void) {
  Py_Initialize();

  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_qtab_Column_init_returns_true, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtab_Column_init_sets_column_name, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtab_Column_init_sets_column_type, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtab_Column_init_does_not_change_descriptor_refcount, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtab_Column_init_does_not_change_name_refcount, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtab_Column_init_does_not_change_type_refcount, setup, teardown),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
