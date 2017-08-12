#!/usr/bin/env bash
set -e

test/c/scripts/test.sh

scripts/install.sh >/dev/null
py.test
