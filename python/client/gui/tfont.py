#   $Id: tfont.py,v 1.2 2003/09/14 04:31:39 riq Exp $
#
# Tenes Empanadas Graciela
# Copyright 2000,2003 Ricardo Quesada (riq@coresecurity.com)
#

import pygame

if not pygame.font.get_init():
    pygame.font.init()

TFont = {
    'helvetica 8'   :  pygame.font.SysFont('helvetica',8),
    'helvetica 10'  :  pygame.font.SysFont('helvetica',10),
    'helvetica 12'  :  pygame.font.SysFont('helvetica',12),
    'helvetica 16'  :  pygame.font.SysFont('helvetica',16,0),
    'helvetica 16b' :  pygame.font.SysFont('helvetica',16,1),
    'helvetica 20'  :  pygame.font.SysFont('helvetica',20,0),
    'helvetica 20b' :  pygame.font.SysFont('helvetica',20,1)
}
