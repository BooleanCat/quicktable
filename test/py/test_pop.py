import pytest
import quicktable


@pytest.fixture
def table():
    table = quicktable.Table([
        ('Name', 'str'),
        ('Level', 'int'),
        ('Wild', 'bool'),
        ('Power', 'float'),
    ])
    table.append(['Raichu', 46, False, 175.0])
    return table


def test_test_pop(table):
    row = table.pop()
    assert row == ['Raichu', 46, False, 175.0]
