import pytest
import textwrap
import quicktable


@pytest.fixture
def table():
    return quicktable.Table([
        ('Name', 'str'),
        ('Level', 'int'),
        ('Wild', 'bool'),
        ('Power', 'float'),
    ])


def test_print_empty_table():
    assert str(quicktable.Table([])) == ''


def test_print_len_0_table(table):
    expected = "| Name (str) | Level (int) | Wild (bool) | Power (float) |"
    assert str(table) == expected
