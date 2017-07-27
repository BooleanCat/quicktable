import pytest
import quicktable


def test_empty_table_has_len_0():
    assert len(quicktable.Table()) == 0

def test_append_increments_length():
    table = quicktable.Table()
    table.append()
    assert len(table) == 1

def test_append_increments_length_twice():
    table = quicktable.Table()
    table.append()
    table.append()
    assert len(table) == 2

def test_pop_decrements_length():
    table = quicktable.Table()
    table.append()
    table.pop()
    assert len(table) == 0

def test_pop_decrements_length_twice():
    table = quicktable.Table()
    table.append()
    table.append()
    table.pop()
    table.pop()
    assert len(table) == 0

def test_pop_empty_table_raises_error():
    table = quicktable.Table()
    with pytest.raises(IndexError) as excinfo:
        table.pop()
    assert str(excinfo.value) == 'pop from empty table'
