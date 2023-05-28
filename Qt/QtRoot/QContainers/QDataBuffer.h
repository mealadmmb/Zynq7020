#pragma once
#include <QDataContainer.h>
#include <iostream>

#ifdef AXI_DMA
class AxiDMA;
#endif

#ifdef LWIPSOCKET
class QUdpSocket;
#endif

#pragma pack(1)
struct QDataBuffer {
	friend class QUdpSocket;

public:
	explicit QDataBuffer(size_t max);
	QDataBuffer();

	QDataBuffer& operator<<(const QDataContainer& newData);
	QDataBuffer& operator<<(const quint8& newData);
	quint8* getPtr() { return ptr; };
	const quint8* getPtr() const { return ptr; };
	quint8* getCurser() { return (ptr + bufferedCount); }
	size_t getBufferedCount() { return bufferedCount; }
	size_t getBufferedCount() const { return bufferedCount; }
	bool isSpaceAvailable() { return (bufferedCount < capacity); }
	void reset();
	void renew();
	void free();
	bool allocate(size_t size);
	bool reserve(size_t n);
	size_t getFreeSpace() { return (capacity - bufferedCount); }
	quint8 getUsagePrecentage() { return ((getBufferedCount() * 100) / capacity); }
	quint8 getFreePrecentage() { return (100 - getUsagePrecentage()); }

#ifdef LWIPSOCKET
size_t fromQUdpSocket(QUdpSocket& sock, size_t maxlen);
#endif

#ifdef AXI_DMA
int toAxiDMA(AxiDMA& dma);
#endif

friend std::ostream& operator<<(std::ostream& os, const QDataBuffer& qDataBuffer);


//sprivate:
quint8* ptr = nullptr;
size_t capacity;
size_t bufferedCount = 0;

};
#pragma pack()
