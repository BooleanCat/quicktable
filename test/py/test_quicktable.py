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


def test_columns_not_setable(empty_table):
    with pytest.raises(AttributeError) as excinfo:
        empty_table.columns = None
    assert str(excinfo.value) == "attribute 'columns' of 'quicktable.Table' objects is not writable"


def test_empty_table_columns_is_empty_tuple(empty_table):
    assert empty_table.columns == tuple()
