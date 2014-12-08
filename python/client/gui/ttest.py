###############################################################################################
# $Id: ttest.py,v 1.1 2003/09/14 04:31:39 riq Exp $
###############################################################################################

import pygame
from pygame.locals import *
import twidget
from tbutton          import TButton
from twidget_manager  import TWidgetManager
from tdialog          import *
import theme

class TTest(TDialog):

    def __init__ (self):
        "Creates the dialog."
        # init superclass
        TDialog.__init__ (self, TWidgetManager.sdl_surface)


    def createWidgets (self):
        "Creates all widgets for the dialog."

        # create the cancel button
        self.wm.register ( TButton ( 'Connect', theme.Theme.loadImage('icon_connect.png'), {twidget.MOUSEBUTTONUP : self.ok } ) )



    def ok (self, trigger, event):
        """Callback triggered when the user clicks the 'Ok' button. Simply closes the dialog and
        returns to the main dialog."""

        # we're cancelling the dialog
        self.state = ACCEPTED
        
        return twidget.DONE



#  Local Variables:
#  mode: auto-fill
#  fill-column: 100
#  End:
