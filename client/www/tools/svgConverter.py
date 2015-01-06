#
#   Convert Inkscape SVG's into a raphael object and
#   hope that ERic found it usefull :)
#

from xml.etree.ElementTree import ElementTree, parse
import sys
from re import search
import os
import math


def convert_all():
    directory = os.getcwd()
    extension = ".svg"
    [convert_file(file) for file in os.listdir(directory) if file.lower().endswith(extension)]
def removepx(val):
    if isinstance(val, str):
        return search(r'[0-9]+', val).group()
    else:
        return val

def convert_file(filename):
    doc = ElementTree(file=filename)
    tree = parse(filename)
    root = tree.getroot()
    rootwidth = removepx(root.get('width'))
    print(rootwidth)
    rootheight = removepx(root.get('height'))
    print(rootheight)
    with open(filename+'.converted.js', 'w') as f:
        if (str(rootwidth) != 'none'):
            f.write('var width = '+str(math.ceil(float(rootwidth)))+';\n')
        else:
            f.write('var width = 100%;\n')
        if (str(rootheight) != 'none'):
            f.write('var height = '+str(math.ceil(float(rootheight)))+';\n')
        else:
            f.write('var height = 100%;\n')

        f.write('var paper = Raphael(\"paper\", width, height);\nvar text = "Width = \"+width+\"\\n Height = \"+height;\n\\nvar paths = [')
        totalpaths = doc.findall("./{http://www.w3.org/2000/svg}path")
        lastpath = totalpaths.pop()
        for result in totalpaths:
            newid = result.get('id').replace('-', '').replace('.','e').replace(' ','n').upper()
            data = result.get('id').split('.')
            data = '\',\''.join(map(str, data))
            print (data)
            def formatattr(attr):
                attributes = ''
                if (search(r'fill:(#[a-zA-Z0-9]{6})', attr)):
                    fill = search(r'fill:(#[a-zA-Z0-9]{6})', attr).group(1)
                else:
                    fill = 'none'
                if (search(r'stroke:(#[a-zA-Z0-9]{6})', attr)):
                    stroke = search(r'stroke:(#[a-zA-Z0-9]{6})', attr).group(1)
                else:
                    stroke = '0'
                if (search(r'stroke-width:([0-9*.0-9]+)', attr)):
                    width = str(int(math.ceil(float(search(r'stroke-width:([0-9*.0-9]+)', attr).group(1)))))

                elif (search(r'stroke-width:([1-9]+)', attr)):
                    width = search(r'stroke-width:([0-9]+)', attr).group(1)
                else:
                    width = '0'
                attributes = '\'' + fill + '\',\'' + stroke + '\', ' + width
                return attributes
            f.write('\n    [\"' + result.get('d') + '\", \n'+ '    [' + formatattr(result.get('style')) + '],[' + '\'' + data + '\'' + ']], \n' )

        #Write the last path with no trailing comma
        f.write('\n    [\"' + lastpath.get('d') + '\", \n'+ '    [' + formatattr(lastpath.get('style')) + '],[' + '\'' + data + '\'' + ']]\n' )

        f.write(']\n')
        f.write('\
var mapset = paper.set();\n\
var length = paths.length;\n\
var c = 0;\n\
for ( ;c < length; c++) {\n\
      //Draw the shapes with their attributes\n\
      var item = paths[c];\n\
      var cpath = item[0];\n\
      var cattr = item[1];\n\
      var cdata = item[2];\n\
      var cfill = cattr[0];\n\
      var cstroke = cattr[1];\n\
      var cstrokewidth = cattr[2];\n\
      var current = paper.path(cpath).attr({fill: cfill, stroke: cstroke, \'stroke-width\': cstrokewidth}).data(cdata);\n\
      mapset.push(current);\n\
    }\n')

if __name__ == '__main__':
    convert_all()
