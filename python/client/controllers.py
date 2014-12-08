## $Id: controllers.py,v 1.4 2003/09/07 21:40:15 riq Exp $
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
from pygame.locals import *

from events import *

class Controller( object ):
    def __init__( self ):
        self._eventMgr = None

    def getEvents( self ):
        raise NotImplemented

    def update( self ):
        raise NotImplemented

    def setEventMgr( self, evMgr ):
        self._eventMgr = evMgr


class FPSController( Controller ):
    """Frames per seconds controller"""
    def update( self ):
        """No more thatn 20 fps"""
        pygame.time.Clock().tick(20)


class PygameController( Controller ):
    def getEvents( self ):

        evts = []

        for event in pygame.event.get():
            if event.type == QUIT:
                evts.append( QuitEvent() )

            elif event.type == MOUSEMOTION:
                evts.append( MouseMotionEvent( pygame.mouse.get_pos() ) )

            elif event.type == MOUSEBUTTONUP:
                evts.append( MouseButtonUp( pygame.mouse.get_pressed() ) )

            elif event.type == MOUSEBUTTONDOWN:
                evts.append( MouseButtonDown( pygame.mouse.get_pressed() ) )

        return evts
