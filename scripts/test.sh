#!/usr/bin/env bash
set -e

./scripts/install.sh >/dev/null
py.test
