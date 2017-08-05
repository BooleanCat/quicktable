#!/usr/bin/env bash
set -e

pushd test/c >/dev/null
  rm -f qtab_*
  make qtab_test_column
  ./qtab_test_column
popd >/dev/null

./scripts/install.sh >/dev/null
py.test
