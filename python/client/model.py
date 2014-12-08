## $Id: model.py,v 1.1 2003/08/19 04:01:57 riq Exp $
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

# The model

import theme 

class Model( object ):

    model_instance = None

    # PLAYER_STATUS
    PLAYER_STATUS_DISCONNECTED  = 0
    PLAYER_STATUS_CONNECTED     = 1
    PLAYER_STATUS_GAMEOVER      = 2     
    PLAYER_STATUS_ENABLED       = 3
    PLAYER_STATUS_START         = 4
    PLAYER_STATUS_ARMIES1       = 5
    PLAYER_STATUS_POSTARMIES1   = 5
    PLAYER_STATUS_ARMIES2       = 6
    PLAYER_STATUS_POSTARMIES2   = 7
    PLAYER_STATUS_IDLE          = 8 
    PLAYER_STATUS_ARMIESC       = 9
    PLAYER_STATUS_EXCHANGE      = 10
    PLAYER_STATUS_POSTARMIESC   = 11
    PLAYER_STATUS_PACT          = 12
    PLAYER_STATUS_TURNSTART     = 13
    PLAYER_STATUS_ATTACK        = 14
    PLAYER_STATUS_TROOPS        = 15
    PLAYER_STATUS_REGROUP       = 16
    PLAYER_STATUS_CARD          = 17
    PLAYER_STATUS_ENDTURN       = 18
#   unreachable state
    PLAYER_STATUS_LAST          = 19

    def __init__( self ):
        self._countries = []
        self._players = []
        self._myname = 'noname'
        self._mycolor = 'nocolor'
        self._status = 0
        self._peerServer = None
        self._conn = None

        self._theme = theme.Theme()
        self._theme.parseTheme()


    def getInstance():
        if Model.model_instance == None:
            Model.model_instance = Model()
        return Model.model_instance
    getInstance = staticmethod( getInstance )
        
    def setName( self, name ):
        """ Sets my name """
        self._myname = name

    def getName( self ):
        return self._myname

    def getCountries( self ):
        return self._countries

    def getPlayers( self ):
        return self._players

    def getStatus( self ):
        return self._status

    def getTheme( self ):
        return self._theme
