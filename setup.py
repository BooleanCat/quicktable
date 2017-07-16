from setuptools import setup, Extension

quicktable = Extension('quicktable', ['src/lib/quicktable.c'])

setup(
    name='quicktable',
    version='0.0',
    ext_modules=[quicktable],
    zip_safe=True,
)
