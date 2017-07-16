#!/usr/bin/env bash
set -e

if [[ ! -z "$( pip show quicktable )" ]]; then
  pip uninstall --yes quicktable
fi

python setup.py install
