#include "http_tcpServer_linux.h"

int	main(){
	TcpServer server = TcpServer();
	server.startServer();
	return 0;
}