#include <psl1ght/lv2/net.h>

#include <psl1ght/lv2/errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>

#define FD(socket) (socket & ~SOCKET_FD_MASK)

int connect(int socket, const struct sockaddr* address, socklen_t address_len)
{
	return lv2Errno(lv2NetConnect(FD(socket), address, (lv2_socklen_t)address_len));
}

int socket(int domain, int type, int protocol)
{
	s32 ret = lv2NetSocket(domain, type, protocol);
	if (ret < 0)
		return lv2Errno(ret);
	return ret | SOCKET_FD_MASK;
}

ssize_t send(int socket, const void* message, size_t length, int flags)
{
	return sendto(FD(socket), message, length, flags, NULL, 0);
}

ssize_t sendto(int socket, const void* message, size_t length, int flags,
		const struct sockaddr* dest_addr, socklen_t dest_len)
{
	s32 ret = lv2NetSendto(FD(socket), message, length, flags, dest_addr, dest_len);
	if (ret < 0)
		return lv2Errno(ret);
	return ret;
}

int shutdown(int socket, int how)
{
	return lv2Errno(lv2NetShutdown(FD(socket), how));
}

int net_close(int fd)
{
	return lv2NetClose(FD(fd));
}

int inet_aton(const char* cp, struct in_addr* inp)
{
	unsigned int num1;
	unsigned int num2;
	unsigned int num3;
	unsigned int num4;
	if (sscanf(cp, "%u.%u.%u.%u", &num1, &num2, &num3, &num4) != 4)
		return 0;
	if ((num1 | num2 | num3 | num4) & 0xFFFFFF00)
		return 0;

	inp->s_addr = htonl((num1 << 24) | (num2 << 16) | (num3 << 8) | num4);

	return 1;
}

int inet_pton(int af, const char* src, void* dst)
{
	switch (af) {
		case AF_INET:
			return inet_aton(src, (struct in_addr*)dst);
		case AF_INET6:
			break;
	}

	return 1;
}
