#include "server/server.h"


int main()
{
	c_server server(22313);

	while (true) {
		server.accept_users();
	}
	
	//server.uninitialize();

	return 0;
}
