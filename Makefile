.PHONY: test test-c test-py clean uninstall

SHELL := /bin/bash

CC = gcc
CFLAGS = $(shell python-config --cflags)
LDFLAGS = $(shell python-config --ldflags)

install:
	python setup.py install

uninstall:
	pip uninstall --yes quicktable

test: build-c/qtb_tests test-c install test-py

test-py:
	py.test -v

clean: uninstall
	rm -rf build/*
	rm -rf build-c/*
	rm -rf quicktable.egg-info/*
	rm -rf dist/*

# ===== C tests =====

_OBJS = \
	column.o \
	column_as_string.o \
	result.o \
	table.o \
	blueprint.o \
	test_column.o \
	test_column_as_string.o \
	test_append.o \
	test_result.o \
	test_table.o \
	tests.o \
	helpers.o
OBJS = $(patsubst %,build-c/%,$(_OBJS))

test-c:
	./build-c/qtb_tests

build-c/helpers.o: test/c/helpers.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

build-c/column.o: src/lib/column/column.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

build-c/table.o: src/lib/table/table.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

build-c/blueprint.o: src/lib/blueprint.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

build-c/column_as_string.o: src/lib/column/column_as_string.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

build-c/result.o: src/lib/result.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

build-c/test_column.o: test/c/test_column.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

build-c/test_append.o: test/c/test_append.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

build-c/test_column_as_string.o: test/c/test_column_as_string.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

build-c/test_result.o: test/c/test_result.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

build-c/test_table.o: test/c/test_table.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

build-c/tests.o: test/c/tests.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

build-c/qtb_tests: $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS) -lcmocka
