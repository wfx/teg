## $Id: theme.py,v 1.4 2003/09/09 18:02:13 riq Exp $
##
## Tenes Empanadas Graciela
##
## Copyright (C) 2000 Ricardo Quesada
##
##  Author: Ricardo Calixto Quesada <riq@coresecurity.com>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; only version 2 of the License
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.



import os
import xml.dom.minidom
import pygame

# TEG imports
import country

class Theme(object):

    themeName = 'm3'
    themePrefix = ''

    def __init__(self, themeName = 'm3'):

        Theme.Theme = themeName
#        self._themePrefix = os.path.join( os.environ['HOME'], '.teg', 'themes', Theme.themeName )
        Theme.themePrefix = os.path.join( '..','..','client','themes', Theme.themeName )
        file_name =  os.path.join( Theme.themePrefix, 'teg_theme.xml' )

        self._themeDom = xml.dom.minidom.parse( file_name )

        self.defaultDict()

    def defaultDict( self ):
        self.themeDict = {}
        self.themeDict['continents'] = {}


    def parseTheme(self):
        
        teg_theme = self._themeDom.getElementsByTagName('teg_theme')[0]
        self.processElements(teg_theme, self.themeDict)


    def processElements(self, element, dictionary, parent = None):

        for node in element.childNodes:
            if node.nodeType == node.ELEMENT_NODE:
                try:
                    method = getattr(self, '_parse_%s' % node.nodeName )
                    method( node, dictionary, parent )
                except AttributeError, e:
                    print 'Element not handled: %s' % node.nodeName
                    print str(e)

    def loadImage( name ):
        """ Load image and return image object"""
        fullname = os.path.join( Theme.themePrefix, name)
        try:
            image = pygame.image.load(fullname)
            if image.get_alpha is None:
                image = image.convert()
            else:
                image = image.convert_alpha()
        except pygame.error, message:
            print 'Cannot load image:', fullname
            raise SystemExit, message
        return image
    loadImage = staticmethod( loadImage )

    ###
    ### XML handlers
    ###
    def _parse_continent( self, element, dictionary, parent ):
        c = country.Continent()
        name =  element.attributes.getNamedItem('name').nodeValue
#        print 'Parsing: %s' % name
        c['name'] = name
        c['offset'] = ( int( element.attributes.getNamedItem('pos_x').nodeValue ),
                         int( element.attributes.getNamedItem('pos_y').nodeValue ) )
        c['countries'] = {}

        dictionary['continents'][name] = c
        self.processElements( element,  dictionary['continents'][name]['countries'], c )


    def _parse_country( self, element, dictionary, parent ):
        name =  element.attributes.getNamedItem('name').nodeValue

        c = country.Country()
        c['name'] = name
        c['file'] = element.attributes.getNamedItem('file').nodeValue
        c['image'] = Theme.loadImage( c['file'] )
        c['continent'] = parent['name']
        c['offset'] = ( int( element.attributes.getNamedItem('pos_x').nodeValue ),
                                    int( element.attributes.getNamedItem('pos_y').nodeValue ) )
        try:
            c['army_offset'] = ( int( element.attributes.getNamedItem('army_x').nodeValue ),
                                            int( element.attributes.getNamedItem('army_y').nodeValue ) )
        except AttributeError, e:
            c['army_offset'] = (0,0)

        dictionary[name] = c


    def _parse_dices_pos( self, element, dictionary, parent ):
        pass


    def _parse_dices_ext_pos( self, element, dictionary, parent ):
        pass

    def _parse_armies_pos( self, element, dictionary, parent ):
        pass

    def _parse_dices_text( self, element, dictionary, parent ):
        pass

    def _parse_author( self, element, dictionary, parent ):
        pass
        
    def _parse_email( self, element, dictionary, parent ):
        pass

    def _parse_version( self, element, dictionary, parent ):
        pass

    def _parse_screenshot( self, element, dictionary, parent ):
        pass

    def _parse_screen( self, element, dictionary, parent ):
        dictionary['screen'] = {}
        dictionary['screen']['size'] = ( int( element.attributes.getNamedItem('size_x').nodeValue ),
                                            int( element.attributes.getNamedItem('size_y').nodeValue ) )
        

    def _parse_toolbar( self, element, dictionary, parent ):
        dictionary['toolbar'] = {}
        dictionary['toolbar']['offset'] = ( int( element.attributes.getNamedItem('offset_x').nodeValue ),
                                            int( element.attributes.getNamedItem('offset_y').nodeValue ) )
        dictionary['toolbar']['image'] = Theme.loadImage( element.attributes.getNamedItem('name').nodeValue )
        dictionary['toolbar']['text_color'] = element.attributes.getNamedItem('text_color').nodeValue

    def _parse_map( self, element, dictionary, parent ):
        dictionary['map'] = {}
        dictionary['map']['offset'] = ( int( element.attributes.getNamedItem('offset_x').nodeValue ),
                                            int( element.attributes.getNamedItem('offset_y').nodeValue ) )
        dictionary['map']['image'] = Theme.loadImage( element.attributes.getNamedItem('file').nodeValue )

    def _parse_cards( self, element, dictionary, parent ):
        pass

    def __del__(self):
        self._themeDom.unlink()
