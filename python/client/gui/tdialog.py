# $Id: tdialog.py,v 1.1 2003/09/14 02:01:14 riq Exp $
###############################################################################################
# $Civil: dialog.py,v 1.8 2003/06/29 09:54:52 chakie Exp $
# This file belongs to http://civil.sf.net
# License: GNU GPL v2
# Copyright: Civil team
###############################################################################################

import sys
import pygame
from pygame.locals import *

import twidget
from twidget import *
from twidget_manager import TWidgetManager

# return codes used to indicate what should happen
ACCEPTED = 0
REJECTED = 1

class TDialog(TWidget):
    """
    This class is used as a baseclass for dialogs. A dialog is a window with widgets that occupies
    the whole or part of the screen. Currently only whole screen dialogs are supported. This class
    contains the main widget manager to which widgets are added. Subclasses could override the
    method createWidgets() to create their own widgets, but that is not mandatory.

    It also contains the main event loop which is started using the method run(). The event loop
    handles events from all registered widgets and calls their callbacks.

    A dialog can have a background. It is set using setBackground() and can be either painted once
    or tiled over the entire screen.
    """

    # define a shared cache
    cache = {}

    # a normal and waiting cursor
    normalcursor = None
    waitcursor   = None

    def __init__ (self, surface):
        "Creates the dialog."
        # call superclass
        TWidget.__init__ (self)
        
        # create the widget manager so that it manages our canvas and then paints out its stuff on
        # the root window
        self.wm = TWidgetManager (surface, self)

        # register ourselves as a paintable object to the widget manager. This should insure that we
        # get repainted too
        self.wm.register ( self )
        
        # create all widgets
        self.createWidgets ()

        # flags used for indicating what we've done
        self.state = REJECTED

        # no background
        self.background = None
        self.tile = 0

        # load the cursors if we don't already have 'em
#        if TDialog.normalcursor == None:
#            # no such cursor, do it
#            normaldata = properties.setup_cursor_normal_data
#            normalmask = properties.setup_cursor_normal_mask
#            waitdata   = properties.setup_cursor_wait_data
#            waitmask   = properties.setup_cursor_wait_mask
#
#            # now load them
#            TDialog.waitcursor   = pygame.cursors.load_xbm ( waitdata,  waitmask)
#            TDialog.normalcursor = pygame.cursors.load_xbm ( normaldata, normalmask )

        # by default we set the normal cursor
#        self.setNormalCursor ()
        
        
    def createWidgets (self):
        "Creates all widgets for the dialog. Override in subclasses."
        pass


    def setBackground (self, filename, tile=1):
        """Sets a background for the entire dialog. The background is loaded from the passed
        'filename'. It is painted either only once or tiled, depending on the setting for 'tile'. A
        value of 1 will tile the image, any other value will draw the background once centered."""

        # do we have the wanted image in our cache?
        if TDialog.cache.has_key ( filename ):
            # yep, use that instead
            self.background = TDialog.cache [filename]
            
        else:
            # not in memory, try to load the surfaces
            self.background = pygame.image.load ( filename ).convert ()

            # store in the cache for later use
            TDialog.cache [filename] = self.background

        # store the tiling value
        self.tile = tile

        # we're dirty now
        self.dirty = 1
        

    def getWidth (self):
        "Returns the width of the dialog."
        return 0


    def getHeight (self):
        "Returns the height of the widget."
        return 0


    def getGeometry (self):
        """Returns the geometry of the widget. This is a tuple containing the x, y, width and height
        of the dialog. Not currently meaningful."""
        return ( self.position [0], self.position [1], 0, 0 )


    def isInside (self,position):
        """Checks wether the passed point is inside the dialog. Returns 1 if inside and 0 if
        outside. A point on the border of the widget is considered to be inside. Currently always
        returns 0."""
        # not inside
        return 0


    def paint (self, destination, force=0):
        """Method that paints the dialog. This method will simply blit out the background if one has
        been set. Override if custom painting is needed."""
        # are we dirty or not?
        if self.background == None or ( self.dirty == 0 and force == 0 ):
            # not dirty, or no background nothing to do here
            return 0

        # get the dimensions of the background
        width, height = self.background.get_size () 
        
        # should we tile or not?
        if self.tile:
            # perform tiling of the background
            for y in range ( TWidgetManager.sdl_surface.getHeight () / height + 1 ):
                for x in range ( TWidgetManager.sdl_surface.getWidth () / width + 1 ):
                    # what height should be used?
                    if y * height > TWidgetManager.sdl_surface.getHeight ():
                        # use only the missing part
                        heighty = (( y + 1 ) * height) - TWidgetManager.sdl_surface.getHeight ()

                    else:
                        # full height of icon
                        heighty = height

                    # what width should be used?
                    if x * width > TWidgetManager.sdl_surface.getWidth ():
                        # use only the missing part
                        widthx = (( x + 1 ) * width) - TWidgetManager.sdl_surface.getWidth ()

                    else:
                        # full width of icon
                        widthx = width

                        # blit it all out
                        destination.blit ( self.background, (x * width, y * height ) )
                        
        else:
            # no tiling, just blurt it out once
            destination.blit ( self.background, self.position )

        self.dirty = 0
        
        # we did something, make sure the widget manager knows that
        return 1



    def run (self):
        """Executes the dialog and runs its internal loop until a callback returns widget.DONE. When
        that dialog is terminated this method also returns.""" 
        
        # loop forever
        while 1:
            # repaint the stuff if needed
            self.wm.paint ()
            
            # get next event
            event = pygame.event.wait()

            # see wether the widget manager wants to handle it
            if event != -1:
                # handle event and get the return code that tells us wehter it was handled or not
                returncode = self.wm.handle ( event )

                # is the event loop done?
                if returncode == twidget.DONE:
                    # disable the timer
                    self.disableTimer ()

                    return self.state

             
    def quit (self):
        """Callback triggered when the player wants to quit. This can be overridden to provide
        custom quitting behaviour. The default just simply exits."""
        print "quitting..."
        
        # terminate pygame
        pygame.quit ()

        # and go away
        sys.exit ( 0 )

        
    def accept (self):
        """Accepts the dialog. Will close it and return from it's event loop with the return status
        'dialog.ACCEPTED'.""" 
        # we're accepting the dialog
        self.state = ACCEPTED

        return twidget.DONE


    def reject (self, trigger, event):
        """Accepts the dialog. Will close it and return from it's event loop with the return status
        'dialog.REJECTED'."""
        # we're cancelling the dialog
        self.state = REJECTED
        
        return twidget.DONE


    def messagebox (self, message):
        import messagebox

        # failed to init network connection, show a messagebox
        messagebox.Messagebox ( message )

        # repaint the stuff if needed
        self.wm.paint (force=1, clear=1)


    def enableTimer (self, ms):
        """Enables timer events. Calling this method will make the method timer() get called every
        'ms' milliseconds. Call disableTimer() to disable the timer."""

        #scenario.dispatcher.registerTimerCallback ( ms, self.timer, self )
        
        # just call the method, make no checks
        pygame.time.set_timer ( TIMER, ms )
            

    def disableTimer (self):
        """Disables timer events."""
        # just call the method, make no checks
        #scenario.dispatcher.deregisterTimerCallback ( self )
        
        pygame.time.set_timer ( TIMER, 0 )

        # remove all old stale events that may have been left in the queue
        pygame.event.get ( [TIMER] )
   

    def timer (self):
        """Callback triggered when the dialog has enabled timers and a timer fires. This should be
        overridden by subclasses to provide the needed code."""
        # by default we're handled
        return twidget.HANDLED


#    def setNormalCursor (self):
#        """Sets the normal cursor for the setup dialogs. This is a cursor that should be used when
#        the player is not waiting for something, ie. when he/she is allowed to do something. This
#        cursor is always set by the constructor."""
#        # just set it
#        pygame.mouse.set_cursor ( *Dialog.normalcursor )
#
#
#    def setWaitCursor (self):
#        """Enables a waiting cursor. This should be used when the game is doing something and can
#        not respond to player actions."""
#        # just set it
#        pygame.mouse.set_cursor ( *Dialog.waitcursor )

        
#  Local Variables:
#  mode: auto-fill
#  fill-column: 100
#  End:
