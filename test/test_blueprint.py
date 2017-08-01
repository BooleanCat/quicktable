import pytest
import quicktable


def test_empty_blueprint_is_allowed():
    quicktable.Table([])


def test_tables_with_no_blueprint_raises_exception():
    with pytest.raises(TypeError) as excinfo:
        quicktable.Table()
    assert str(excinfo.value) == 'function takes exactly 1 argument (0 given)'


def test_non_sequence_is_invalid_blueprint():
    with pytest.raises(TypeError) as excinfo:
        quicktable.Table(None)
    assert str(excinfo.value) == 'invalid blueprint'


def test_non_sequence_column_descriptor_is_invalid():
    with pytest.raises(TypeError) as excinfo:
        quicktable.Table([None])
    assert str(excinfo.value) == 'invalid blueprint'


def test_length_of_column_descriptor_not_two_is_invalid():
    with pytest.raises(TypeError) as excinfo:
        quicktable.Table([('Name')])
    assert str(excinfo.value) == 'invalid blueprint'


def test_column_name_not_string():
    with pytest.raises(TypeError) as excinfo:
        quicktable.Table([(None, 'str')])
    assert str(excinfo.value) == 'invalid blueprint'


def test_second_column_name_not_string():
    with pytest.raises(TypeError) as excinfo:
        quicktable.Table([('Name', 'str'), (None, 'str')])
    assert str(excinfo.value) == 'invalid blueprint'


def test_column_type_not_string():
    with pytest.raises(TypeError) as excinfo:
        quicktable.Table([('Name', None)])
    assert str(excinfo.value) == 'invalid blueprint'


def test_second_column_type_not_string():
    with pytest.raises(TypeError) as excinfo:
        quicktable.Table([('Name', 'str'), ('Level', None)])
    assert str(excinfo.value) == 'invalid blueprint'
