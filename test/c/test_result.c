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

static void test_result_success(void **state) {
  Result result;

  result = ResultSuccess();

  assert_true(ResultSuccessful(result));
  assert_false(ResultFailed(result));
}

static void test_result_failure(void **state) {
  Result result;

  result = ResultFailure(PyExc_RuntimeError, "I failed");

  assert_true(ResultFailed(result));
  assert_false(ResultSuccessful(result));
}

static void test_result_failure_from_py_err(void **state) {
  Result result;

  PyErr_SetString(PyExc_RuntimeError, "Something is very wrong.");
  result = ResultFailureFromPyErr();

  assert_null(PyErr_Occurred());
  assert_true(ResultFailed(result));
  assert_int_equal(RESULT_ERROR_STORED, result.value.error.type);

  PyErr_Restore(
    result.value.error.value.stored.exc_value,
    result.value.error.value.stored.exc_type,
    result.value.error.value.stored.exc_traceback
  );
  PyErr_Clear();
}

static void test_result_failure_from_result(void **state) {
  Result result;
  ResultInt result_int;

  result_int = ResultIntFailure(PyExc_MemoryError, "No memory!");
  result = ResultFailureFromResult(result_int);

  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "No memory!");
}

static void test_result_failure_raise(void **state) {
  Result result;

  PyErr_SetString(PyExc_MemoryError, "No memory!");
  result = ResultFailureFromPyErr();
  ResultFailureRaise(result);

  assert_non_null(PyErr_Occurred());
}

static void test_result_size_t_success(void **state) {
  ResultSize_t result;

  result = ResultSize_tSuccess(14);

  assert_true(ResultSuccessful(result));
  assert_int_equal(14, ResultValue(result));
}

static void test_result_size_t_failure(void **state) {
  ResultSize_t result;

  result = ResultSize_tFailure(PyExc_RuntimeError, "I failed");

  assert_true(ResultFailed(result));
  assert_string_equal("I failed", result.value.error.value.new.message);
}

static void test_result_size_t_failure_from_py_err(void **state) {
  ResultSize_t result;

  PyErr_SetString(PyExc_RuntimeError, "Something is very wrong.");
  result = ResultSize_tFailureFromPyErr();

  assert_null(PyErr_Occurred());
  assert_true(ResultFailed(result));
  assert_int_equal(RESULT_ERROR_STORED, result.value.error.type);

  PyErr_Restore(
    result.value.error.value.stored.exc_value,
    result.value.error.value.stored.exc_type,
    result.value.error.value.stored.exc_traceback
  );
  PyErr_Clear();
}

static void test_result_size_t_failure_from_result(void **state) {
  ResultSize_t result;
  ResultCharPtr result_char_ptr;

  result_char_ptr = ResultCharPtrFailure(PyExc_RuntimeError, "Oh, no.");
  result = ResultSize_tFailureFromResult(result_char_ptr);

  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "Oh, no.");
}

static void test_result_size_t_failure_raise(void **state) {
  ResultSize_t result;

  PyErr_SetString(PyExc_MemoryError, "No memory!");
  result = ResultSize_tFailureFromPyErr();
  ResultFailureRaise(result);

  assert_non_null(PyErr_Occurred());
}

static void test_result_size_t_ptr_success(void **state) {
  size_t number;
  ResultSize_tPtr result;

  number = 14;
  result = ResultSize_tPtrSuccess(&number);

  assert_true(ResultSuccessful(result));
  assert_int_equal(14, *ResultValue(result));
}

static void test_result_size_t_ptr_failure(void **state) {
  ResultSize_tPtr result;

  result = ResultSize_tPtrFailure(PyExc_RuntimeError, "I failed");

  assert_true(ResultFailed(result));
  assert_string_equal("I failed", result.value.error.value.new.message);
}

static void test_result_size_t_ptr_failure_from_py_err(void **state) {
  ResultSize_tPtr result;

  PyErr_SetString(PyExc_RuntimeError, "Something is very wrong.");
  result = ResultSize_tPtrFailureFromPyErr();

  assert_null(PyErr_Occurred());
  assert_true(ResultFailed(result));
  assert_int_equal(RESULT_ERROR_STORED, result.value.error.type);

  PyErr_Restore(
    result.value.error.value.stored.exc_value,
    result.value.error.value.stored.exc_type,
    result.value.error.value.stored.exc_traceback
  );
  PyErr_Clear();
}

static void test_result_size_t_ptr_failure_from_result(void **state) {
  ResultSize_tPtr result;
  ResultCharPtr result_char_ptr;

  result_char_ptr = ResultCharPtrFailure(PyExc_RuntimeError, "Oh, no.");
  result = ResultSize_tPtrFailureFromResult(result_char_ptr);

  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "Oh, no.");
}

static void test_result_size_t_ptr_failure_raise(void **state) {
  ResultSize_tPtr result;

  PyErr_SetString(PyExc_MemoryError, "No memory!");
  result = ResultSize_tPtrFailureFromPyErr();
  ResultFailureRaise(result);

  assert_non_null(PyErr_Occurred());
}

static void test_result_int_success(void **state) {
  ResultInt result;

  result = ResultIntSuccess(14);

  assert_true(ResultSuccessful(result));
  assert_int_equal(14, ResultValue(result));
}

static void test_result_int_failure(void **state) {
  ResultInt result;

  result = ResultIntFailure(PyExc_RuntimeError, "I failed");

  assert_true(ResultFailed(result));
  assert_string_equal("I failed", result.value.error.value.new.message);
}

static void test_result_int_failure_from_py_err(void **state) {
  ResultInt result;

  PyErr_SetString(PyExc_MemoryError, "Huh...");
  result = ResultIntFailureFromPyErr();

  assert_null(PyErr_Occurred());
  assert_true(ResultFailed(result));
  assert_int_equal(RESULT_ERROR_STORED, result.value.error.type);

  PyErr_Restore(
    result.value.error.value.stored.exc_value,
    result.value.error.value.stored.exc_type,
    result.value.error.value.stored.exc_traceback
  );
  PyErr_Clear();
}

static void test_result_int_failure_from_result(void **state) {
  ResultInt result;
  ResultCharPtr result_char_ptr;

  result_char_ptr = ResultCharPtrFailure(PyExc_RuntimeError, "Oh, no.");
  result = ResultIntFailureFromResult(result_char_ptr);

  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "Oh, no.");
}

static void test_result_int_failure_raise(void **state) {
  ResultInt result;

  PyErr_SetString(PyExc_MemoryError, "No memory!");
  result = ResultIntFailureFromPyErr();
  ResultFailureRaise(result);

  assert_non_null(PyErr_Occurred());
}

static void test_result_char_ptr_success(void **state) {
  ResultCharPtr result;

  result = ResultCharPtrSuccess("Yay!");

  assert_true(ResultSuccessful(result));
  assert_string_equal("Yay!", ResultValue(result));
}

static void test_result_char_ptr_failure(void **state) {
  ResultCharPtr result;

  result = ResultCharPtrFailure(PyExc_MemoryError, "Oh, no.");

  assert_true(ResultFailed(result));
  assert_string_equal("Oh, no.", result.value.error.value.new.message);
}

static void test_result_char_ptr_failure_from_py_err(void **state) {
  ResultCharPtr result;

  PyErr_SetString(PyExc_RuntimeError, "Something is very wrong.");
  result = ResultCharPtrFailureFromPyErr();

  assert_null(PyErr_Occurred());
  assert_true(ResultFailed(result));
  assert_int_equal(RESULT_ERROR_STORED, result.value.error.type);

  PyErr_Restore(
    result.value.error.value.stored.exc_value,
    result.value.error.value.stored.exc_type,
    result.value.error.value.stored.exc_traceback
  );
  PyErr_Clear();
}

static void test_result_char_ptr_failure_from_result(void **state) {
  ResultCharPtr result;
  ResultInt result_int;

  result_int = ResultIntFailure(PyExc_RuntimeError, "Oh, no.");
  result = ResultCharPtrFailureFromResult(result_int);

  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "Oh, no.");
}

static void test_result_char_ptr_failure_raise(void **state) {
  ResultCharPtr result;

  PyErr_SetString(PyExc_MemoryError, "No memory!");
  result = ResultCharPtrFailureFromPyErr();
  ResultFailureRaise(result);

  assert_non_null(PyErr_Occurred());
}

static void test_result_pyobject_ptr_success(void **state) {
  ResultPyObjectPtr result;
  PyObject *four;

  four = PyLong_FromLongLong_SUCCESS(4);
  result = ResultPyObjectPtrSuccess(four);

  assert_true(ResultSuccessful(result));
  assert_int_equal(4, PyLong_AsLong(ResultValue(result)));

  Py_DECREF(four);
}

static void test_result_pyobject_ptr_failure(void **state) {
  ResultPyObjectPtr result;

  result = ResultPyObjectPtrFailure(PyExc_MemoryError, "Oh, no.");

  assert_true(ResultFailed(result));
  assert_string_equal("Oh, no.", result.value.error.value.new.message);
}

static void test_result_pyobject_ptr_failure_from_py_err(void **state) {
  ResultPyObjectPtr result;

  PyErr_SetString(PyExc_RuntimeError, "Something is very wrong.");
  result = ResultPyObjectPtrFailureFromPyErr();

  assert_null(PyErr_Occurred());
  assert_true(ResultFailed(result));
  assert_int_equal(RESULT_ERROR_STORED, result.value.error.type);

  PyErr_Restore(
    result.value.error.value.stored.exc_value,
    result.value.error.value.stored.exc_type,
    result.value.error.value.stored.exc_traceback
  );
  PyErr_Clear();
}

static void test_result_pyobject_ptr_failure_from_result(void **state) {
  ResultPyObjectPtr result;
  ResultCharPtr result_char_ptr;

  result_char_ptr = ResultCharPtrFailure(PyExc_RuntimeError, "Oh, no.");
  result = ResultPyObjectPtrFailureFromResult(result_char_ptr);

  assert_true(ResultFailed(result));
  assert_string_equal(result.value.error.value.new.message, "Oh, no.");
}

static void test_result_pyobject_ptr_failure_raise(void **state) {
  ResultPyObjectPtr result;

  PyErr_SetString(PyExc_MemoryError, "No memory!");
  result = ResultPyObjectPtrFailureFromPyErr();
  ResultFailureRaise(result);

  assert_non_null(PyErr_Occurred());
}

static void test_result_failure_raise_from_new(void **state) {
  Result result;

  result = ResultFailure(PyExc_TypeError, "Wrong type, friend.");
  ResultFailureRaise(result);

  assert_non_null(PyErr_Occurred());
  assert_exc_string_equal("Wrong type, friend.");
  PyErr_Clear();
}

#define register_test(test) cmocka_unit_test_setup_teardown(test, setup, teardown)

static const struct CMUnitTest tests[] = {
    register_test(test_result_success),
    register_test(test_result_failure),
    register_test(test_result_failure_from_py_err),
    register_test(test_result_failure_from_result),
    register_test(test_result_failure_raise),

    register_test(test_result_size_t_success),
    register_test(test_result_size_t_failure),
    register_test(test_result_size_t_failure_from_py_err),
    register_test(test_result_size_t_failure_from_result),
    register_test(test_result_size_t_failure_raise),

    register_test(test_result_size_t_ptr_success),
    register_test(test_result_size_t_ptr_failure),
    register_test(test_result_size_t_ptr_failure_from_py_err),
    register_test(test_result_size_t_ptr_failure_from_result),
    register_test(test_result_size_t_ptr_failure_raise),

    register_test(test_result_int_success),
    register_test(test_result_int_failure),
    register_test(test_result_int_failure_from_py_err),
    register_test(test_result_int_failure_from_result),
    register_test(test_result_int_failure_raise),

    register_test(test_result_char_ptr_success),
    register_test(test_result_char_ptr_failure),
    register_test(test_result_char_ptr_failure_from_py_err),
    register_test(test_result_char_ptr_failure_from_result),
    register_test(test_result_char_ptr_failure_raise),

    register_test(test_result_pyobject_ptr_success),
    register_test(test_result_pyobject_ptr_failure),
    register_test(test_result_pyobject_ptr_failure_from_py_err),
    register_test(test_result_pyobject_ptr_failure_from_result),
    register_test(test_result_pyobject_ptr_failure_raise),

    register_test(test_result_failure_raise_from_new),
};

int test_result_run() {
  return cmocka_run_group_tests(tests, NULL, NULL);
}
