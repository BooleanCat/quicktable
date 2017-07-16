from setuptools import setup, Extension

quicktable = Extension(
    'quicktable',
    ['src/lib/quicktable.c', 'src/lib/table.c'],
)

setup(
    name='quicktable',
    version='0.0',
    ext_modules=[quicktable],
    include_dirs=['src/include'],
    zip_safe=True,
)
