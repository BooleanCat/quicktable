import quicktable

def test_empty_table_has_len_0():
    assert len(quicktable.Table()) == 0

def test_append_increments_length():
    table = quicktable.Table()
    table.append()
    assert len(table) == 1

def test_pop_decrements_length():
    table = quicktable.Table()
    table.append()
    table.pop()
    assert len(table) == 0
