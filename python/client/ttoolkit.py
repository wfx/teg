## $Id: ttoolkit.py,v 1.4 2003/09/09 21:32:32 riq Exp $
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

## A simple Toolkit for TEG using pygame, the TToolkit :-)

import pygame

# TEG imports
import font
from events import *

class TToolkitException( Exception ):
    pass


class TDisplay( object ):
    def __init__( self, surface = None ):
        TToolkit.surface = surface 
        self._listTk = []

    def addWidget( self, tk ):
        if not isinstance( tk, TToolkit ):
            raise TToolkitException('TDisplay.add() accepts only TToolkit objects: %s' % tk )

        self._listTk.append( tk )
        if tk.isDirty():
            tk.update()

    def remove( self, tk ):
        if tk in self._listTk:
            self._listTk.remove( tk )
        else:
            raise TToolkitException('Object is not in TDisplay')

    def update( self ):
        pygame.display.flip()

    def notify( self, event ):
        for i in self._listTk:
            i.notify( event )

    def setEventMgr( self, mgr ):
        pass

class TToolkit( object ):

    surface = None

    # static methods
    def setSurface( aSurface ):
        TToolkit.surface = aSurface
    setSurface = staticmethod( setSurface )

    def __init__( self ):
        self._size = (0,0)
        self._origin = (0,0)
        self._dirty = True
        self._update = False
        self._mouseOver = False
        self._mousePressed = False 

        if TToolkit.surface == None:
            raise TToolkitException('No surface given!')

    def isDirty( self ):
        return self._dirty

    def setSize( self, size ):
        self._size = size
        self._dirty = True

    def getSize( self ):
        return self._size

    def getOrigin( self ):
        return self._origin

    def setOrigin( self, origin ):
        self._origin = origin
        self._dirty = True

    def update( self ):
        """ re-draws the widget """
        pass

    def notify( self, event ):
        """ default manager for MouseMotionEvent """

        if isinstance( event, MouseButtonDown ):
            if not self._mousePressed:
                self._mousePressed = True
                self._dirty = True

        elif isinstance( event, MouseButtonUp ):
            if self._mousePressed:
                self._mousePressed = False
                self._dirty = True

        elif isinstance( event, MouseMotionEvent ):
            pos = event.getPos()
            if pos[0] >= self._origin[0] and pos[0] <= self._origin[0] + self._size[0] and \
                pos[1] >= self._origin[1] and pos[1] <= self._origin[1] + self._size[1]:
                if not self._mouseOver:
                    self._mouseOver = True
                    self._dirty = True
            else:
                if self._mouseOver:
                    self._mouseOver = False
                    self._dirty = True

        if self._dirty:
            self.update()


class TLayout( TToolkit ):
    def __init__( self ):
        TToolkit.__init__( self )
        self._spacing = 5

    def setSpacing( self, spacing ):
        self._spacing = spacing

    def getSpacing( self ):
        return self._spacing


class TBoxLayout( TLayout ):
    LeftToRight= 0
    RightToLeft = 1
    TopToBottom = 2
    BottomToTop = 3

    def __init__( self ):
        TLayout.__init__( self )
        self._widgets = []
        self._direction = TBoxLayout.LeftToRight

    # public methods
    def setOrigin( self, origin ):
        TLayout.setOrigin( self, origin )
        self._calculateChildsOrigin()

    def addWidget( self, widget ):
        if not isinstance( widget, TToolkit ):
            raise TToolkitException("TBoxLayout only accepts TToolkit objects")

        self._widgets.append( widget )
        self._updateWidgetOrigin( widget )

    def setDirection( self, direction ):
        self._direction = direction

    def update( self ):
        for w in self._widgets:
            w.update()

    def notify( self, event ):
        for w in self._widgets:
            w.notify( event )


class THBoxLayout( TBoxLayout ):
    """ TToolkit Horizontal Box Layout """
    def __init__( self ):
        TBoxLayout.__init__( self )
        self.setDirection( TBoxLayout.LeftToRight )

    # protected methods
    def _calculateChildsOrigin( self ):
        pass

    def _updateWidgetOrigin( self, widget ):
        widget.setOrigin( (self._origin[0] + self._size[0], self._origin[1] ) )

        y = self._size[1]
        if self._size[1] < widget.getSize()[1]:
            y = widget.getSize()[1]
        self._size = ( self._size[0] + widget.getSize()[0], y )

class TVBoxLayout( TBoxLayout ):
    """ TToolkit Vertical Box Layout """
    def __init__( self ):
        TBoxLayout.__init__( self )
        self.setDirection( TBoxLayout.TopToBottom )

    def _calculateChildsOrigin( self ):
        pass

    def _updateWidgetOrigin( self, widget ):
        widget.setOrigin( (self._origin[0], self._origin[1] + self._size[1] ) )

        x = self._size[0]
        if self._size[0] < widget.getSize()[0]:
            x = widget.getSize()[0]
        self._size = ( x, self._size[1] + widget.getSize()[1] )

class TButton( TToolkit ):

    buttonFont = font.Fonts.getInstance().get('helvetica 16')

    def __init__( self, label = 'NO LABEL', icon = None ):
        TToolkit.__init__( self )
        self._label = label
        self.__label_size = (0,0)
        self._icon = icon 
        self.__icon_size = (0,0)
        self._recalculateSize()

    # public
    def setLabel( self, label ):
        """ Sets the label of a button. """
        self._label = label
        self._recalculateSize()
        self._dirty = True

    def getLabel( self, label ):
        """ Returns the label of a button. """
        return self._label

    def setIcon( self, icon ):
        self._icon = icon
        self._recalculateSize()
        self._dirty = True
        
    def update( self ):
        """ Redraw the button only if it is necesary. """
        if self._dirty:

            offset = 0
            color = pygame.color.Color('white')
            if self._mouseOver:
#                color = pygame.color.Color('grey')
                color = (240,240,240,255)
                if self._mousePressed:
                    offset = 2
            rect = pygame.Rect( self._origin[0] + offset, self._origin[1] + offset, self._size[0] - offset, self._size[1] -offset )
            pygame.draw.rect( TToolkit.surface, color, rect, 0)

            if self._mouseOver and self._mousePressed:
                rect = pygame.Rect( self._origin[0] + 2 , self._origin[1] + 2, self._size[0] -4, self._size[1] -4 )
                pygame.draw.rect( self.surface, (192,192,192,255), rect, 2)
                
            if self._icon:
                x = (self._size[0] - self.__icon_size[0]) /2
                TToolkit.surface.blit( self._icon, (self._origin[0] + offset + x , self._origin[1] + 5 + offset ) )

            if self._label:
                x = (self._size[0] - self.__label_size[0]) /2
                render = TButton.buttonFont.render( self._label, 1, (0,0,0))
                TToolkit.surface.blit( render, (self._origin[0] + offset + x, self._origin[1] + 5 + offset + self.__icon_size[1] ) )
            self._update = True
            self._dirty = False

    # protected
    def _recalculateSize( self ):
        spacing_x = 10
        spacing_y = 10
        self.__label_size = (0,0)
        self.__icon_size = (0,0)
        if self._icon:
            self.__icon_size = self._icon.get_rect()[2:]

        self.__label_size = TButton.buttonFont.size( self._label )
        if self.__icon_size[0] > self.__label_size[0]:
            size_x = self.__icon_size[0]
        else:
            size_x = self.__label_size[0]
        size_y = self.__icon_size[1] + self.__label_size[1]
        self._size = (size_x + spacing_x, size_y + spacing_y )


class TToolbar( THBoxLayout ):

    Border = 2
    def addSeparator( self ):
        separator = TSeparator()
        self.addWidget(separator)

    def addWidget( self, widget ):
        THBoxLayout.addWidget( self, widget )
        for w in self._widgets:
            if isinstance(w, TSeparator):
                s = w.getSize()
                w.setSize( (s[0], self._size[1]) )

class TSeparator( TToolkit ):
    """ A Separator, useful in Toolbars """
    XSize = 20
    def __init__( self ):
        TToolkit.__init__( self )
        self.setSize( (TSeparator.XSize,0) )

    def update( self ):
        if self._dirty:
            color = pygame.color.Color('white')
            rect = pygame.Rect( self._origin[0], self._origin[1], self._size[0], self._size[1] )
            pygame.draw.rect( TToolkit.surface, color, rect, 0)

            yy = self._size[1] / 2
            y = yy - yy /2
            x = self._size[0] / 2 - 1
            rect = pygame.Rect( self._origin[0] + x, self._origin[1]+y, 4, yy )
            pygame.draw.rect( TToolkit.surface, (192,192,192,255), rect, 2)

            self._dirty = False


class TWindow( TToolkit ):

    titleFont = font.Fonts.getInstance().get('helvetica 10')
    titleY = 25
    titleColor = (0,192,204,255)
    windowColor = (224,240,240,255)

    def __init__( self, title = '' ):
        TToolkit.__init__(self)
        self._title = title
        self.__title_size = (0,0)
        self._calcTitleSize()

    def setTitle( self, title ):
        self._title = title

    def getTitle( self ):
        return self._title

    def update( self ):
        if self._dirty:
            self._dirty = True

            rect = pygame.Rect( self._origin[0], self._origin[1], self._size[0], TWindow.titleY  )
            pygame.draw.rect( TToolkit.surface, TWindow.titleColor, rect, 0)

            render = TButton.buttonFont.render( self._title, 1, (0,0,0))
            x_offset = (self._size[0] - self.__title_size[0]) / 2
            TToolkit.surface.blit( render, (self._origin[0] + x_offset, self._origin[1] + 3 ) )
   
            rect = pygame.Rect( self._origin[0], self._origin[1] + TWindow.titleY, self._size[0], self._size[1] - TWindow.titleY )
            pygame.draw.rect( TToolkit.surface, TWindow.windowColor, rect, 0)

    def _calcTitleSize( self ):
        self.__title_size = TButton.buttonFont.size( self._title )
