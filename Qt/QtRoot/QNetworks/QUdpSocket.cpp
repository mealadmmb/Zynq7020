#ifdef LWIPSOCKET

#include <QUdpSocket.h>
#include <string.h>
#include "lwip/sockets.h"

qint64 QUdpSocket::writeDatagram(const void* data, qint64 len, const QHostAddress& host, quint16 port) {


	int sock = -1;
	qint64 out = -1;
	struct sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	err_t err;

	sock = lwip_socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		printf("QUdpSocket: Error creating Socket, error: %X, sock: %i\r\n", errno, sock);
		lwip_close(sock);
		return out;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(host.Address.c_str());

	err = connect(sock, (struct sockaddr *)&addr, addrLen);
	if (err != ERR_OK) {
		lwip_close(sock);
		return out;
	}

	out = lwip_sendto(sock, data, len, 0, (struct sockaddr *)&addr, addrLen);
	lwip_close(sock);
	return out;

}

qint64 QUdpSocket::readDatagram(char* data, qint64 maxlen, QHostAddress* host, quint16* port) {

	qint64 out = -1;
	if(_sock == -1 ) return out;
	out = lwip_recvfrom(_sock, data, maxlen, 0, (struct sockaddr *)_addr, (socklen_t*)&_addrLen);
	return out;

}

bool QUdpSocket::bindToHost(const QHostAddress& host, quint16 port) {

	_host = host;
	_port = port;

	lwip_close(_sock);
	_sock = -1;

	memset(_addr, 0, sizeof(struct sockaddr_in));

	err_t err;
	bool out = true;

	_sock = lwip_socket(AF_INET, SOCK_DGRAM, 0);
	if (_sock < 0) {
		printf("QUdpSocket: Error creating Socket\r\n");
		lwip_close(_sock);
		_sock = -1;
		out = false;
	}
	else {
		_addr->sin_family = AF_INET;
		_addr->sin_port = htons(_port);
		if(_host.Address.size() > 0)
			_addr->sin_addr.s_addr = inet_addr(_host.Address.c_str());
		else
			_addr->sin_addr.s_addr = INADDR_ANY;

		if((err = lwip_bind(_sock, (struct sockaddr *)_addr, _addrLen)) != ERR_OK) {
			printf("QUdpSocket: Error on bind: %d\r\n", err);
			lwip_close(_sock);
			_sock = -1;
			out = false;
		}
	}
	return out;
}

QUdpSocket::QUdpSocket() {
	_addr = new sockaddr_in();
	_addrLen = sizeof(*_addr);
}

QUdpSocket::~QUdpSocket() {
	if(_addr) delete _addr;
}



QByteArray QUdpSocket::readDatagram(qint64 maxlen, QHostAddress* host, quint16* port) {
	QUdpSocket::socketDataContainer in(maxlen);
	in.size = readDatagram((char*)in.buffer, maxlen);
	return in.toQByteArray();
}

#endif
