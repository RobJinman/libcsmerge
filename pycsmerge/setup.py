import os, sys
from setuptools import setup, Extension

module1 = Extension('_pycsmerge',
                    ['Pycsmerge.cpp'],
                    define_macros=[
                        ('APPROX_BEZIERS', 1),
                        ('CGAL_USE_GMP', 1),
                        ('CGAL_USE_MPFR', 1),
                        ('pycsmerge_EXPORTS', 1)
                    ],
                    include_dirs=[
                        '/usr/local/include',
                        '../libcsmerge/include'
                    ],
                    library_dirs=[
                        '/usr/local/lib',
                        '../build/libcsmerge'
                    ],
                    libraries=[
                        'csmerge',
                        'boost_python-py3{}'.format(sys.version_info.minor),
                        'mpfr',
                        'gmp',
                        'CGAL',
                        'CGAL_Core',
                        'boost_thread',
                        'boost_system',
                        'pthread'
                    ],
                    extra_compile_args=[
                        '-std=c++11',
                        '-O3',
                        '-frounding-math',
                        '-Wall',
                        '-fPIC',
                        '-Wl,-soname,_pycsmerge.so'
                    ])

setup(
    name='Pycsmerge',
    version='1.0.0',
    author='Rob Jinman',
    author_email='info@recursiveloop.org',
    packages=['pycsmerge'],
    license='Proprietry',
    install_requires=[''],
    ext_modules=[ module1 ])
