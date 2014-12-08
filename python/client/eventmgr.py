## $Id: eventmgr.py,v 1.2 2003/08/18 20:42:59 riq Exp $
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


#
# class EventMgr based on sjbrown (http://sjbrown.ezide.com/writing-games.html)
#

import pygame
from weakref import WeakKeyDictionary

# TEG imports
from events import *

class EventMgr( object ):
    def __init__(self ):
        self.preControllers = []
        self.controllers = []
        self.listeners = []
        self.postListeners = []

        self._quit = 0
        self.eventQueue = []

    def registerController( self, controller ):
        # controller should be a subclass of controllers.Controller
        controller.setEventMgr( self )
        self.controllers.append( controller )

    def registerListener( self, listener ):
        # listener should be a subclass of listeners.Listener
        listener.setEventMgr( self )
        self.listeners.append( listener )

    def registerPostListener( self, postListener ):
        self.postListeners.append( postListener )

    def registerPreController( self, preController ):
        self.preControllers.append( preController )
        
    def post( self, event ):
        self.eventQueue.append( event )

        if isinstance(event, QuitEvent): 
            self._quit = 1

    def loop( self ):
        while( not self._quit ):

            for preController in self.preControllers:
                preController.update()

            for ctrl in self.controllers:
                self.eventQueue = self.eventQueue + ctrl.getEvents()

            # copy events, since listener might 'post' some events and the 
            # controllers might not be called.
            from copy import copy
            evts = copy( self.eventQueue )
            self.eventQueue = []

            while len(evts) > 0: 
                ev = evts.pop(0)
                for ob in self.listeners:
                    ob.notify( ev )

                # some listener might handle the quit event.
                if isinstance(ev, QuitEvent): 
                    self._quit = 1

            for postListener in self.postListeners:
                postListener.update()
