#!/usr/bin/env bash
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TEST_DIR="$( dirname "$DIR" )"

export PY_CFLAGS="$( python-config --cflags )"
export PY_LDFLAGS="$( python-config --ldflags )"

pushd "$TEST_DIR" >/dev/null
  make clean
  make qtb_test_column
  ./qtb_test_column
  make clean
popd >/dev/null
