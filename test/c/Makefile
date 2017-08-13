CC = gcc
CFLAGS = ${PY_CFLAGS}
LDFLAGS = ${PY_LDFLAGS}

qtb_column.o: ../../src/lib/column.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

qtb_test_column.o: test_column.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

qtb_tests.o: tests.c
	$(CC) -o $@ -c $(CFLAGS) $^

qtb_tests: qtb_tests.o qtb_column.o qtb_test_column.o
	$(CC) $^ -o $@ $(PY_LDFLAGS) -lcmocka

clean:
	rm -rf qtb_*
