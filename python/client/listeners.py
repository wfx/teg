## $Id: listeners.py,v 1.1 2003/08/18 20:42:59 riq Exp $
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


class Listener( object ):

    def __init__( self ):
        self._eventMgr = None
        
    def notify( self, ev ):
        raise NotImplemented

    def update( self ):
        raise NotImplemented

    def setEventMgr( self, eventMgr ):
        self._eventMgr = eventMgr

    def post( self, event ):
        self._eventMgr.post( event )
