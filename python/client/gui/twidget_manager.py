# $Id: twidget_manager.py,v 1.1 2003/09/14 02:01:14 riq Exp $
###############################################################################################
# $Civil: widget_manager.py,v 1.6 2003/06/29 09:54:52 chakie Exp $
# This file belongs to http://civil.sf.net
# License: GNU GPL v2
# Copyright: Civil team
###############################################################################################

import pygame
import pygame.display
from pygame.locals import *
from twidget import TWidget
import twidget

class TWidgetManager( object ):

    sdl_surface = None

    """
    This class defines a simple framework for managing widgets and their events. All created widgets
    should be added to a framework before they are of any use or can receive events. All SDL events
    should be sent to this framework for possible dispatching to widgets. If an event does not
    consern any widget the other parts of the application are free to do whatever they wish with the
    event.

    Every event should be sent to handle() as the raw SDL event as received from pygame.event.poll ()
    or pygame.event.wait().
    """

    def __init__ (self, surface, dialog=None):
        """Initializes the instance"""

        # clear the list of registered widgets
        self.widgets = []

        # no keyboard shortcuts yet
        self.shortcuts = {}
        
        # the previous mouse position, for enter/leave events
        self.last_pos = (-1, -1)

        # the surface widgets are painted on
        self.surface = surface

        # store the dialog too
        self.dialog = dialog
        

    def register (self, widget, shortcut=None):
        """Register a widget within the widget manager. The widget will after it has been registered
        be able to receive various events. If the shortcut is a valid Pygame key id (of of the K_*
        constants), then the shortcut will be stored. The shortcut is used for activating a left
        mouse press on the widget when the given shortcut key is pressed."""
        # is the widget already in the list?
        if not widget in self.widgets:
            # nope, add it
            self.widgets.append ( widget )

            # do we have a shortcut?
            if shortcut != None:
                # yeah, so add it
                self.shortcuts [shortcut] = widget
            

    def deRegister (self, widget):
        """Removes the passed widget from the management."""
        # do we have the widget in the list?
        if widget in self.widgets:
            # yep, remove it
            self.widgets.remove ( widget )

            # TODO: delete from shortcuts too


    def paint (self, force = 0, clear = 0):
        """Repaints the surface if needed."""
        paintneeded = 0

        # should we clear the disply first?
        if clear:
            # yep, do that first
            self.surface.fill ( (0,0,0) ) 
            
        # loop over all widgets
        for widget in self.widgets:
            # paint widget 
            if widget.paint (self.surface, force):
                # it did something, so set the flag
                paintneeded = 1
                
        # is a paint needed?
        if paintneeded:
            # update the whole screen
#            TWidgetManager.sdl_surface.update ()
            pygame.display.flip()
         

    def handle (self, event):
        """Handles the passed event. If the event is of any interest to any of the registred widgets
        the widgets are allowed to act on the event. If any widget handled the event then this
        method will return 1 to indicate that it should not be processed any further. A value of 0
        indicates that the event was not processed."""

        # what type of event do we have?
        if event.type == twidget.KEYDOWN:
            # keyboard key pressed 
            return self.handleKeyDown (event)
            
        elif event.type == twidget.KEYUP:
            # keyboard key released
            return self.handleKeyUp (event)

        elif event.type == twidget.MOUSEMOTION:
            # mouse moved 
            return self.handleMouseMotion (event)

        elif event.type == twidget.MOUSEBUTTONDOWN:
            # mouse button pressed
            return self.handleMousePressed (event)

        elif event.type == twidget.MOUSEBUTTONUP:
            # mouse button released 
            return self.handleMouseReleased (event)

        elif event.type == twidget.TIMER:
            # timer event
            return self.handleTimer (event)

        elif event.type == twidget.QUIT:
            # player wants to quit
            return self.handleQuit (event)
            
        # it was none of our events
        return 0
    

    def handleKeyDown (self, event):
        """Handles an event when a key was pressed but not yet released. Uses the last known
        position of the mouse to determine which widget has focus."""

        # by default we've not handled it
        handled = twidget.UNHANDLED

        # loop over all widgets in the list
        for w in self.widgets:
            # is the mouse inside the widget?
            if w.isInside ( self.last_pos ) and w.isVisible ():
                # have the widget handle the event
                handled = w.handle ( twidget.KEYDOWN, event )

        # was it handled?
        if handled != twidget.UNHANDLED:
            # it was handled somehow, go away
            return handled

        # not handled, so see if we have any keyboard shortcuts for itm get the key 
        key = event.key

        # any shortcut for it?
        if self.shortcuts.has_key ( key ):
            # yeah, so get the widget
            w = self.shortcuts[key]

            #execute create an left mouse click event that's inside the widget
            newevent = pygame.event.Event ( MOUSEBUTTONUP, pos=w.getPosition (), button=1 )

            return self.handleMouseReleased ( newevent )

        # no shortcut either, it was not handled
        return twidget.UNHANDLED
    

    def handleKeyUp (self, event):
        """Handles an event when a pressed key released. Uses the last known
        position of the mouse to determine which widget has focus."""
        # by default we've not handled it
        handled = twidget.UNHANDLED

        # loop over all widgets in the list
        for w in self.widgets:
            # is the mouse inside the widget?
            if w.isInside ( self.last_pos ) and w.isVisible ():
                # have the widget handle the event
                w.handle ( twidget.KEYUP, event )
                handled = twidget.HANDLED

        return handled
     

    def handleMouseMotion (self, event):
        """Handles an event when the mouse pointer was moved within a widget."""

        # get the position of the mouse
        position = event.pos

        # by default we've not handled it
        handled = twidget.UNHANDLED
        
        # loop over all widgets in the list
        for w in self.widgets:

            # is the mouse inside the widget?
            if w.isInside ( position ) and not w.isInside ( self.last_pos ) and w.isVisible ():
                # have the widget handle the event
                w.handle ( twidget.MOUSEENTEREVENT, event )
                handled = twidget.HANDLED

            # is the mouse outside the widget
            elif not w.isInside ( position ) and w.isInside ( self.last_pos ) and w.isVisible ():
                # have the widget handle the event
                w.handle ( twidget.MOUSELEAVEEVENT, event )
                handled = twidget.HANDLED

            # is the mouse the widget and the last move was too inside?
            elif w.isInside ( position ) and w.isInside ( self.last_pos ) and w.isVisible ():
                # have the widget handle the event
                w.handle ( twidget.MOUSEMOTION, event )
                handled = twidget.HANDLED

        # store new last position
        self.last_pos = position

        return handled
            

    def handleMousePressed (self, event):
        """Handles an event when a mouse key was pressed but not yet released."""

        # get the position of the press
        position = event.pos
       
        # loop over all widgets in the list
        for w in self.widgets:
            # is the mouse inside the widget?
            if w.isInside ( position ) and w.isVisible ():
                # have the widget handle the event
                status = w.handle ( twidget.MOUSEBUTTONDOWN, event )

                # did it handle it?
                if status != twidget.UNHANDLED:
                    # it handled it, we're done here
                    return status

        return twidget.UNHANDLED
       

    def handleMouseReleased (self, event):
        """Handles an event when a pressed mouse button was released."""

        # get the position of the press
        position = event.pos
        
        # loop over all widgets in the list
        for w in self.widgets:
            # is the mouse inside the widget?
            if w.isInside ( position ) and w.isVisible ():
                # have the widget handle the event
                status = w.handle ( twidget.MOUSEBUTTONUP, event )

                # did it handle it?
                if status != twidget.UNHANDLED:
                    # it handled it, we're done here
                    return status

        return twidget.UNHANDLED


    def handleTimer (self, event):
        """Handles a timer event."""

        # do we have a dialog?
        if self.dialog != None:
            # yep, call it
            return self.dialog.timer ()
         
        # it was not handled
        return twidget.UNHANDLED


    def handleQuit (self, event):
        """Handles a quit event. This is called when the player has done something that should cause
        the game to quit."""
        
        # do we have a dialog?
        if self.dialog != None:
            # yep, call it. this may never actually return if the dialog decides to quit
            return self.dialog.quit ()
         
        # it was not handled
        return twidget.UNHANDLED
       
#  Local Variables:
#  mode: auto-fill
#  fill-column: 100
#  End:
