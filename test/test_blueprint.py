import pytest
import quicktable


def test_empty_blueprint_is_allowed():
    quicktable.Table([])


def test_tables_with_no_blueprint_raises_exception():
    with pytest.raises(TypeError) as excinfo:
        quicktable.Table()
    assert str(excinfo.value) == 'function takes exactly 1 argument (0 given)'


def test_invalid_blueprint():
    with pytest.raises(TypeError) as excinfo:
        quicktable.Table(None)
    assert str(excinfo.value) == 'invalid blueprint'
