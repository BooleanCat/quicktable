import pytest
import quicktable


@pytest.fixture
def table():
    return quicktable.Table([
        ('Name', 'str'),
        ('Level', 'int'),
    ])


def test_append(table):
    table.append(['Pikachu', 12])


def test_append_non_sequence_is_invalid(table):
    with pytest.raises(TypeError) as excinfo:
        table.append(None)
    assert str(excinfo.value) == 'append with non-sequence'


def test_append_mismatching_length_is_invalid(table):
    with pytest.raises(TypeError) as excinfo:
        table.append(['Pikachu'])
    assert str(excinfo.value) == 'append with mismatching row length'
