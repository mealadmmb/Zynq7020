#pragma once
#ifdef XSDK
#ifdef QFREERTOS

#include <QHostAddress.h>
#include <QByteArray.h>
#include <QTcpClient.h>
class QTcpServer {
public:


	QTcpServer(const QHostAddress&);
	bool doListen(const QHostAddress& address, uint16_t port);
	bool waitForClient();
	int64_t sendToClient(const int& client, void * data, int64_t len);
	int64_t receiveFromClient(const int& client, void* data, int64_t maxLen);
	bool deleteClient(const int& client);
	QVector<int> clients;





private:
	int localSocket = -1;
	const QHostAddress serverAddr;
};


#endif
#endif
