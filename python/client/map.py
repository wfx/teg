## $Id: map.py,v 1.2 2003/09/08 03:29:55 riq Exp $
##
## Tenes Empanadas Graciela
##
## Copyright (C) 2000,2003 Ricardo Quesada
##
## Author: Ricardo Calixto Quesada <riq@coresecurity.com>
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


import pygame

# teg imports
import ttoolkit
import model

class Map( ttoolkit.TToolkit ):

    def __init__( self, theme ):
        ttoolkit.TToolkit.__init__( self )
        self._theme = theme
        self.setOrigin( self._theme['map']['offset'] )
        rect = self._theme['map']['image'].get_rect()
        self.setSize( (rect[2],rect[3]) )

    def _updateContinents( self ):

        list_of_countries = model.Model.getInstance().getCountries()

        for cont in self._theme['continents']:
            obj_cont = self._theme['continents'][cont]
            (x,y) = obj_cont['offset']
            for country in obj_cont['countries']:
                obj_country = obj_cont['countries'][country]
                (xc,yc) = obj_country['offset']
                ttoolkit.TToolkit.surface.blit( obj_country['image'], (x+xc+self._origin[0],y+yc+self._origin[1]) )
                image = obj_country['image']
                rect =  image.get_rect()
                obj_country['rect'] = ( ( x + xc + self._origin[0], y + yc + self._origin[1] ),
                                                    ( rect[2], rect[3] )
                                                    )
                list_of_countries.append( obj_country )


    def _updateArmies( self, list = None ):
        """ Update the armies in the countries. If list == None then update all the countries """
        if list == None:
            list = model.Model.getInstance().getCountries()

        for country in list:
            (xa,ya)= country['army_offset']
            ((xc1,yc1),(xc2,yc2)) = country['rect']
            center_x = (xc2/2) + xc1 + xa / 3
            center_y = (yc2/2) + yc1 + ya / 3
            pygame.draw.circle( ttoolkit.TToolkit.surface, ( pygame.color.Color('white') ), (center_x,center_y), 6, 0)
            pygame.draw.circle( ttoolkit.TToolkit.surface, ( pygame.color.Color('black') ), (center_x,center_y), 7, 1)


    def notify( self, event ):
        pass

    def update( self ):
        if self._dirty:
            ttoolkit.TToolkit.surface.blit( self._theme['map']['image'], self._origin)
            self._updateContinents()
            self._updateArmies()
            self._dirty = False
