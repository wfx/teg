# $Id: tbutton.py,v 1.1 2003/09/14 02:01:14 riq Exp $
###############################################################################################
# $Civil: button.py,v 1.3 2003/06/05 20:12:06 chakie Exp $
###############################################################################################

from twidget import TWidget
from tfont import TFont
import twidget
import pygame
from pygame.locals import *


class TButton(TWidget):
    """
    This class defines a simple pushbutton which can can be clicked. It needs a surfaces that
    represent the button. 

    This class plays a small sound when the button is clicked.
    """

    Spacing = (10,10)
    buttonFont = TFont.get('helvetica 16')


    def __init__ (self, label = '', icon = None, callbacks = None):
        """Initializes the widget. Loads the icon."""

        # firts call superclass constructor
        TWidget.__init__ (self, callbacks)

        # initialize label
        self._label = label
        self.__label_size = (0,0)

        # initialize icon
        self._icon = icon
        self.__icon_size = (0,0)

        # generate surfaces for label/icon
        self._generateSurfaces()
 
        # set our internal callbacks so that we can trap keys
        # use enter/leave up/down
        self.internal = {twidget.MOUSEENTEREVENT : self.mouseEnter,
                         twidget.MOUSELEAVEEVENT : self.mouseLeave,
                         twidget.MOUSEBUTTONUP   : self.mouseUp,
                         twidget.MOUSEBUTTONDOWN : self.mouseDown }
            


    def _calculateSize( self ):
        """Returns the size needed for the icon/label"""
        if self._icon:
            self.__icon_size = self._icon.get_rect()[2:]

        self.__label_size = TButton.buttonFont.size( self._label )
        if self.__icon_size[0] > self.__label_size[0]:
            size_x = self.__icon_size[0]
        else:
            size_x = self.__label_size[0]

        size_y = self.__icon_size[1] + self.__label_size[1]
        return (size_x + TButton.Spacing[0], size_y + TButton.Spacing[1] )

    def _generateSurfaces( self ):
        """Generates the surfaces for the button"""
        self.surface = pygame.Surface( self._calculateSize() )

        self._surface_outside = self._getSurfaceOutside()
        self._surface_inside = self._getSurfaceInside()
        self._surface_pressed = self._getSurfacePressed()
        self._surface_released = self._getSurfaceReleased()

        self.surface = self._surface_outside
    
    def mouseUp (self, event):
        """Internal callback triggered when the mouse is released when it is over a button. This
        sets a new icon for the button."""
        # set the new icon
        self.surface = self._surface_released

        # play a sound
#        scenario.audio.playSample ( 'button-clicked' )
        
        # we're dirty
        self.dirty = 1


    def mouseDown (self, event):
        """Internal callback triggered when the mouse is pressed when it is over a button. This
        sets a new icon for the button."""
        # set the new icon
        self.surface = self._surface_pressed

        # we're dirty
        self.dirty = 1


    def mouseEnter (self, event):
        """Internal callback triggered when the mouse enters a button. This sets a new icon for the
        button.""" 
        # set the new icon
        self.surface = self._surface_inside
        
        # we're dirty
        self.dirty = 1


    def mouseLeave (self, event):
        """Internal callback triggered when the mouse leaves a button. This sets a new icon for the
        button.""" 
        # set the new icon
        self.surface = self._surface_outside
        
        # we're dirty
        self.dirty = 1

    def __getSurface( self, color, offset = 0, drawBox = False ):

        surface = pygame.Surface( self._calculateSize() )

        rect = pygame.Rect( self.getX() + offset, self.getY() + offset, self.getWidth() - offset, self.getHeight() - offset )
        pygame.draw.rect( surface, color, rect, 0)

        if drawBox:
            rect = pygame.Rect( self.getX() + 2 , self.getY() + 2, self.getWidth() -4, self.getHeight() -4 )
            pygame.draw.rect( surface, (192,192,192,255), rect, 2)

        if self._icon:
            x = (self.getWidth() - self.__icon_size[0]) / 2
            surface.blit( self._icon, (self.getX() + x + offset , self.getY() + 5 + offset) )

        if self._label:
            x = (self.getWidth() - self.__label_size[0]) / 2
            render = TButton.buttonFont.render( self._label, 1, (0,0,0))
            surface.blit( render, (self.getX() + x + offset, self.getY() + 5 + self.__icon_size[1] + offset ) )

        return surface 

    def _getSurfaceOutside( self ):
        """Generates the surface when the mouse pointer is outside the surface"""
        return self.__getSurface( pygame.color.Color('white') )

    def _getSurfaceInside( self ):
        """Generates the surface when the mouse pointer is inside the surface"""
        return self.__getSurface( (240,240,240,255) )

    def _getSurfacePressed( self ):
        """Generates the surface when the mouse pointer is inside the surface"""
        return self.__getSurface( (240,240,240,255), 2, True )

    def _getSurfaceReleased( self ):
        """Generates the surface when the mouse pointer is inside the surface"""
        return self.__getSurface( (240,240,240,255), 0 )
