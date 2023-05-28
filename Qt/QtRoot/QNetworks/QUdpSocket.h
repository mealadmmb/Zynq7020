#pragma once
#ifdef LWIPSOCKET

#include <QHostAddress.h>
#include <QDataContainer.h>
#include <lwipopts.h>
struct sockaddr_in;
class QHostAddress;
class QUdpSocket {
public:

	struct socketDataContainer {
		socketDataContainer(size_t reserve) : reserved(reserve) {
			buffer = new uint8_t[reserved];
		}
		~socketDataContainer() {
			delete buffer;
		}
		inline QByteArray toQByteArray() const {
			return QByteArray((char*)buffer, size);
		}

		quint8* buffer = nullptr;
		quint16 size;
		quint16 reserved;
	};

	static constexpr quint32 maxNonFragmentPayloadSize = IP_FRAG_MAX_MTU; // avoiding to fragmentation
	static constexpr quint32 maxFragmentPayloadSize = 65507; // max valid size while fragmentation allowed

	QUdpSocket();
	~QUdpSocket();
	bool bindToHost(const QHostAddress& host, quint16 port);

	qint64 writeDatagram(const void *data, qint64 len, const QHostAddress& host, quint16 port);
	inline qint64 writeDatagram(const QByteArray &datagram, const QHostAddress& host, quint16 port)
	{ return writeDatagram(datagram.constData(), datagram.size(), host, port); }
	inline qint64 writeDatagram(const QDataContainer& dc, const QHostAddress& host, quint16 port) {
		return writeDatagram(dc.ptr, dc.size, host, port); }

	qint64 readDatagram(char *data, qint64 maxlen, QHostAddress* host = nullptr, quint16* port = nullptr);
	QByteArray readDatagram(qint64 maxlen, QHostAddress* host = nullptr, quint16* port = nullptr);

private:
	QHostAddress _host;
	quint16 _port;


	int _sock = -1;
	sockaddr_in* _addr;
	quint32 _addrLen;


};


#endif
