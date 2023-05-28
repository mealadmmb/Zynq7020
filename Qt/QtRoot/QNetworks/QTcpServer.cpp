#ifdef XSDK
#ifdef QFREERTOS

#include <QTcpServer.h>
#include <string.h>
#include "lwip/sockets.h"
bool QTcpServer::waitForClient() {
	struct sockaddr_in remote;
	int new_sd;
	int size = sizeof(remote);
	if ((new_sd = accept(localSocket, (struct sockaddr *)&remote,(socklen_t *)&size)) > 0){
		//TODO: Fix QTcpClient Class
		//		int newClient(QHostAddress(inet_ntoa(remote.sin_addr)), new_sd);
		printf("Recently a client connected with socket: %i\r\n", new_sd);
		printf("Recently a client connected with IP %s\r\n", inet_ntoa(remote.sin_addr));
		clients.push_back(new_sd);
	}
	return true;
}

QTcpServer::QTcpServer(const QHostAddress& Addr):
				serverAddr(Addr)
{
	/* set up address to connect to */
	//        memset(&address, 0, sizeof(address));

	if ((localSocket = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("TCP server: Error creating Socket\r\n");
		return;
	}
}

bool QTcpServer::doListen(const QHostAddress& address, uint16_t port){
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if(address.Address.size() > 0){

		addr.sin_addr.s_addr = inet_addr(address.Address.c_str());
	}
	else
		addr.sin_addr.s_addr = INADDR_ANY;

	int err = bind(localSocket, (struct sockaddr *)&addr, sizeof(addr));
	if (err != ERR_OK) {
		printf("QTcpServer: Error on bind: %d, %d\r\n", err, localSocket);
		close(localSocket);
		return false;
	}

	if (listen(localSocket, 1) < 0) {
		printf("TCP server: tcp_listen failed\r\n");
		close(localSocket);
		return false;
	}
	return true;
}

int64_t QTcpServer::sendToClient(const int& client, void* data, int64_t len) {
	int64_t sentByte = -1;
	if(client > 0){
		printf("before sending to client\r\n");
		sentByte =  send(client, (char *)data, len, 0);
		return sentByte;
	}
	else
		return -1;

}

int64_t QTcpServer::receiveFromClient(const int& client, void* data, int64_t maxLen) {
	int64_t receivedBytes = -1;

	if (client > 0) {
		receivedBytes = recv(client, (char *) data, maxLen, MSG_PEEK);
		return receivedBytes;
	}
	else {
		printf("Client Corrupted!\r\n");
		return -1;
	}

}

bool QTcpServer::deleteClient(const int& client) {
	for(size_t i = 0; i < clients.size(); i++) {
		if(clients[i] == client) {
			closesocket(client);
			printf("after closing socket and before delete client\r\n");
			clients.removeAt(i);
			return true;
		}
	}
	return false;
}

#endif
#endif
