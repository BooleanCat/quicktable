#!/usr/bin/env bash
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TEST_DIR="$( dirname "$DIR" )"

export PY_CFLAGS
export PY_LDFLAGS
PY_CFLAGS="$( python-config --cflags )"
PY_LDFLAGS="$( python-config --ldflags )"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --untilItFails)
      UNTILITFAILS=true
    ;;
  esac

  shift
done

if [ ! -z "$UNTILITFAILS" ]; then echo hi; fi

pushd "$TEST_DIR" >/dev/null
  make clean
  make build/qtb_tests

  if [ ! -z "$UNTILITFAILS" ]; then
    while true; do
      ./build/qtb_tests
    done
  else
    ./build/qtb_tests
  fi

  make clean
popd >/dev/null
