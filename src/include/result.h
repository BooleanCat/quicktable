#ifndef QTB_RESULT_H
#define QTB_RESULT_H

#include <Python.h>

typedef enum {
  QTB_RESULT_SUCCESS,
  QTB_RESULT_FAILURE,
} QtbResultType;

#define QtbResult_HEAD \
  QtbResultType type;

#define QtbResultSuccessful(result) (result.type == QTB_RESULT_SUCCESS)
#define QtbResultFailed(result) (result.type == QTB_RESULT_FAILURE)
#define QtbResultValue(result) (result.value.value)

typedef enum {
  QTB_RESULT_ERROR_NEW,
  QTB_RESULT_ERROR_STORED,
} QtbResultErrorType;

typedef struct {
  PyObject *py_err_class;
  const char *message;
} QtbResultErrorNew;

typedef struct {
  PyObject *exc_value;
  PyObject *exc_type;
  PyObject *exc_traceback;
} QtbResultErrorStored;

typedef union {
  QtbResultErrorNew new;
  QtbResultErrorStored stored;
} QtbResultErrorValue;

typedef struct {
  QtbResultErrorType type;
  QtbResultErrorValue value;
} QtbResultError;

QtbResultError package_py_err(void);
void unpackage_py_err(QtbResultError);

#define QtbResultRegisterSuccess(type, value) ((type){QTB_RESULT_SUCCESS, {value}})
#define QtbResultRegisterFailure(type, py_err, message) ((type){ \
  QTB_RESULT_FAILURE, \
  .value.error={ \
    QTB_RESULT_ERROR_NEW, \
    .value.new={py_err, message} \
  } \
})
#define QtbResultRegisterFailureFromPyErr(type) ((type){QTB_RESULT_FAILURE, .value.error=package_py_err()})
#define QtbResultFailureRaise(result) unpackage_py_err(result.value.error)
#define QtbResultFailureMessage(result) (result.value.error.value.new.message)

// ===== empty result =====

typedef union {
  QtbResultError error;
} QtbResultValue;

typedef struct {
  QtbResult_HEAD
  QtbResultValue value;
} QtbResult;

#define QtbResultSuccess ((QtbResult){QTB_RESULT_SUCCESS})
#define QtbResultFailure(py_err, message) QtbResultRegisterFailure(QtbResult, py_err, message)
#define QtbResultFailureFromPyErr() QtbResultRegisterFailureFromPyErr(QtbResult)

// ===== size_t result =====

typedef union {
  size_t value;
  QtbResultError error;
} QtbResultSize_tValue;

typedef struct {
  QtbResult_HEAD
  QtbResultSize_tValue value;
} QtbResultSize_t;

#define QtbResultSize_tSuccess(value) QtbResultRegisterSuccess(QtbResultSize_t, value)
#define QtbResultSize_tFailure(py_err, message) QtbResultRegisterFailure(QtbResultSize_t, py_err, message)
#define QtbResultSize_tFailureFromPyErr() QtbResultRegisterFailureFromPyErr(QtbResultSize_t)

// ===== int result =====

typedef union {
  int value;
  QtbResultError error;
} QtbResultIntValue;

typedef struct {
  QtbResult_HEAD
  QtbResultIntValue value;
} QtbResultInt;

#define QtbResultIntSuccess(value) QtbResultRegisterSuccess(QtbResultInt, value)
#define QtbResultIntFailure(py_err, message) QtbResultRegisterFailure(QtbResultInt, py_err, message)
#define QtbResultIntFailureFromPyErr() QtbResultRegisterFailureFromPyErr(QtbResultInt)

// ===== char *result =====

typedef union {
  char *value;
  QtbResultError error;
} QtbResultCharPtrValue;

typedef struct {
  QtbResult_HEAD
  QtbResultCharPtrValue value;
} QtbResultCharPtr;

#define QtbResultCharPtrSuccess(value) QtbResultRegisterSuccess(QtbResultCharPtr, value)
#define QtbResultCharPtrFailure(py_err, message) QtbResultRegisterFailure(QtbResultCharPtr, py_err, message)
#define QtbResultCharPtrFailureFromPyErr() QtbResultRegisterFailureFromPyErr(QtbResultCharPtr)

// ===== PyObject *result =====

typedef union {
  PyObject *value;
  QtbResultError error;
} QtbResultPyObjectPtrValue;

typedef struct {
  QtbResult_HEAD
  QtbResultPyObjectPtrValue value;
} QtbResultPyObjectPtr;

#define QtbResultPyObjectPtrSuccess(value) QtbResultRegisterSuccess(QtbResultPyObjectPtr, value)
#define QtbResultPyObjectPtrFailure(py_err, message) QtbResultRegisterFailure(QtbResultPyObjectPtr, py_err, message)
#define QtbResultPyObjectPtrFailureFromPyErr() QtbResultRegisterFailureFromPyErr(QtbResultPyObjectPtr)

#endif
