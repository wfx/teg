/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#include "transport.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "net.h"
#include "protocol.h"

namespace teg
{

int Transport::print(char const *msg)
{
	std::size_t const len{strlen(msg)};
	if(write(msg, len)) {
		return static_cast<int>(len);
	}
	return -1;
}

int Transport::printf(char const *format, ...)
{
	char buffer[PROT_MAX_LEN];

	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	buffer[sizeof(buffer) - 1] = 0;

	return print(buffer);
}

SocketTransport::SocketTransport(int fd, bool owns)
	: fd_{fd}
	, owns_{owns}
{
}

SocketTransport::~SocketTransport()
{
	if(owns_) {
		net_close(fd_);
	}
}

int SocketTransport::fd() const
{
	return fd_;
}

ssize_t SocketTransport::read_line(void *buffer, std::size_t maxlen)
{
	return net_readline(fd_, buffer, maxlen);
}

bool SocketTransport::write(char const *data, std::size_t len)
{
	return net_writen(fd_, data, len) >= 0;
}

void SocketTransport::close()
{
	if(owns_ && fd_ >= 0) {
		net_close(fd_);
	}
	fd_ = -1;
}

}
