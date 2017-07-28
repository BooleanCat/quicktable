import pytest
import quicktable


@pytest.fixture
def empty_table():
    return quicktable.Table([])


def test_empty_table_has_len_0(empty_table):
    assert len(empty_table) == 0


def test_append_increments_length():
    table = quicktable.Table([])
    table.append()
    assert len(table) == 1


def test_append_increments_length_twice():
    table = quicktable.Table([])
    table.append()
    table.append()
    assert len(table) == 2


def test_pop_decrements_length():
    table = quicktable.Table([])
    table.append()
    table.pop()
    assert len(table) == 0


def test_pop_decrements_length_twice():
    table = quicktable.Table([])
    table.append()
    table.append()
    table.pop()
    table.pop()
    assert len(table) == 0


def test_pop_empty_table_raises_exception(empty_table):
    with pytest.raises(IndexError) as excinfo:
        empty_table.pop()
    assert str(excinfo.value) == 'pop from empty table'


def test_pop_empty_table_keeps_len_same(empty_table):
    with pytest.raises(IndexError) as excinfo:
        empty_table.pop()
    assert len(empty_table) == 0


def test_tables_with_no_blueprint_raises_exception():
    with pytest.raises(TypeError) as excinfo:
        quicktable.Table()
    assert str(excinfo.value) == 'function takes exactly 1 argument (0 given)'


def test_column_names_not_setable(empty_table):
    with pytest.raises(AttributeError) as excinfo:
        empty_table.column_names = None
    assert str(excinfo.value) == "attribute 'column_names' of 'quicktable.Table' objects is not writable"


def test_empty_table_column_names_is_empty_tuple(empty_table):
    assert empty_table.column_names == tuple()


def test_invalid_blueprint():
    with pytest.raises(TypeError) as excinfo:
        quicktable.Table(None)
    assert str(excinfo.value) == 'invalid blueprint'
