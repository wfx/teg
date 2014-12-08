## $Id: gui.py,v 1.4 2003/09/09 21:32:32 riq Exp $
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
from listeners import *
from controllers import *
from events import *
import model
import map
import toolbar
from ttoolkit import *
import eventmgr
import theme

class GUI( object ):
    def __init__( self ):

        # default size
        self.surface = pygame.display.set_mode( (800,600) )
        pygame.display.set_caption('Tenes Empanadas Graciela')

        # call this before initializing SDL
        self._model = model.Model.getInstance()
        self._display = None

    def initScreen( self ):
#        try:
#            self.surface = pygame.display.set_mode( self._theme['screen']['size'] )
#        except Exception:
#            pass

        # Fill background
        self._background = pygame.Surface(self.surface.get_size())
        self._background = self._background.convert()
        self._background.fill((255, 255, 255))

        self._display = TDisplay( self.surface )
        vbox = TVBoxLayout()
        tb = TToolbar()
        button1 = TButton('Connect', theme.Theme.loadImage('icon_connect.png'))
        button2 = TButton('Start', theme.Theme.loadImage('icon_connect.png'))
        button3 = TButton('Send armies', theme.Theme.loadImage('icon_connect.png'))
        button4 = TButton('1. Regroup', theme.Theme.loadImage('icon_connect.png'))
        button5 = TButton('2. Get card', theme.Theme.loadImage('icon_connect.png'))
        button6 = TButton('3. End turn', theme.Theme.loadImage('icon_connect.png'))
        button7 = TButton('View players', theme.Theme.loadImage('icon_connect.png'))
        tb.addWidget( button1 )
        tb.addWidget( button2 )
        tb.addSeparator()
        tb.addWidget( button3 )
        tb.addWidget( button4 )
        tb.addWidget( button5 )
        tb.addWidget( button6 )
        tb.addSeparator()
        tb.addWidget( button7 )
        theMap = map.Map( self._theme )
        theToolbar = toolbar.Toolbar( self._theme['toolbar'] )
        vbox.addWidget( tb )
        vbox.addWidget( theMap )
        vbox.addWidget( theToolbar )


        self.surface = pygame.display.set_mode( vbox.getSize() )

        self._display.addWidget( vbox )
        self._display.update()


        w = TWindow('Connect to server')
        w.setSize((300,300))
        w.setOrigin((120,120))
        w.update()


    def mainLoop( self ):

        ## XXX: This should be moved to TDisplay
        self._eventMgr = eventmgr.EventMgr()
        # controllers
        self._eventMgr.registerPreController( FPSController() )
        self._eventMgr.registerController( PygameController() )

        # listeners 
        self._eventMgr.registerListener( self._display )

        # post listener
        self._eventMgr.registerPostListener( self._display )

        self._eventMgr.loop()

    def setTheme( self, theme ):
        self._theme = theme 
