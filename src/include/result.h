#ifndef RESULT_H
#define RESULT_H

#include <Python.h>

typedef enum {
  RESULT_SUCCESS,
  RESULT_FAILURE,
} ResultType;

#define Result_HEAD \
  ResultType type;

#define ResultSuccessful(result) (result.type == RESULT_SUCCESS)
#define ResultFailed(result) (result.type == RESULT_FAILURE)
#define ResultValue(result) (result.value.value)

typedef enum {
  RESULT_ERROR_NEW,
  RESULT_ERROR_STORED,
} ResultErrorType;

typedef struct {
  PyObject *py_err_class;
  const char *message;
} ResultErrorNew;

typedef struct {
  PyObject *exc_value;
  PyObject *exc_type;
  PyObject *exc_traceback;
} ResultErrorStored;

typedef union {
  ResultErrorNew new;
  ResultErrorStored stored;
} ResultErrorValue;

typedef struct {
  ResultErrorType type;
  ResultErrorValue value;
} ResultError;

ResultError package_py_err(void);
void unpackage_py_err(ResultError);

#define ResultRegisterSuccess(type, value) ((type){RESULT_SUCCESS, {value}})
#define ResultRegisterFailure(type, py_err, message) ((type){ \
  RESULT_FAILURE, \
  .value.error={ \
    RESULT_ERROR_NEW, \
    .value.new={py_err, message} \
  } \
})
#define ResultRegisterFailureFromPyErr(type) ((type){RESULT_FAILURE, .value.error=package_py_err()})
#define ResultFailureRaise(result) unpackage_py_err(result.value.error)
#define ResultFailureMessage(result) (result.value.error.value.new.message)

// ===== empty result =====

typedef union {
  ResultError error;
} ResultValue;

typedef struct {
  Result_HEAD
  ResultValue value;
} Result;

#define ResultSuccess ((Result){RESULT_SUCCESS})
#define ResultFailure(py_err, message) ResultRegisterFailure(Result, py_err, message)
#define ResultFailureFromPyErr() ResultRegisterFailureFromPyErr(Result)

// ===== size_t result =====

typedef union {
  size_t value;
  ResultError error;
} ResultSize_tValue;

typedef struct {
  Result_HEAD
  ResultSize_tValue value;
} ResultSize_t;

#define ResultSize_tSuccess(value) ResultRegisterSuccess(ResultSize_t, value)
#define ResultSize_tFailure(py_err, message) ResultRegisterFailure(ResultSize_t, py_err, message)
#define ResultSize_tFailureFromPyErr() ResultRegisterFailureFromPyErr(ResultSize_t)

// ===== int result =====

typedef union {
  int value;
  ResultError error;
} ResultIntValue;

typedef struct {
  Result_HEAD
  ResultIntValue value;
} ResultInt;

#define ResultIntSuccess(value) ResultRegisterSuccess(ResultInt, value)
#define ResultIntFailure(py_err, message) ResultRegisterFailure(ResultInt, py_err, message)
#define ResultIntFailureFromPyErr() ResultRegisterFailureFromPyErr(ResultInt)

// ===== char *result =====

typedef union {
  char *value;
  ResultError error;
} ResultCharPtrValue;

typedef struct {
  Result_HEAD
  ResultCharPtrValue value;
} ResultCharPtr;

#define ResultCharPtrSuccess(value) ResultRegisterSuccess(ResultCharPtr, value)
#define ResultCharPtrFailure(py_err, message) ResultRegisterFailure(ResultCharPtr, py_err, message)
#define ResultCharPtrFailureFromPyErr() ResultRegisterFailureFromPyErr(ResultCharPtr)

// ===== PyObject *result =====

typedef union {
  PyObject *value;
  ResultError error;
} ResultPyObjectPtrValue;

typedef struct {
  Result_HEAD
  ResultPyObjectPtrValue value;
} ResultPyObjectPtr;

#define ResultPyObjectPtrSuccess(value) ResultRegisterSuccess(ResultPyObjectPtr, value)
#define ResultPyObjectPtrFailure(py_err, message) ResultRegisterFailure(ResultPyObjectPtr, py_err, message)
#define ResultPyObjectPtrFailureFromPyErr() ResultRegisterFailureFromPyErr(ResultPyObjectPtr)

#endif
