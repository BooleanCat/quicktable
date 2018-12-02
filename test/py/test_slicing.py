import pytest
import quicktable


@pytest.fixture
def table():
    return quicktable.Table([
        ('Name', 'str'),
        ('Level', 'int'),
        ('Wild', 'bool'),
        ('Power', 'float'),
    ])


def test_slice_when_len_0_is_invalid(table):
    with pytest.raises(IndexError) as excinfo:
        table[0]
    assert str(excinfo.value) == 'table index out of range'


def test_slice(table):
    table.append(['Pikachu', 12, False, 12.2])
    assert table[0] == ['Pikachu', 12, False, 12.2]


def test_slice_two_rows(table):
    table.append(['Pikachu', 12, False, 12.2])
    table.append(['Raichu', 45, True, 14.0])

    assert table[0] == ['Pikachu', 12, False, 12.2]
    assert table[1] == ['Raichu', 45, True, 14.0]


def test_slice_negative(table):
    table.append(['Pikachu', 12, False, 12.2])
    assert table[-1] == ['Pikachu', 12, False, 12.2]


def test_slice_negative_two_rows(table):
    table.append(['Pikachu', 12, False, 12.2])
    table.append(['Raichu', 45, True, 14.0])

    assert table[-1] == ['Raichu', 45, True, 14.0]
    assert table[-2] == ['Pikachu', 12, False, 12.2]
