## $Id: tlayout.py,v 1.2 2004/02/08 20:49:08 riq Exp $
##
## Tenes Empanadas Graciela
##
## Copyright (C) 2000,2003 Ricardo Quesada
##
## Author: Ricardo Calixto Quesada <riq@coresecurity.com>

from twidget    import *

class TLayout( TWidget ):

    def __init__ (self):
        "Creates the layout."
        # call superclass
        TWidget.__init__ (self)
        
        
class TBoxLayout( TLayout ):
    LeftToRight= 0
    RightToLeft = 1
    TopToBottom = 2
    BottomToTop = 3

    def __init__( self, surface ):
        TLayout.__init__( self )
        self._direction = TBoxLayout.LeftToRight

        # create the widget manager so that it manages our canvas and then paints out its stuff on
        # the root window
        self.wm = TWidgetManager (surface, self)

        # register ourselves as a paintable object to the widget manager. This should insure that we
        # get repainted too
        self.wm.register ( self )

    # public methods
    def setOrigin( self, origin ):
        TLayout.setOrigin( self, origin )
        self._calculateChildsOrigin()


    def addWidget( self, widget ):
        """Adds a widget to the container."""
        self.wm.register( widget )
        self.dirty = 1


    def setDirection( self, direction ):
        self._direction = direction
        self.dirty = 1


    def handle( self, type, event ):
        """Handle the events of the contained widgets and propagate the event."""
        self.wm.handle( event )


    def paint (self, destination, force=0):
        """Paints the inner widgets."""


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

