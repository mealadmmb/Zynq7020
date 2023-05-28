#pragma once
#ifdef XSDK
#ifdef QFREERTOS

#include <QHostAddress.h>
#include <QByteArray.h>

class QTcpClient {
	friend class QTcpServer;
public:
	QTcpClient(const QHostAddress& Addr, int sock):
		sock(sock),
		Addr(Addr)
{}
	QTcpClient():
		sock(-1),
		Addr(QHostAddress(""))
	{}

private:
	const int sock;
	const QHostAddress Addr;
};


#endif
#endif
