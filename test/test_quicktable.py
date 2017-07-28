import pytest
import quicktable


def test_empty_table_has_len_0():
    assert len(quicktable.Table([])) == 0


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


def test_pop_empty_table_raises_exception():
    table = quicktable.Table([])
    with pytest.raises(IndexError) as excinfo:
        table.pop()
    assert str(excinfo.value) == 'pop from empty table'


def test_pop_empty_table_keeps_len_same():
    table = quicktable.Table([])
    with pytest.raises(IndexError) as excinfo:
        table.pop()
    assert len(table) == 0


def test_table_accepts_empty_blueprint():
    quicktable.Table([])


def test_tables_with_no_blueprint_raises_exception():
    with pytest.raises(TypeError) as excinfo:
        quicktable.Table()
    assert str(excinfo.value) == 'function takes exactly 1 argument (0 given)'
