.PHONY: test test-c test-py

SHELL := /bin/bash

CC = gcc
CFLAGS = $(shell python-config --cflags)
LDFLAGS = $(shell python-config --ldflags)

install:
	python setup.py install

test: test/c/build/qtb_tests test-c install test-py

test-py:
	py.test -v

clean:
	rm -rf test/c/build/*

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
OBJS = $(patsubst %,test/c/build/%,$(_OBJS))

test-c:
	./test/c/build/qtb_tests

test/c/build/helpers.o: test/c/helpers.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

test/c/build/column.o: src/lib/column/column.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

test/c/build/table.o: src/lib/table/table.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

test/c/build/blueprint.o: src/lib/blueprint.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

test/c/build/column_as_string.o: src/lib/column/column_as_string.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

test/c/build/result.o: src/lib/result.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

test/c/build/test_column.o: test/c/test_column.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

test/c/build/test_append.o: test/c/test_append.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

test/c/build/test_column_as_string.o: test/c/test_column_as_string.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

test/c/build/test_result.o: test/c/test_result.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

test/c/build/test_table.o: test/c/test_table.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

test/c/build/tests.o: test/c/tests.c
	$(CC) -o $@ -c $(CFLAGS) -Isrc/include $^

test/c/build/qtb_tests: $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS) -lcmocka
