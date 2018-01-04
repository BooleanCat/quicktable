CFLAGS = ${PY_CFLAGS}
LDFLAGS = ${PY_LDFLAGS}
CC = gcc

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
OBJS = $(patsubst %,build/%,$(_OBJS))

build/helpers.o: helpers.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/column.o: ../../src/lib/column.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/table.o: ../../src/lib/table.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/blueprint.o: ../../src/lib/blueprint.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/column_as_string.o: ../../src/lib/column_as_string.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/result.o: ../../src/lib/result.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/test_column.o: test_column.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/test_append.o: test_append.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/test_column_as_string.o: test_column_as_string.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/test_result.o: test_result.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/test_table.o: test_table.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/tests.o: tests.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/qtb_tests: $(OBJS)
	$(CC) $^ -o $@ $(PY_LDFLAGS) -lcmocka

.PHONY: clean

clean:
	rm -rf build/*
