CC = gcc
CFLAGS = ${PY_CFLAGS} -lcmocka
LDFLAGS = ${PY_LDFLAGS}

qtab_test_column: test_column.c
	$(CC) -o qtab_test_column $(CFLAGS) $(LDFLAGS) test_column.c
