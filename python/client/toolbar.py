## $Id: toolbar.py,v 1.2 2003/09/08 03:29:55 riq Exp $
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

# TEG imports
import ttoolkit
from events import *
import model
import font

class Toolbar( ttoolkit.TToolkit ):

    ToolbarFont20 = font.Fonts.getInstance().get('helvetica 20')
    ToolbarFont16 = font.Fonts.getInstance().get('helvetica 16')

    def __init__(self, theme ):
        ttoolkit.TToolkit.__init__(self)
        self._theme = theme
        self.setOrigin( self._theme['offset'] )
        self._mousePos = (0,0)
        rect = theme['image'].get_rect()
        self.setSize( (rect[2],rect[3]) )

    def notify( self, event ):
        """ Called from EventMgr every time a new event is generated """
        if isinstance( event, MouseMotionEvent ):
            self._dirty = True
            self.update()
            self._mousePos = event.getPos()

    def update( self ):
        if self._dirty:
            self._updateToolbar()
            self._updateCountryNames()
            self._dirty = False

    def _updateToolbar( self ):
        ttoolkit.TToolkit.surface.blit( self._theme['image'], self._origin )

    def _updateCountryNames( self ):
        country = self._getCountryAt( self._mousePos )
        if country:
            render = Toolbar.ToolbarFont20.render( country['name'], 1, (0,0,0))
            self.surface.blit( render, (self._origin[0]+8, self._origin[1]+0) )
            render = Toolbar.ToolbarFont16.render( country['continent'], 1, (0,0,0))
            self.surface.blit( render, (self._origin[0]+8, self._origin[1]+18) )

    def _getCountryAt( self, coord ):
        # XXX: Improve this algorithm using:
        #  a) Load all the images, convert them to unique colors,
        #     test mouse over those images
        #  b) split the map in 4 parts, etc
        for c in model.Model.getInstance().getCountries():
            rect = c['rect']
#            print rect,coord
                # coord X
            if coord[0] > rect[0][0] and coord[0] < ( rect[0][0] + rect[1][0] ) and coord[1] > rect[0][1] and coord[1] < ( rect[0][1] + rect[1][1] ): 
                if not c['image'].get_at( ( coord[0] - rect[0][0], coord[1] - rect[0][1] ) ) == (0,0,0,0):
                    return c
