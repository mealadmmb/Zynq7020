#pragma once
#include <cstddef>
#include <QFile.h>
#include <atomic>
#ifdef AXI_DMA
#include <AxiDMA.h>
#endif



#ifdef LWIPSOCKET
class QUdpSocket;
class QHostAddress;
#endif

#ifdef QFREERTOS
#include <QMutex.h>
#endif

class QDebug;

class QCircularBuffer {

public:

	QCircularBuffer(size_t size, QString _name = "QCircularBuffer", quint8 fillWith = 0, bool _dbg = false);
	size_t usedSpace() const { return writtenBytes.load(); }
	quint8 usedSpacePrecentage() const { return ((usedSpace() * 100.0F) / (double)_capacity); }

	size_t freeSpace() const { return (_capacity - writtenBytes.load()); }
	quint8 freeSpacePrecentage() const { return ((freeSpace() * 100.0F) / (double)_capacity); }
	quint8* begin() { return basePtr; }
	// this is a imaginary ptr, invalid to use
	quint8* end() { return (basePtr + _capacity); }
	size_t toFile(QFile& file, size_t size);
	bool isEmpty() const { return (usedSpace() == 0); }
	bool isFull() const { return (freeSpace() == 0); }
	size_t capacity() const { return _capacity; }
	void erase();


	bool getU8(quint8& out);
	bool getU32(quint32& out);
	bool getU64(quint64& out);


#ifdef LWIPSOCKET
	size_t fromQUdpSocket(QUdpSocket& socket, size_t size);
	size_t toQUdpSocket(const QHostAddress& host, quint16 port, size_t size);
#endif

#ifdef AXI_DMA
	size_t toAxiDMA(AxiDMA& dma, size_t size);
	size_t fromAxiDMA(AxiDMA& dma, size_t size);
#endif



	//private:

	quint8* basePtr = nullptr; // begin of allocated memory
	quint8* writePtr = nullptr; // always store the valid ptr to start writing data to
	quint8* readPtr = nullptr; // always store the valid ptr to start reading data from

	std::atomic<size_t> writtenBytes = 0;

	const size_t _capacity;

	QDebug dbg();
	QString name;
	bool dbgEnable;
#ifdef QFREERTOS
	QMutex mutex = QMutex(QMutex::NonRecursive, true);
#endif

};
