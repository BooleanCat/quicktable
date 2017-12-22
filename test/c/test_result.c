#include <Python.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include "result.h"
#include "helpers.h"

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

static void test_qtb_result_success(void **state) {
  QtbResult result;

  result = QtbResultSuccess;

  assert_true(QtbResultSuccessful(result));
  assert_false(QtbResultFailed(result));
}

static void test_qtb_result_failure(void **state) {
  QtbResult result;

  result = QtbResultFailure(PyExc_RuntimeError, "I failed");

  assert_true(QtbResultFailed(result));
  assert_false(QtbResultSuccessful(result));
}

static void test_qtb_result_failure_from_py_err(void **state) {
  QtbResult result;

  PyErr_SetString(PyExc_RuntimeError, "Something is very wrong.");
  result = QtbResultFailureFromPyErr();

  assert_null(PyErr_Occurred());
  assert_true(QtbResultFailed(result));
  assert_int_equal(QTB_RESULT_ERROR_STORED, result.value.error.type);

  PyErr_Restore(
    result.value.error.value.stored.exc_value,
    result.value.error.value.stored.exc_type,
    result.value.error.value.stored.exc_traceback
  );
  PyErr_Clear();
}

static void test_qtb_result_failure_raise(void **state) {
  QtbResult result;

  PyErr_SetString(PyExc_MemoryError, "No memory!");
  result = QtbResultFailureFromPyErr();
  QtbResultFailureRaise(result);

  assert_non_null(PyErr_Occurred());
}

static void test_qtb_result_size_t_success(void **state) {
  QtbResultSize_t result;

  result = QtbResultSize_tSuccess(14);

  assert_true(QtbResultSuccessful(result));
  assert_int_equal(14, QtbResultValue(result));
}

static void test_qtb_result_size_t_failure(void **state) {
  QtbResultSize_t result;

  result = QtbResultSize_tFailure(PyExc_RuntimeError, "I failed");

  assert_true(QtbResultFailed(result));
  assert_string_equal("I failed", result.value.error.value.new.message);
}

static void test_qtb_result_size_t_failure_from_py_err(void **state) {
  QtbResultSize_t result;

  PyErr_SetString(PyExc_RuntimeError, "Something is very wrong.");
  result = QtbResultSize_tFailureFromPyErr();

  assert_null(PyErr_Occurred());
  assert_true(QtbResultFailed(result));
  assert_int_equal(QTB_RESULT_ERROR_STORED, result.value.error.type);

  PyErr_Restore(
    result.value.error.value.stored.exc_value,
    result.value.error.value.stored.exc_type,
    result.value.error.value.stored.exc_traceback
  );
  PyErr_Clear();
}

static void test_qtb_result_size_t_failure_raise(void **state) {
  QtbResultSize_t result;

  PyErr_SetString(PyExc_MemoryError, "No memory!");
  result = QtbResultSize_tFailureFromPyErr();
  QtbResultFailureRaise(result);

  assert_non_null(PyErr_Occurred());
}

static void test_qtb_result_int_success(void **state) {
  QtbResultInt result;

  result = QtbResultIntSuccess(14);

  assert_true(QtbResultSuccessful(result));
  assert_int_equal(14, QtbResultValue(result));
}

static void test_qtb_result_int_failure(void **state) {
  QtbResultInt result;

  result = QtbResultIntFailure(PyExc_RuntimeError, "I failed");

  assert_true(QtbResultFailed(result));
  assert_string_equal("I failed", result.value.error.value.new.message);
}

static void test_qtb_result_int_failure_from_py_err(void **state) {
  QtbResultInt result;

  PyErr_SetString(PyExc_MemoryError, "Huh...");
  result = QtbResultIntFailureFromPyErr();

  assert_null(PyErr_Occurred());
  assert_true(QtbResultFailed(result));
  assert_int_equal(QTB_RESULT_ERROR_STORED, result.value.error.type);

  PyErr_Restore(
    result.value.error.value.stored.exc_value,
    result.value.error.value.stored.exc_type,
    result.value.error.value.stored.exc_traceback
  );
  PyErr_Clear();
}

static void test_qtb_result_int_failure_raise(void **state) {
  QtbResultInt result;

  PyErr_SetString(PyExc_MemoryError, "No memory!");
  result = QtbResultIntFailureFromPyErr();
  QtbResultFailureRaise(result);

  assert_non_null(PyErr_Occurred());
}

static void test_qtb_result_char_ptr_success(void **state) {
  QtbResultCharPtr result;

  result = QtbResultCharPtrSuccess("Yay!");

  assert_true(QtbResultSuccessful(result));
  assert_string_equal("Yay!", QtbResultValue(result));
}

static void test_qtb_result_char_ptr_failure(void **state) {
  QtbResultCharPtr result;

  result = QtbResultCharPtrFailure(PyExc_MemoryError, "Oh, no.");

  assert_true(QtbResultFailed(result));
  assert_string_equal("Oh, no.", result.value.error.value.new.message);
}

static void test_qtb_result_char_ptr_failure_from_py_err(void **state) {
  QtbResultCharPtr result;

  PyErr_SetString(PyExc_RuntimeError, "Something is very wrong.");
  result = QtbResultCharPtrFailureFromPyErr();

  assert_null(PyErr_Occurred());
  assert_true(QtbResultFailed(result));
  assert_int_equal(QTB_RESULT_ERROR_STORED, result.value.error.type);

  PyErr_Restore(
    result.value.error.value.stored.exc_value,
    result.value.error.value.stored.exc_type,
    result.value.error.value.stored.exc_traceback
  );
  PyErr_Clear();
}

static void test_qtb_result_char_ptr_failure_raise(void **state) {
  QtbResultCharPtr result;

  PyErr_SetString(PyExc_MemoryError, "No memory!");
  result = QtbResultCharPtrFailureFromPyErr();
  QtbResultFailureRaise(result);

  assert_non_null(PyErr_Occurred());
}

static void test_qtb_result_pyobject_ptr_success(void **state) {
  QtbResultPyObjectPtr result;
  PyObject *four;

  four = PyLong_FromLongLong_succeeds(4);
  result = QtbResultPyObjectPtrSuccess(four);

  assert_true(QtbResultSuccessful(result));
  assert_int_equal(4, PyLong_AsLong(QtbResultValue(result)));

  Py_DECREF(four);
}

static void test_qtb_result_pyobject_ptr_failure(void **state) {
  QtbResultPyObjectPtr result;

  result = QtbResultPyObjectPtrFailure(PyExc_MemoryError, "Oh, no.");

  assert_true(QtbResultFailed(result));
  assert_string_equal("Oh, no.", result.value.error.value.new.message);
}

static void test_qtb_result_pyobject_ptr_failure_from_py_err(void **state) {
  QtbResultPyObjectPtr result;

  PyErr_SetString(PyExc_RuntimeError, "Something is very wrong.");
  result = QtbResultPyObjectPtrFailureFromPyErr();

  assert_null(PyErr_Occurred());
  assert_true(QtbResultFailed(result));
  assert_int_equal(QTB_RESULT_ERROR_STORED, result.value.error.type);

  PyErr_Restore(
    result.value.error.value.stored.exc_value,
    result.value.error.value.stored.exc_type,
    result.value.error.value.stored.exc_traceback
  );
  PyErr_Clear();
}

static void test_qtb_result_pyobject_ptr_failure_raise(void **state) {
  QtbResultPyObjectPtr result;

  PyErr_SetString(PyExc_MemoryError, "No memory!");
  result = QtbResultPyObjectPtrFailureFromPyErr();
  QtbResultFailureRaise(result);

  assert_non_null(PyErr_Occurred());
}

static void test_qtb_result_failure_raise_from_new(void **state) {
  QtbResult result;

  result = QtbResultFailure(PyExc_TypeError, "Wrong type, friend.");
  QtbResultFailureRaise(result);

  assert_non_null(PyErr_Occurred());
  assert_exc_string_equal("Wrong type, friend.");
  PyErr_Clear();
}

#define register_test(test) cmocka_unit_test_setup_teardown(test, setup, teardown)

static const struct CMUnitTest tests[] = {
    register_test(test_qtb_result_success),
    register_test(test_qtb_result_failure),
    register_test(test_qtb_result_failure_from_py_err),
    register_test(test_qtb_result_failure_raise),

    register_test(test_qtb_result_size_t_success),
    register_test(test_qtb_result_size_t_failure),
    register_test(test_qtb_result_size_t_failure_from_py_err),
    register_test(test_qtb_result_size_t_failure_raise),

    register_test(test_qtb_result_int_success),
    register_test(test_qtb_result_int_failure),
    register_test(test_qtb_result_int_failure_from_py_err),
    register_test(test_qtb_result_int_failure_raise),

    register_test(test_qtb_result_char_ptr_success),
    register_test(test_qtb_result_char_ptr_failure),
    register_test(test_qtb_result_char_ptr_failure_from_py_err),
    register_test(test_qtb_result_char_ptr_failure_raise),

    register_test(test_qtb_result_pyobject_ptr_success),
    register_test(test_qtb_result_pyobject_ptr_failure),
    register_test(test_qtb_result_pyobject_ptr_failure_from_py_err),
    register_test(test_qtb_result_pyobject_ptr_failure_raise),

    register_test(test_qtb_result_failure_raise_from_new),
};

int test_result_run() {
  return cmocka_run_group_tests(tests, NULL, NULL);
}
