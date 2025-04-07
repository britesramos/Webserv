#include "http_tcpServer_linux.h"

int	main(){
	TcpServer server = TcpServer("0.0.0.0", 8080);
	return 0;
}