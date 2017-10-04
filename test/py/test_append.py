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


def test_append(table):
    table.append(['Pikachu', 12, True, 12.5])


def test_append_1000_times(table):
    for i in range(1000):
        table.append(['Pikachu', i, True, 12.0])


def test_append_non_sequence_is_invalid(table):
    with pytest.raises(TypeError) as excinfo:
        table.append(None)
    assert str(excinfo.value) == 'append with non-sequence'


def test_append_mismatching_length_is_invalid(table):
    with pytest.raises(TypeError) as excinfo:
        table.append(['Pikachu'])
    assert str(excinfo.value) == 'append with mismatching row length'


def test_append_mismatching_type_for_str(table):
    with pytest.raises(TypeError) as excinfo:
        table.append([None, 12, True, 12.5])
    assert str(excinfo.value) == 'non-str entry for str column'


def test_append_mismatching_type_for_int(table):
    with pytest.raises(TypeError) as excinfo:
        table.append(['Pikachu', None, True, 12.5])
    assert str(excinfo.value) == 'non-int entry for int column'


def test_append_mismatching_type_for_float(table):
    with pytest.raises(TypeError) as excinfo:
        table.append(['Pikachu', 12, True, None])
    assert str(excinfo.value) == 'non-float entry for float column'


def test_append_mismatching_type_for_bool(table):
    with pytest.raises(TypeError) as excinfo:
        table.append(['Pikachu', 12, None, 12.5])
    assert str(excinfo.value) == 'non-bool entry for bool column'
