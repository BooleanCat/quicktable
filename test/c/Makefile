CFLAGS = ${PY_CFLAGS}
LDFLAGS = ${PY_LDFLAGS}
CC = gcc

_OBJS = column.o test_column.o test_append.o tests.o helpers.o
OBJS = $(patsubst %,build/%,$(_OBJS))

build/helpers.o: helpers.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/column.o: ../../src/lib/column.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/test_column.o: test_column.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/test_append.o: test_append.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

build/tests.o: tests.c
	$(CC) -o $@ -c $(CFLAGS) $^

build/qtb_tests: $(OBJS)
	$(CC) $^ -o $@ $(PY_LDFLAGS) -lcmocka

.PHONY: clean

clean:
	rm -rf build/*
