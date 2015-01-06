#
#   Convert Inkscape SVG's into a raphael object and
#   hope that Eric found it usefull :)
#
#   TODO: This ugly py needs a rewrite.
#

import sys
import os
import math
from xml.etree.ElementTree import ElementTree, parse
from re import search

def removepx(val):
    if isinstance(val, str):
        return search(r'[0-9]+', val).group()
    else:
        return val

def mk_path(path):
    return path

def mk_attr(attr):
    a = ''
    fill = 'none'
    stroke = '0'
    width = '0'

    if (search(r'fill:(#[a-zA-Z0-9]{6})', attr)):
        fill = search(r'fill:(#[a-zA-Z0-9]{6})', attr).group(1)
    if (search(r'stroke:(#[a-zA-Z0-9]{6})', attr)):
        stroke = search(r'stroke:(#[a-zA-Z0-9]{6})', attr).group(1)
    if (search(r'stroke-width:([0-9*.0-9]+)', attr)):
        width = str(int(math.ceil(float(search(r'stroke-width:([0-9*.0-9]+)', attr).group(1)))))
    elif (search(r'stroke-width:([1-9]+)', attr)):
        width = search(r'stroke-width:([0-9]+)', attr).group(1)

    attr = '\'' + fill + '\',\'' + stroke + '\', ' + width
    return attr

def mk_data(data):
    return data

def svg2raphael(target):
    directory = os.getcwd()

    fpath = '../themes/'
    fsvg = fpath + target + '.svg'
    fjs = fpath + target + '.js'

    SVG_NS = "http://www.w3.org/2000/svg"
    tree = ElementTree()
    tree.parse(fsvg)
    #doc = tree.getroot()

    #open(fjs, 'w') as f
    #f.write('var width = 100%;\n')
    #f.write('var height = 100%;\n')
    #f.wrtie('var map = Raphael(\"map\", width, height);\"\n')

    for node in tree.findall('.//{%s}path' % SVG_NS):
        p = mk_path(node.get('d'))
        a = mk_attr(node.get('style'))
        d = mk_attr(node.get('id'))
        print(a)

if __name__ == '__main__':
    svg2raphael('map_risk')
