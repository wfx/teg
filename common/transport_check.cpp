// SPDX-License-Identifier: GPL-2.0-or-later

#include "transport.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <gtest/gtest.h>

namespace teg
{

namespace
{

void make_socketpair(int sockets[2])
{
	ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, sockets));
}

}

TEST(Transport, line_roundtrip)
{
	int sockets[2];
	make_socketpair(sockets);
	SocketTransport sender{sockets[0]};
	SocketTransport receiver{sockets[1]};

	char buffer[64];
	EXPECT_EQ(6, sender.printf("hello\n"));
	EXPECT_EQ(6, receiver.read_line(buffer, sizeof(buffer)));
	EXPECT_STREQ("hello\n", buffer);
}

TEST(Transport, multiple_lines)
{
	int sockets[2];
	make_socketpair(sockets);
	SocketTransport sender{sockets[0]};
	SocketTransport receiver{sockets[1]};

	char buffer[64];
	EXPECT_EQ(6, sender.print("first\n"));
	EXPECT_EQ(7, sender.print("second\n"));

	EXPECT_EQ(6, receiver.read_line(buffer, sizeof(buffer)));
	EXPECT_STREQ("first\n", buffer);
	EXPECT_EQ(7, receiver.read_line(buffer, sizeof(buffer)));
	EXPECT_STREQ("second\n", buffer);
}

TEST(Transport, end_of_file)
{
	int sockets[2];
	make_socketpair(sockets);
	SocketTransport sender{sockets[0]};
	SocketTransport receiver{sockets[1]};
	sender.close();

	char buffer[64];
	EXPECT_EQ(0, receiver.read_line(buffer, sizeof(buffer)));
}

TEST(Transport, close_is_idempotent)
{
	int sockets[2];
	make_socketpair(sockets);

	SocketTransport transport{sockets[0]};
	EXPECT_EQ(sockets[0], transport.fd());
	transport.close();
	EXPECT_EQ(-1, transport.fd());
	transport.close();
	EXPECT_EQ(-1, transport.fd());

	close(sockets[1]);
}

TEST(Transport, borrowed_descriptor_is_not_closed)
{
	int sockets[2];
	make_socketpair(sockets);

	{
		SocketTransport transport{sockets[0], false};
		EXPECT_EQ(sockets[0], transport.fd());
	}
	EXPECT_NE(-1, fcntl(sockets[0], F_GETFD));

	close(sockets[0]);
	close(sockets[1]);
}

}
