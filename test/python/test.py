from pycsmerge import *


if __name__ == '__main__':
    glyph = [
        86, 'hmoveto', 74, 659, -74, 'hlineto', 'endchar'
    ]

    watermark = [
        0, -240, 'rmoveto', 16.25, 0, 'rlineto', 106.75, 417.70729053318826, 'rlineto', 106.75, -417.70729053318826, 'rlineto', 16.25, 0, 'rlineto', -114.875, 449.5, 'rlineto', 114.875, 449.5, 'rlineto', -16.25, 0, 'rlineto', -106.75, -417.70729053318826, 'rlineto', -106.75, 417.70729053318826, 'rlineto', -16.25, 0, 'rlineto', 114.875, -449.5, 'rlineto', -114.875, -449.5, 'rlineto', 0, 240, 'rmoveto'
    ]

    glyph2 = [
        472, 368, 'rmoveto', -133, 225, 258, 66, -589, -66, 257, -225, -133, -66, 133, -302, 74, 302, 133, 'hlineto', 'endchar'
    ]

    watermark2 = [
        0, -237, 'rmoveto', 28.3625, 0, 'rlineto', 274.1375, 425.96466584292557, 'rlineto', 274.13750000000005, -425.96466584292557, 'rlineto', 28.362499999999955, 0, 'rlineto', -288.31875, 448.0, 'rlineto', 288.31875, 448.0, 'rlineto', -28.362499999999955, 0, 'rlineto', -274.13750000000005, -425.96466584292557, 'rlineto', -274.1375, 425.96466584292557, 'rlineto', -28.3625, 0, 'rlineto', 288.31875, -448.0, 'rlineto', -288.31875, -448.0, 'rlineto', 0, 237, 'rmoveto'
    ]

    print('one')
    res1 = merge_charstrings(glyph2, watermark2)
    print('two')
    res2 = merge_charstrings(glyph, watermark)
    print('three')
