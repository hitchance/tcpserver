#include "server.h"

using namespace wrp;

void c_server::initialize() {
	// clear the terminal.
	system("clear");
	
	std::cout << "starting tcp server on " << m_port << std::endl;

	m_fd = _socket(AF_INET, SOCK_STREAM, 0);

	// will allow us to re-use the same address.
	_setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR);

	// fill in the structure.
	m_server.sin_family = AF_INET;
	m_server.sin_port = htons(m_port);
	m_server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	// assign the structure to socket.
	_bind(m_fd, (sockaddr*)&m_server, sizeof(m_server));

	// set socket to listening mode & limit the ammount of possible connections at once.
	_listen(m_fd, MAX_CON);

	std::cout << yellow << "BUFFSYNC228" << red << " SERVER " << green << "PRESENTS" << reset << std::endl;

	std::cout << "/-----server initialized-----/" << std::endl;
	std::cout << "telnet " << inet_ntoa(m_server.sin_addr) << " " << htons(m_server.sin_port) << std::endl;
	std::cout << "/----------------------------/" << std::endl;
	
	// server is ready to accept new connections.
	m_accept = true;
	m_total_con = 0;
	m_curr_con = 0;
}

void c_server::handle_connections(user_t* user) {
	m_curr_con++;
	m_total_con++;

	user->m_t_con = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	user->m_t_act = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	user->m_o_socket_flags = fcntl(user->m_socket, F_GETFL); // store them in case we need them later

	while (true) {

		fcntl(user->m_socket, F_SETFL, O_NONBLOCK);

		uint64_t curtime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		if (curtime - user->m_t_con > 300) {
			std::cout << user->m_socket << " was kicked due to session expiration(300sec).." << std::endl;
			break;
		}

		if (curtime - user->m_t_act > 30) {
			std::cout << user->m_socket << " was kicked due to inactivity(30sec).." << std::endl;
			break;
		}

		char buf[512];
		int b = _read(user->m_socket, buf, sizeof(buf));

		if (b > 0) {
			user->m_t_act = curtime;
			auto inc = std::string(buf, 0, b - 2);
			std::cout << inc << std::endl;
		}
		else if (b == 0) {
			break; // disconnected
		}
		else {
			// error, since packet is empty..
		}

	}

	_close(user->m_socket);
	user->m_t_con = 0;
	m_curr_con--;
	user->m_status = -1;
}

void c_server::accept_users() {
	// find free array spot
	for (int i = 0; i < MAX_CON; i++) {
		if (m_users.at(i).m_status == -1) {
			m_users.at(i).m_thread.detach();
			m_users.at(i).m_status = 1;
		}

		m_accept = m_curr_con < MAX_CON;
		if (!m_accept)
			continue;

		if (m_users.at(i).m_status == 1) {
			socklen_t m_client_len = sizeof(m_client);
			if (socketfd_t m_socket1 = _accept(m_fd, (sockaddr*)&m_client, &m_client_len)) {
				if (m_users.at(i).m_status == 1) {
					m_users.at(i).m_status = 0;
					m_users.at(i).m_socket = m_socket1;
					m_users.at(i).m_thread = std::thread(&c_server::handle_connections, this, &m_users.at(i));
				}
			}
		}
	}
}

void c_server::uninitialize() {
	std::cout << "shutting down!" << std::endl;
	_close(m_fd);
}