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


def test_print_len_1_table(table):
    expected = textwrap.dedent("""
        | Name (str) | Level (int) | Wild (bool) | Power (float) |
        | Pikachu    | 14          | False       | 14.23         |
    """).strip()
    table.append(['Pikachu', 14, False, 14.23])
    assert str(table) == expected


def test_print_len_5_table(table):
    expected = textwrap.dedent("""
        | Name (str) | Level (int) | Wild (bool) | Power (float) |
        | Pikachu    | 14          | False       | 14.23         |
        | Charmander | 65          | True        | 134.65        |
        | Zubat      | 19          | True        | 4.30          |
        | Mewtwo     | 100         | True        | 543.00        |
        | Jigglypuff | 42          | False       | 97.24         |
    """).strip()
    table.append(['Pikachu', 14, False, 14.23])
    table.append(['Charmander', 65, True, 134.65])
    table.append(['Zubat', 19, True, 4.3])
    table.append(['Mewtwo', 100, True, 543.0])
    table.append(['Jigglypuff', 42, False, 97.24])
    assert str(table) == expected


def test_print_len_6_table_truncates(table):
    expected = textwrap.dedent("""
        | Name (str) | Level (int) | Wild (bool) | Power (float) |
        | Pikachu    | 14          | False       | 14.23         |
        | Charmander | 65          | True        | 134.65        |
        | Zubat      | 19          | True        | 4.30          |
        | Mewtwo     | 100         | True        | 543.00        |
        | Jigglypuff | 42          | False       | 97.24         |
    """).strip()
    table.append(['Pikachu', 14, False, 14.23])
    table.append(['Charmander', 65, True, 134.65])
    table.append(['Zubat', 19, True, 4.3])
    table.append(['Mewtwo', 100, True, 543.0])
    table.append(['Jigglypuff', 42, False, 97.24])
    table.append(['Wigglytuff', 43, True, 113.49])
    assert str(table) == expected


def test_print_table_long_cell_pads_header(table):
    expected = textwrap.dedent("""
        | Name (str)  | Level (int) | Wild (bool) | Power (float) |
        | Pikachu     | 14          | False       | 14.23         |
        | Fletchinder | 9           | True        | 9.54          |
    """).strip()
    table.append(['Pikachu', 14, False, 14.23])
    table.append(['Fletchinder', 9, True, 9.54])
    assert str(table) == expected
