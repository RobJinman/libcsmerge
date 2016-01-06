from pycsmerge import *


if __name__ == '__main__':
    cs1 = [
        0, 0, 'rmoveto',
        100, 'hlineto',
        100, 'vlineto',
        -100, 'hlineto',
        -100, 'vlineto',
        'endchar'
    ]

    cs2 = [
        50, 50, 'rmoveto',
        100, 'hlineto',
        100, 'vlineto',
        -100, 'hlineto',
        -100, 'vlineto',
        'endchar'
    ]

    cs3 = merge_charstrings(cs1, cs2)

    print(cs3)
