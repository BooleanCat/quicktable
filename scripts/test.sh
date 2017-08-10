#!/usr/bin/env bash
set -e

export PY_CFLAGS="$( python-config --cflags )"
export PY_LDFLAGS="$( python-config --ldflags )"

pushd test/c >/dev/null
  make clean
  make qtab_test_column
  ./qtab_test_column
  make clean
popd >/dev/null

./scripts/install.sh >/dev/null
py.test
