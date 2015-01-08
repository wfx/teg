#
#   Convert Inkscape SVG's into raphael js code
#
#   This ugly py needs some touch.
#   TODO:
#   -   give map name as option
#   -   fill:url(#id) ?
#   ===========================================

import sys
import os
import math
import json
from xml.etree.ElementTree import ElementTree, parse
from re import search, split

def mk_path(path):
    return path

def mk_attr(attr):
    s = attr
    l = split(':|;',s)
    i = iter(l)
    d = dict(zip(i,i))
    return json.dumps(d)

def mk_data(data):
    d = data.split('-')
    if (len(d)==3):
        d = '\'id\':\'' + d[2]+'_'+d[1] + '\', \'group\':\'' + d[0].replace('_', ' ') + '\', \'name\':\'' + d[2].replace('_', ' ') + '\''
    else:
        return False
    return str(d)

def svg2raphael(target):
    directory = os.getcwd()

    fpath = '../themes/'
    fsvg = fpath + target + '.svg'
    fjs = fpath + target + '.js'

    SVG_NS = "http://www.w3.org/2000/svg"
    tree = ElementTree()
    tree.parse(fsvg)

    with open(fjs, 'w') as f:
        f.write('function playfield(c) {\n')
        f.write('var m = Raphael($(c).attr(\'id\'), $(c).attr(\'width\'), $(c).attr(\'height\'));\n')
        f.write('var obj = [];\n')

        # Def's - Gradiant's
        # ------------------------------------------------------
        # From:
        # <linearGradient id="linearGradient4043">
        #     <stop style="stop-color:#aca641;stop-opacity:1;" offset="0" id="stop4045" />
        #     <stop style="stop-color:#848c25;stop-opacity:1;" offset="1" id="stop4047" />
        # </linearGradient>
        # To:
        # fill: "r(.5,.5)#fff-#fff:70-#000", "opacityStops": "1-0-0.6"

        for node in tree.findall('.//{%s}linearGradient' % SVG_NS):
            print(node.get('id'))
        for node in tree.findall('.//{%s}stop' % SVG_NS):
            print(node.get('style'))

        # ------------------------------------------------------

        # Path
        for node in tree.findall('.//{%s}path' % SVG_NS):
            p = mk_path(node.get('d'))
            a = mk_attr(node.get('style'))
            d = mk_data(node.get('id'))
            if (d != False):
                #print('var c = map.path(\'' + p + '\');\n')
                #print('c.attr({' + a + '}).data({' + d + '});\n')
                f.write('var c = m.path(\'' + p + '\');\n')
                f.write('c.attr(' + a + ').data({' + d + '});\n')
                f.write('obj.push(c);\n')
        f.write('return obj;\n')
        f.write('}')
    f.close()
    return

if __name__ == '__main__':
    svg2raphael('map_risk')
