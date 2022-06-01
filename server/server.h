#pragma once

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <iostream>
#include <thread>
#include <string>
#include <ctime>
#include <array>

typedef int socketfd_t;

const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string yellow("\033[1;33m");
const std::string cyan("\033[0;36m");
const std::string magenta("\033[0;35m");
const std::string reset("\033[0m");

const int MAX_CON = 2;

namespace wrp {
	inline socketfd_t _socket(int domain, int type, int protocol) {
		socketfd_t res = socket(domain, type, protocol);
		if (res == -1) {
			perror("_socket returned this error");
			exit(0X0001);
		}
		//std::cout << " _socket in action" << std::endl;
		return res;
	}

	inline int _bind(socketfd_t sockfd, const struct sockaddr* addr, socklen_t addrlen) {
		int res = bind(sockfd, addr, addrlen);
		if (res == -1) {
			perror("_bind returned this error");
			exit(0x0002);
		}
		//std::cout << " _bind in action" << std::endl;
		return res;
	}

	inline int _listen(socketfd_t sockfd, int backlog) {
		int res = listen(sockfd, backlog);
		if (res == -1) {
			perror("_listen returned this error");
			exit(0x0003);
		}
		//std::cout << " _listen in action" << std::endl;
		return res;
	}

	inline int _accept(socketfd_t sockfd, struct sockaddr* addr, socklen_t* addrlen) {
		int res = accept(sockfd, addr, addrlen);
		if (res == -1) {
			perror("_accept returned this error: ");
			exit(0x0004);
		}
		return res;
	}

	inline int _read(socketfd_t sockfd, void* buf, size_t len) {
		int res = read(sockfd, buf, len);
		//if (res == -1) {
		//	perror("_read returned this error");
		//	exit(0x0005);
		//}
		return res;
	}

	inline int _send(socketfd_t sockfd, const void* buf, size_t len, int flags) {
		int res = send(sockfd, buf, len, flags);
		if (res == -1) {
			perror("_send returned this error");
			exit(0x0006);
		}
		//std::cout << "sent data to:" << sockfd << std::endl;
		return res;
	}

	inline int _close(socketfd_t fd) {
		int res = close(fd);
		if (res == -1) {
			perror("_close returned this error");
			exit(0x0007);
		}
		return res;
	}

	inline int _setsockopt(socketfd_t sockfd, int level, int optname) {
		int a = 1;
		int res = setsockopt(sockfd, level, optname, &a, sizeof(a));
		if (res == -1) {
			perror("_setsockopt returned this error");
			exit(0x0008);
		}
		return res;
	}
}

struct user_t {
public:
	int m_status = 1; // 1 - ready, 0 - active , -1 - delete me
	int m_o_socket_flags; // original socket flags.
	std::thread m_thread;
	socketfd_t m_socket;
	uint64_t m_t_con; // connection time.
	uint64_t m_t_act; // last activity time.. e.g connected/send/recv..
};

class c_server {
private: // status 
	std::array<user_t, MAX_CON> m_users;
	bool m_accept;
	int m_total_con;
	int m_curr_con;
private: // settings
	int m_port; 
private: // setup 
	socketfd_t m_fd;
	sockaddr_in m_server, m_client;
public:
	void accept_users();
	void handle_connections(user_t* user);

	void initialize();
	void uninitialize();

	c_server(int port) {
		m_port = port;
		initialize();
	}
};