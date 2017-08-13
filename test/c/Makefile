CC = gcc
CFLAGS = ${PY_CFLAGS}
LDFLAGS = ${PY_LDFLAGS}

qtab_column.o: ../../src/lib/column.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

qtab_test_column.o: test_column.c
	$(CC) -o $@ -c $(CFLAGS) -I../../src/include $^

qtab_test_column: qtab_column.o qtab_test_column.o
	$(CC) $^ -o $@ $(PY_LDFLAGS) -lcmocka

clean:
	rm -rf qtab_*
