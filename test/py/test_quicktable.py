import pytest
import quicktable


@pytest.fixture
def empty_table():
    return quicktable.Table([])


def test_empty_table_has_len_0(empty_table):
    assert len(empty_table) == 0


def test_append_increments_length(empty_table):
    empty_table.append()
    assert len(empty_table) == 1


def test_append_increments_length_twice(empty_table):
    empty_table.append()
    empty_table.append()
    assert len(empty_table) == 2


def test_pop_decrements_length(empty_table):
    empty_table.append()
    empty_table.pop()
    assert len(empty_table) == 0


def test_pop_decrements_length_twice(empty_table):
    empty_table.append()
    empty_table.append()
    empty_table.pop()
    empty_table.pop()
    assert len(empty_table) == 0


def test_pop_empty_table_raises_exception(empty_table):
    with pytest.raises(IndexError) as excinfo:
        empty_table.pop()
    assert str(excinfo.value) == 'pop from empty table'


def test_pop_empty_table_keeps_len_same(empty_table):
    with pytest.raises(IndexError) as excinfo:
        empty_table.pop()
    assert len(empty_table) == 0
