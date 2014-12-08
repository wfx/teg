#!/usr/bin/python
#		$Id: tegmetaserver.py,v 1.13 2002/11/14 11:29:33 riq Exp $
"""
 Tenes Empanadas Graciela

 Copyright (C) 2000 Ricardo Quesada

 Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; only version 2 of the License

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.


 This is the TEG's metaserver

 A subclass of SimpleHTTPRequestHandler

 To get info about the metaserver you can use the WEB

 Available commands:
	/addServer/server_name/port/version/description
	/delServer/server_name/port
	/listServers
	/listServers_xml

"""

import SimpleHTTPServer
import BaseHTTPServer
import time

class Singleton:
	__single = None
	def __init__( self ):
		if Singleton.__single:
			raise Singleton.__single
		Singleton.__single = self	 

class TEGData( Singleton ):

	NAME = 0
	PORT = 1
	VERSION = 2
	COMMENT = 3
	TIME = 4

	FIVE_MIN = 5 * 60

	def __init__( self ):
		Singleton.__init__( self )
		self.tegServers = {}

	def addServer( self, key, value):
		# value is a list of:
		#   name, port, version, comment
		# append timestamp
		value.append( time.time() )
		self.tegServers[key] = value

		self.updateTimeValues()

	def delServer( self, key):
		self.tegServers[key] = None
		self.updateTimeValues()

	def listServers( self ):
		self.updateTimeValues()

		v = self.tegServers.values()
		for i in range(0,len(v)):
			try:
				v.remove(None)
			except ValueError:
				continue
		return v

	def updateTimeValues( self ):
		# Remove times that are more than 5 minutes old
		now = time.time()
		for i in self.tegServers.keys():
			val = self.tegServers[ i ]
			if ( val != None and val[TEGData.TIME] + TEGData.FIVE_MIN ) < now :
				self.tegServers[ i ] = None

def Handle( x = Singleton ):
	try:
		single = x()
	except Singleton, s:
		single = s
	return single	 


class TEGRequestHandler( SimpleHTTPServer.SimpleHTTPRequestHandler ):

	def teg_listServers(self):

		self.wfile.write('<!DOCTYPE html PUBLIC "-//w3c//dtd html 4.0 transitional//en">\n')
		self.wfile.write("<html>\n<head>\n<title>TEG's Metaserver</title>\n</head>\n")
		str ="<body>\n"
		data = Handle(TEGData)

		str +='<center>'
		str +='<table BORDER=0 BGCOLOR="#dedede" NOSAVE>\n'
		str +="<th><center>Available TEG's servers</center></th>"
		str +="<tr>\n"
		str +="\t<td><b>Server</b></td>\n"
		str +="\t<td><b>Port</b></td>\n"
		str +="\t<td><b>Version</b></td>\n"
		str +="\t<td><b>Comments</b></td>\n"
		str +="\t<td><b>Last Ping</b></td>\n"
		str +="</tr>\n"

		index = 0
		for i in data.listServers():
			if index % 2 == 0:
				str +='<tr BGCOLOR="#eeeeee" NOSAVE>\n'
			else:
				str +='<tr BGCOLOR="#dedede" NOSAVE>\n'

			index += 1

			str +="<!-- entry -->"
			try:
				for j in range(0,len(i)):
					str +="<td>%s</td>" % i[j]
				str +="</tr>\n"
			except:
				print 'Invalid entry:'
				print i

		str +="</table>\n"
		str +='</center>\n'
		str +="</body>\n</html>\n"
		self.wfile.write('\n' + str)

	def teg_listServers_xml(self):

		data = Handle(TEGData)
		str = '<?xml version="1.0"?>\n'
		str += '<teg_metaserver version="1.0">\n'

		for i in data.listServers():
			str += '\t<entry>\n'
			try:
				len_i = len(i)
				for j in range(0,len_i):
					if j % len_i == 0:
						str +="\t\t<name>%s</name>\n" % i[j]
					if j % len_i == 1:
						str +="\t\t<port>%s</port>\n" % i[j]
					if j % len_i == 2:
						str +="\t\t<version>%s</version>\n" % i[j]
					if j % len_i == 3:
						str +="\t\t<comment>%s</comment>\n" % i[j]
			except:
				print 'Invalid entry:'
				print i
			str +="\t</entry>\n"
		str +='</teg_metaserver>\n'
		self.wfile.write(str)

	def teg_addServer(self):
		if len(self.teg_params) == 4:

			# replace '+' with ' ' in comments
			self.teg_params[3] = self.teg_params[3].replace('+',' ')
			self.teg_params[3] = self.teg_params[3].replace('<','_')
			self.teg_params[3] = self.teg_params[3].replace('>','_')

			data = Handle(TEGData)
			data.addServer( self.teg_params[0] + ':' + self.teg_params[1], self.teg_params )
			self.wfile.write('<html><head><title>server added</title></head></html>')
		else:			 
			self.send_error(400, "invalid parameters")

	def teg_delServer(self):
		if len(self.teg_command) > 1:
			data = Handle(TEGData)
			data.delServer( self.teg_params[0] + ':' + self.teg_params[1] )
			self.wfile.write('<html><head><title>server deleted</title></head></html>')
		else:			 
			self.send_error(400, "invalid parameters")
	
	def do_GET(self):
		"""Serve a GET request."""

		self.teg_command = self.path[1:].split('/')  

		if len(self.teg_command) > 0:
				mname = 'teg_' + self.teg_command[0]
				if not hasattr(self, mname):
					self.send_error(501, "Unsupported method (%s)" % `self.teg_command[0]`)
					return
				self.teg_params = self.teg_command[1:]

				# use peer address, instead of the one provided by user
				if len( self.teg_params ) > 0:
					self.teg_params[0] = self.client_address[0]

				method = getattr(self, mname)
				method()
		else:
			self.send_error(400, "Invalid request" )
			return

	def do_HEAD(self):
		pass


def main(HandlerClass = TEGRequestHandler, ServerClass = BaseHTTPServer.HTTPServer):

	BaseHTTPServer.test(HandlerClass, ServerClass)

main()
