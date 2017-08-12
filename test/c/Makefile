CC = gcc
CFLAGS = ${PY_CFLAGS} -lcmocka
LDFLAGS = ${PY_LDFLAGS}

qtab_column.o:
	$(CC) -o qtab_column.o -c $(CFLAGS) -I../../src/include ../../src/lib/column.c

qtab_test_column.o:
	$(CC) -o qtab_test_column.o -c $(CFLAGS) -I../../src/include test_column.c

qtab_test_column: qtab_column.o qtab_test_column.o
	$(CC) qtab_column.o qtab_test_column.o -o qtab_test_column $(PY_LDFLAGS) -lcmocka

clean:
	rm -rf qtab_*
