# $Id: twidget.py,v 1.1 2003/09/14 02:01:14 riq Exp $
###############################################################################################
# $Civil: widget.py,v 1.5 2003/06/29 09:54:52 chakie Exp $
# This file belongs to http://civil.sf.net
# License: GNU GPL v2
# Copyright: Civil team
###############################################################################################


import pygame

# define two additional events
MOUSEENTEREVENT = 100
MOUSELEAVEEVENT = 101
MOUSEBUTTONDOWN = pygame.MOUSEBUTTONDOWN
MOUSEBUTTONUP   = pygame.MOUSEBUTTONUP
MOUSEMOTION     = pygame.MOUSEMOTION
KEYUP           = pygame.KEYUP
KEYDOWN         = pygame.KEYDOWN
QUIT            = pygame.QUIT
TIMER           = pygame.USEREVENT + 2

# define a few constants we need
HANDLED   = 0
UNHANDLED = 1
DONE      = 2

class TWidget( object ):
    """
    This class defines a baseclass for all GUI widgets. It contains some common operations such as
    size handling and paiting the widget. In itself this class is not useful, and should be
    subclassed by more specialized classes.

    Subclasses should provide a SDL surface (self.surface) that can be blitted out when painting the
    widget as well as a map of the events  the widget is interested in (self.events). The map
    is checked every time an event happens within the widget, and the assigned callback is called if
    one such callback is registered. By default no widget has any callbacks registered.
    """

    def __init__ (self, callbacks = None, position = (0,0) ):
        """Initializes the widget. Subclasses are required to set the member self.surface to
        something valid. This is the visible part of the widget."""

        # store the position
        self.position = position

        # no surface yet
        self.surface = None

        # set the default callbacks
        self.callbacks = callbacks

        # set default internal callbacks. Those widgets who have internal callbacks should set them
        # manually 
        self.internal = None
        
        # we're dirty by default
        self.dirty = 1

        # we're also visible by default
        self.visible = 1
        

    def getWidth (self):
        "Returns the width of the widget."
        return self.surface.get_width ()


    def getHeight (self):
        "Returns the height of the widget."
        return self.surface.get_height ()


    def getX (self):
        "Returns the x-position of the widget."
        return self.position [0]

 
    def getY (self):
        "Returns the y-position of the widget."
        return self.position [1]


    def getPosition (self):
        "Returns a position containing the x- and y-position of the widget."
        return self.position


    def setPosition (self, position):
        """Set a new position for the widget. The passed position must be a tuple ontaining the new x-
        and y-position of the widget."""
        self.position = position

        # we're dirty now
        self.dirty = 1


    def getGeometry (self):
        """Returns the geometry of the widget. This is a tuple containing the x, y, width and height
        of the widget"""
        return ( self.position [0], self.position [1],
                 self.surface.get_width (), self.surface.get_height () )


    def setVisible (self, visible=1):
        """Sets the widget as visible if 'visible' is 1 and hidden if it is 0."""
        self.visible = visible


    def isVisible (self):
        """Returns 1 if the widget is visible and 0 otherwise."""
        return self.visible
    

    def isInside (self,position):
        """Checks wether the passed point is inside the widget. Returns 1 if inside and 0 if
        outside. A point on the border of the widget is considered to be inside."""
        ourx = self.position [0]
        oury = self.position [1]
        width = self.surface.get_width ()
        height = self.surface.get_height ()
        x = position[0]
        y = position[1]
        
        # is it inside?
        if ourx <= x and x <= ourx + width and oury <= y and y <= oury + height:
            # it's inside
            return 1

        # not inside
        return 0


    def paint (self, destination, force=0):
        """Method that paints the widget. This method will simply blit out the surface of the widget
        onto the destination surface. Override if custom painting is needed."""
        # are we dirty or not?
        if not self.dirty and not force:
            # not dirty, nothing to do here
            return 0

        # visible?
        if not self.visible:
            # not visible, nothing to do here
            return 0
            
        # we're dirty, blit out our full surface to the main surface
        destination.blit ( self.surface, (self.position [0], self.position [1] ) )

        self.dirty = 0

        # we did something, make sure the widget manager knows that
        return 1
   

    def handle (self, type, event):
        """Handles the passed event if a callback has been registered. If no callback for the event
        can be found then this method just returns."""
        # should it be handled internally?
        if self.internal and self.internal.has_key ( type ) and self.internal[type] != None:
            # yep, so execute it, but we don't care about any return value
            code = self.internal[type] ( event )

            # did we get a code at all?
            if code != None:
                # ah, nope, so return a default answer
                return code
        
        # do we have an external handler for that type?
        if self.callbacks and self.callbacks.has_key ( type ) and self.callbacks[type] != None:
            # yep, so execute it and return whatever it returns
            code = self.callbacks[type] ( self, event )

            # did we get a code at all?
            if code == None:
                # ah, nope, so return a default answer
                return HANDLED
            
            # we got a good answer, return it
            return code
        
        # the widget is not interested in the eent
        return UNHANDLED

    

#  Local Variables:
#  mode: auto-fill
#  fill-column: 100
#  End:
