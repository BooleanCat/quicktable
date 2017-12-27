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


def test_integration(table):
    assert len(table) == 0
    assert str(table) == textwrap.dedent("""
        | Name (str) | Level (int) | Wild (bool) | Power (float) |
    """).strip()

    table.append(['Pikachu', 18, True, 32.16])

    assert len(table) == 1
    assert table[0] == ['Pikachu', 18, True, 32.16]
    assert str(table) == textwrap.dedent("""
        | Name (str) | Level (int) | Wild (bool) | Power (float) |
        | Pikachu    | 18          | True        | 32.16         |
    """).strip()

    table.append(['Charizard', 45, False, 164.3])

    assert len(table) == 2
    assert table[1] == ['Charizard', 45, False, 164.3]
    assert str(table) == textwrap.dedent("""
        | Name (str) | Level (int) | Wild (bool) | Power (float) |
        | Pikachu    | 18          | True        | 32.16         |
        | Charizard  | 45          | False       | 164.30        |
    """).strip()

    row = table.pop()

    assert row == ['Charizard', 45, False, 164.3]
    assert len(table) == 1
    assert str(table) == textwrap.dedent("""
        | Name (str) | Level (int) | Wild (bool) | Power (float) |
        | Pikachu    | 18          | True        | 32.16         |
    """).strip()

    for i in range(1000):
        table.append(['Charizard', i, False, 164.3])

    assert len(table) == 1001
    assert str(table) == textwrap.dedent("""
        | Name (str) | Level (int) | Wild (bool) | Power (float) |
        | Pikachu    | 18          | True        | 32.16         |
        | Charizard  | 0           | False       | 164.30        |
        | Charizard  | 1           | False       | 164.30        |
        | Charizard  | 2           | False       | 164.30        |
        | Charizard  | 3           | False       | 164.30        |
    """).strip()

    row = table.pop()

    assert row == ['Charizard', 999, False, 164.3]
