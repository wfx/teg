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

#pragma once

#include <cstddef>
#include <sys/types.h>

namespace teg
{

/** \brief A bidirectional line oriented connection
 *
 * A transport hides how the client and the server talk to each other. The
 * current implementation is backed by a file descriptor, so it can be used in
 * select()/poll() based event loops and in the GTK main loop. Future
 * implementations can use an in-process queue to embed the server into the
 * client as a library.
 *
 * All transports are line oriented: messages are terminated by a newline.
 */
class Transport {
public:
	virtual ~Transport() = default;

	/// \brief File descriptor usable for readiness notification, or -1.
	virtual int fd() const = 0;

	/** \brief Read one line into \p buffer
	 *
	 * The line including the terminating newline is stored NUL terminated when
	 * there is enough space.
	 *
	 * \return the number of bytes read, 0 on end of file, -1 on error.
	 */
	virtual ssize_t read_line(void *buffer, std::size_t maxlen) = 0;

	/// \brief Write all bytes of \p data.
	/// \return true on success, false on error.
	virtual bool write(char const *data, std::size_t len) = 0;

	/// \brief Close the connection. The transport must not be used afterwards.
	virtual void close() = 0;

	/// \brief Write the NUL terminated string \p msg.
	int print(char const *msg);

	/// \brief Format and write a message.
	int printf(char const *format, ...) __attribute__((format(printf, 2, 3)));
};

/** \brief A transport backed by a POSIX file descriptor
 *
 * Works for sockets, socketpairs, pipes and terminals. If \p owns is true, the
 * descriptor is closed in the destructor and by close().
 */
class SocketTransport final: public Transport {
public:
	explicit SocketTransport(int fd, bool owns = true);
	~SocketTransport() override;

	int fd() const override;
	ssize_t read_line(void *buffer, std::size_t maxlen) override;
	bool write(char const *data, std::size_t len) override;
	void close() override;

private:
	int fd_;
	bool owns_;
};

}
