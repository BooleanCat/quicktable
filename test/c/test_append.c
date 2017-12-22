#include <Python.h>
#include "column.h"
#include "helpers.h"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "result.h"

static int setup(void **state) {
  PyGILState_STATE *gstate;

  gstate = (PyGILState_STATE *)malloc(sizeof(PyGILState_STATE));
  *gstate = PyGILState_Ensure();

  *state = (void *)gstate;
  return 0;
}

static int teardown(void **state) {
  PyGILState_STATE *gstate;

  gstate = (PyGILState_STATE *)(*state);
  PyErr_Clear();
  PyGILState_Release(*gstate);
  free(*state);

  return 0;
}

static void test_qtb_column_append(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *name;
  QtbResult result;

  descriptor = new_descriptor("Name", "str");
  name = PyUnicode_FromString_succeeds("Pikachu");
  column = qtb_column_new_succeeds();
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);

  result = qtb_column_append(column, name);
  Py_DECREF(name);

  assert_true(QtbResultSuccessful(result));
  assert_int_equal(column->size, 1);
  assert_string_equal("Pikachu", column->data[0].s);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_append_str_strdup_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *name;
  QtbResult result;

  column = qtb_column_new_succeeds();
  descriptor = new_descriptor("Name", "str");
  name = PyUnicode_FromString_succeeds("Pikachu");

  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);
  column->strdup = &failing_strdup;

  result = qtb_column_append(column, name);
  Py_DECREF(name);

  assert_true(QtbResultFailed(result));
  assert_string_equal("could not create PyUnicodeobject", QtbResultFailureMessage(result));
  assert_int_equal(column->size, 0);

  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_append_str_PyUnicode_AsUTF8_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *name;
  QtbResult result;

  column = qtb_column_new_succeeds();
  descriptor = new_descriptor("Name", "str");
  name = PyUnicode_FromString_succeeds("Pikachu");
  qtb_column_init_succeeds(column, descriptor);
  Py_DECREF(descriptor);
  column->PyUnicode_AsUTF8 = &failing_PyUnicode_AsUTF8;

  result = qtb_column_append(column, name);
  assert_true(QtbResultFailed(result));
  QtbResultFailureRaise(result);
  assert_exc_string_equal("PyUnicode_AsUTF8 failed");
  assert_int_equal(column->size, 0);

  Py_DECREF(name);
  qtb_column_dealloc(column);
  free(column);
}

static void test_qtb_column_append_grow_fails(void **state) {
  QtbColumn *column;
  PyObject *descriptor;
  PyObject *name;
  QtbResult result;

  column = qtb_column_new_succeeds();
  descriptor = new_descriptor("Name", "str");
  qtb_column_init_succeeds(column, descriptor);
  column->realloc = &failing_realloc;

  name = PyUnicode_FromString_succeeds("Pikachu");

  for (size_t i = 0; i < QTB_COLUMN_INITIAL_CAPACITY; i++)
    qtb_column_append_succeeds(column, name);

  result = qtb_column_append(column, name);
  assert_true(QtbResultFailed(result));
  assert_int_equal(column->size, QTB_COLUMN_INITIAL_CAPACITY);
  assert_string_equal("failed to grow column", QtbResultFailureMessage(result));

  Py_DECREF(name);
  qtb_column_dealloc(column);
  free(column);
}

static const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test_qtb_column_append, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_append_str_strdup_fails, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_append_str_PyUnicode_AsUTF8_fails, setup, teardown),
    cmocka_unit_test_setup_teardown(test_qtb_column_append_grow_fails, setup, teardown),
};

int test_append_run() {
  return cmocka_run_group_tests(tests, NULL, NULL);
}
