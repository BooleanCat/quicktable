.PHONY: test test-c test-py

SHELL := /bin/bash

install:
	python setup.py install

test: install test-c test-py

test-c:
	test/c/scripts/test.sh

test-py:
	py.test -v
