quicktable
==========

A module for easy manipulation of tabular data.

Installation
------------
```shell
git clone https://github.com/BooleanCat/quicktable
cd quicktable && ./scripts/install.sh
```

A quick look
------------

```
>>> import quicktable
>>> table = quicktable.Table([
...   ('Name', 'str'),
...   ('Level', 'int'),
...   ('Loyal', 'bool'),
... ])
>>> table.append(['Pikachu', 24, True])
>>> table.append(['Charmander', 14, False])
>>> print(table)
| Name       | Level | Loyal |
| Pikachu    | 24    | True  |
| Charmander | 14    | False |
>>> table = table.sort('Name')
>>> print(table)
| Name       | Level | Loyal |
| Charmander | 14    | False |
| Pikachu    | 24    | True  |
```

Version `0.1.0` feature goals
-----------------------------
```python
import quicktable

table = quicktable.Table([
  ('Name', 'str'),
  ('Level', 'int'),
  ('Wild', 'bool'),
  ('Power', 'float'),
])

table.append(['Pikachu', 24, True, 23.1])
table.append(['Charmander', 12, False, 20.7])
row = table.pop()
table = table.sort('Name')
print(table)
len(table)
table.columns
table[0]
table[:5]
```
