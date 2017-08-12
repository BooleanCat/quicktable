#!/usr/bin/env bash
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$( dirname "$( dirname "$DIR" )" )" )"

docker run \
  -v "$PROJECT_ROOT:/home/test/quicktable" \
  -w /home/test/quicktable/test/c \
  quicktable-test-c \
  scripts/_test.sh
