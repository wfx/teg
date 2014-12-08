## $Id: font.py,v 1.3 2003/09/07 21:40:15 riq Exp $
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

# XXX: should be sub-class of dict
class Fonts( object ):
    fonts_instance = None

    def __init__(self):
        if not pygame.font.get_init():
            pygame.font.init()

        self.fonts = {}
        self.fonts['helvetica 8'] =  pygame.font.SysFont('helvetica',8)
        self.fonts['helvetica 10'] =  pygame.font.SysFont('helvetica',10)
        self.fonts['helvetica 12'] =  pygame.font.SysFont('helvetica',12)
        self.fonts['helvetica 16'] =  pygame.font.SysFont('helvetica',16)
        self.fonts['helvetica 16b'] =  pygame.font.SysFont('helvetica',16,1)
        self.fonts['helvetica 20'] =  pygame.font.SysFont('helvetica',20,0)
        self.fonts['helvetica 20b'] =  pygame.font.SysFont('helvetica',20,1)


    def get( self, value ):
        return self.fonts[value]

    def getInstance():
        if Fonts.fonts_instance == None:
            Fonts.fonts_instance = Fonts()
        return Fonts.fonts_instance
    getInstance = staticmethod( getInstance )
