libcsmerge
==========

Allows the merging of OpenType CFF charstrings. The charstrings should first be 'desubroutinized' and have the leading set-width value (if any) removed, leaving only valid PostScript. Here is an example contour represented as a Python list:

        [ 344, 'hmoveto', 65, 'hlineto', -6, 24, -3, 44, 93, 'vvcurveto', 163, 'vlineto', 113, -70, 48, -98, -52, -59, -13, -35, -55, 'vhcurveto', 28, -52, 'rlineto', 20, 33, 40, 19, 57, 'hhcurveto', 59, 43, -32, -72, 'hvcurveto', -30, 'vlineto', -90, -3, -48, -2, -38, -5, -32, -16, 'rlinecurve', -48, -22, -32, -43, -63, 'vvcurveto', -91, 64, -55, 86, 59, 46, 28, 35, 37, 'vhcurveto', -4, 181, 'rmoveto', -120, 'vlineto', -23, -22, -44, -39, -59, 'hhcurveto', -54, -34, 36, 52, 31, 14, 27, 27, 15, 'hvcurveto', 21, 11, 27, 6, 41, 1, 'rrcurveto', 'endchar' ]

Through the Python API only a single function, merge_charstrings, is exposed. The C++ API provides a Charstrings module and a Geometry module.


Build libcsmerge
----------------

        mkdir build
        cd build
        cmake -G "Unix Makefiles" ..
        make
