## $Id: tegclient.py,v 1.6 2003/09/14 04:31:39 riq Exp $
##
## Tenes Empanadas Graciela
##
## Copyright (C) 2000,2003 Ricardo Quesada
##
## Author: Ricardo Calixto Quesada <riq@coresecurity.com>


# python modules
import pygame

# teg modules
import gui
from gui import twidget_manager
from gui import ttest
import theme

class TegClient( object ):
    def __init__( self ):

        pygame.init()

        theme.Theme()

        surface = pygame.display.set_mode( (800,600) )
        gui.twidget_manager.TWidgetManager.sdl_surface = surface


    def run( self ):
        gui.ttest.TTest().run()


if  __name__ == '__main__':
    aTeg = TegClient()
    aTeg.run()
