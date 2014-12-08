## $Id: connection.py,v 1.2 2003/08/19 04:01:57 riq Exp $
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


import socket
import select

# teg imports
import version

class Connection( object ):
    def __init__( self ):
        self._connected = False

    def connect( self, peer ):
        if not self._connected:
            self._sock = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
            self._sock.connect( peer )
            self._connect = True

    def disconnect( self ):
        self._sock.close()

    def send( self, data ):
        self._sock.send( '%s\n' % data  )

    def recv( self ):
        return self._sock.recv(5000)

    def hasData( self ):
        return select.select( [self._sock,],[],[], 0.000001 )

class Protocol( object ):

    # --- Protocol version -------------------------------
    PROTOCOL_HIVER      = 6
    PROTOCOL_LOVER      = 0

    # --- Tokens -----------------------------------------
    TOKEN_CVERSION      = "client_version"
    TOKEN_DISCON        = "disconnect"
    TOKEN_EXIT          = "exit"
    TOKEN_GAMEOVER      = "game_over"
    TOKEN_HELP          = "help"
    TOKEN_LOST          = "loser"
    TOKEN_MESSAGE       = "msg"
    TOKEN_PLAYERID      = "player_id"
    TOKEN_RECONNECT     = "reconnect"
    TOKEN_NEWPLAYER     = "newplayer"
    TOKEN_PVERSION      = "protocol_version"
    TOKEN_QUMM          = "quiero_un_mundo_mejor"
    TOKEN_REM           = "*"
    TOKEN_ROBOT         = "quiero_jugar_con_un_robot"
    TOKEN_SCORES        = "scores"
    TOKEN_STATS         = "stats"
    TOKEN_SERVERFULL    = "el_servidor_esta_lleno"
    TOKEN_GAMEINPROGRESS= "game_in_progress"
    TOKEN_SVERSION      = "server_version"
    TOKEN_ALONE         = "solo_a_ningun_lado"
    TOKEN_START         = "start"
    TOKEN_STATUS        = "status"
    TOKEN_TEST          = "eva_test"
    TOKEN_TURN          = "turno"
    TOKEN_WAIT          = "wait"
    TOKEN_WINNER        = "winner"
    TOKEN_ERROR         = "error"
    TOKEN_OK            = "ok"
    TOKEN_COUNTRIES     = "paises"
    TOKEN_ARMIES1       = "fichas"
    TOKEN_ARMIES2       = "fichas2"
    TOKEN_ARMIES3       = "fichas3"
    TOKEN_ARMIESC       = "fichasc"
    TOKEN_ATTACK        = "ataque"
    TOKEN_TROOPS        = "tropas"
    TOKEN_COUNTRY       = "pais"
    TOKEN_REGROUP       = "reagrupe"
    TOKEN_CARD          = "tarjeta"
    TOKEN_DICES         = "dados"
    TOKEN_EXCHANGE      = "canje"
    TOKEN_PLACE2ARMIES  = "ejer2"
    TOKEN_TYPEOFGAME    = "modalidad"
    TOKEN_MISSION       = "mission"
    TOKEN_SET           = "set"
    TOKEN_VIEW          = "view"
    TOKEN_COLOR         = "color"
    TOKEN_SAVE          = "save"
    TOKEN_SAYWHAT       = "que_tengo_que_hacer"
    TOKEN_ECHO          = "echo"
    TOKEN_GGZ           = "avoding_perdig_effect"
    TOKEN_PACT_REQUEST  = "pacto_request"
    TOKEN_PACT_REPLY    = "pacto_reply"
    TOKEN_SURRENDER     = "soy_cobarde"
    TOKEN_KICK          = "kick"
    TOKEN_ENUM_CARDS    = "enum_cards"
    TOKEN_NEW_ROUND     = "new_round"
    TOKEN_METASERVER    = "metaserver"

    # --- Options -----------------------------------------
    OPTION_CONQWORLD    = "conquer_world"
    OPTION_ARMIES       = "armies"
    OPTION_SEED         = "seed"
    OPTION_HELP         = "help"
    OPTION_CMISSION     = "common_mission"
    OPTION_FOG_OF_WAR   = "fog_of_war"

    functions = {
        TOKEN_PLAYERID : 'PlayerID',
        TOKEN_SVERSION: 'SVersion',
        TOKEN_PVERSION: 'PVersion',
    }


    def __init__( self ):
        self._c = Connection()
        self._c.connect( ('127.0.0.1',2000) )

    def run( self ):
        self.sendPVersion()
        s = self._c.recv()
        p = Parser()
        return p.parse(s)


    def parseCommands( self ):
        if self._c.hasData() == ([],[],[]):
            return

        str = self._c.recv()
        commands = Parser().parse( str )
        for command in commands:
            try:
                f = Protocol.functions[command[0]]
                method = getattr(self, 'recv%s' % f )
                method( command[1:] )
            except KeyError, e:
                raise
            except AttributeError, e:
                raise

    # --- send commands ----------------------------
    def sendPlayerID( self, name, isPlayer=1, isHuman=1 ):
        """ Sends players name, isPlayer, isHuman to server """
        self._c.send( '%s=%s,%d,%d' % ( Protocol.TOKEN_PLAYERID, name, isPlayer, isHuman ) )

    def sendColor( self, color ):
        """ Sends desired color to server """
        self._c.send( '%s=%d' % ( Protocol.TOKEN_COLOR, color ) )

    def sendMessage( self, msg ):
        """ Sends a message to all players """
        self._c.send( '%s=%s' % ( Protocol.TOKEN_MESSAGE, msg) )

    def sendExit( self ):
        """ Sends the Exit message """
        self._c.send( '%s' % Protocol.TOKEN_EXIT )

    def sendRequestCountries( self ):
        """ Request the status of all of the countries """
        self._c.send( '%s=-1' % (Protocol.TOKEN_COUNTRIES) )

    def sendEnumCards( self ):
        """ Ask the server to list all of my cards """
        self._c.send( '%s' % (Protocol.TOKEN_ENUM_CARDS) )

    def sendNewRound( self ):
        """ Start a new round """
        self._c.send( '%s' % (Protocol.TOKEN_NEW_ROUND) )

    def sendRequestStatus( self ):
        """ Request the status of the game """
        self._c.send( '%s' % (Protocol.TOKEN_STATUS) )

    def sendStart( self ):
        """ Start the game """
        self._c.send( '%s' % (Protocol.TOKEN_START) )

    def sendRequestCard( self ):
        """ send a request for card to the server """
        self._c.send( '%s' % (Protocol.TOKEN_TARJETA) )

    def sendMoveTroops( self, src, dst, total ):
        """ Move troops from src country to destination country """
        self._c.send( '%s=%d,%d,%d' % (Protocol.TOKEN_TROOPS,src,dst,total) )

    def sendEndTurn( self ):
        """ End my turn """
        self._c.send( '%s' % Protocol.TOKEN_TURN )

    def sendSurrender( self ):
        """ I'm coward and I will surrender """
        self._c.send( '%s' % Protocol.TOKEN_SURRENDER )

    def sendRequestMissions( self ):
        """ Ask the server what are my missions """
        self._c.send( '%s' % Protocol.TOKEN_MISSION )

    def sendRequestTypeOfGame( self ):
        """ What is the current type of the game """
        self._c.send( '%s' % Protocol.TOKEN_TYPEOFGAME )

    def sendSetTypeOfGame( self, conqWorld, fogOfWar, withCommon, armies1, armies2 ):
        """ Sets the type of the game """
        self._c.send("%s=%s=%d;%s=%s=%d;%s=%s=%d;%s=%s=%d,%d",
                Protocol.TOKEN_SET,Protocol.OPTION_CONQWORLD, conqWorld,
                Protocol.TOKEN_SET,Protocol.OPTION_FOG_OF_WAR, fogOfWar,
                Protocol.TOKEN_SET,Protocol.OPTION_CMISSION, withCommon,
                Protocol.TOKEN_SET,Protocol.OPTION_ARMIES,armies1,armies2 )

    def sendWhatToDo( self ):
        """ ask server what do I have to do (robots uses this) """
        self._c.send( "%s" % Protocol.TOKEN_SAYWHAT )

    def sendEcho( self, msg ):
        """ echo client (robots uses this) """
        self._c.send( "%s=%s" % ( Protocol.TOKEN_ECHO,msg ) )

    def sendPVersion( self ):
        """ Sends the protocol version, request the server version, send the client version """
        self._c.send("%s=%d,%d" % ( Protocol.TOKEN_PVERSION,Protocol.PROTOCOL_HIVER,Protocol.PROTOCOL_LOVER) )
        self._c.send("%s" % Protocol.TOKEN_SVERSION )
        self.sendCVersion()

    def sendCVersion( self ):
        """ sends client version """
        self._c.send("%s=%s %s" % ( Protocol.TOKEN_CVERSION,version.NAMEVERSION,version.NUMBERVERSION ))

    def sendRequestScores( self ):
        """ ask for scores """
        self._c.send( Protocol.TOKEN_SCORES )

    def sendRequestRobot( self ):
        """ request a robot """
        self._c.send(TOKEN_ROBOT)

    def sendRegroup( self, src, dst, total ):
        raise NotImplemented

    def sendPlace2Armies( self, country ):
        raise NotImplemented

    def sendExchange( self, c1, c2, c3 ):
        raise NotImplemented

    def sendPlaceArmies( self ):
        raise NotImplemented

    def sendAttack( self ):
        raise NotImplemented

    def sendReAttack( self ):
        raise NotImplemented

    # --- recv commands ----------------------------
    def recvPlayerID( self, str ):
        p = Parser(s=',').parse( str[0] )
        print p

    def recvPVersion( self, str ):
        p = Parser(s=',',kv=':').parse( str[0] )
        hi = p[0]
        lo = p[1]
        print p

        if hi != Protocol.PROTOCOL_HIVER:
            raise Exception("Aborting: Different protocols version")

    def recvSVersion( self, str ):
        print str

class Parser( object ):
    def __init__( self, s = '\n', kv = '=' ):
        self._command_separator = s
        self._keyvalue_separator = kv

    def parse( self, str ):
        l = []
        commands = str.split( self._command_separator )
        for command in commands:
            if command == [''] or command == '':
                continue
            kv = command.split( self._keyvalue_separator )
            if len(kv) > 0:
                l.append( kv )
        return l


if __name__ == '__main__':
    p = Protocol()
    p.run()
    p.sendPlayerID('pirulin')
    p.parseCommands()
