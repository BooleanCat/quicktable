from setuptools import setup, Extension

quicktable = Extension(
    'quicktable',
    [
        'src/lib/quicktable.c',
        'src/lib/table.c',
        'src/lib/blueprint.c',
        'src/lib/column.c',
        'src/lib/result.c',
    ],
)

setup(
    name='quicktable',
    version='0.0',
    ext_modules=[quicktable],
    include_dirs=['src/include'],
    zip_safe=True,
)
