import os, sys
from distutils.core import setup, Extension

print(sys.version_info);

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
                        '/usr/include/python3.{}m'.format(sys.version_info.minor),
                        '../libcsmerge/include'
                    ],
                    library_dirs=[
                        '/usr/local/lib',
                        '../build/libcsmerge'
                    ],
                    libraries=[
                        'csmerge',
                        'boost_python37',
                        'mpfr',
                        'gmp',
                        'CGAL',
                        'CGAL_Core',
                        'boost_thread-mt',
                        'boost_system-mt',
                        'pthread'
                    ],
                    extra_compile_args=[
                        '-stdlib=libc++',
                        '-mmacosx-version-min=10.11',
                        '-std=c++11',
                        '-O3',
                        '-frounding-math',
                        '-Wall',
                        '-fPIC',
                        '-Wl,-soname,_pycsmerge.so'
                    ],
                    extra_link_args=[
                        '-stdlib=libc++',
                        '-mmacosx-version-min=10.11'
                    ])

setup(
    name='Pycsmerge',
    version='1.0.0',
    author='Rob Jinman',
    author_email='jinmanr@gmail.com',
    packages=['pycsmerge'],
    license='Proprietry',
    install_requires=[''],
    ext_modules=[ module1 ])
