#pragma once
#ifdef XSDK
#include <QDataBuffer.h>
#include <QVector.h>
#include <QSet.h>

#ifdef AXI_DMA
#include <AxiDMA.h>
#endif

#ifdef QFREERTOS
#include <QMutex.h>
#include <QMutexLocker.h>
#endif


class QUdpSocket;


class QDataBufferHolder {
public:
	QDataBufferHolder(size_t numberOfBuffers, size_t bufferSize);
#ifdef LWIPSOCKET
	bool  fromQUdpSocket(QUdpSocket& sock, size_t maxlen);
#endif
#ifdef AXI_DMA
	void toAxiDMA(AxiDMA& dma);
#endif
	void freePendingBuffers();
	size_t getFreeBuffersCount() { return freeBufferIndexes.size(); }
	size_t getFullBuffersCount() { return fullBufferIndexes.size(); }
	size_t getPendingBuffersCount() { return pendingTofreeIndexes.size(); }
	size_t getAllBuffersCount() { return buffers.size(); }


private:

	void printFreeBufferIndexes() { std::cout << freeBufferIndexes << "\n"; }
	void printFullBufferIndexes() { std::cout << fullBufferIndexes << "\n"; }
	QSet<size_t> fullBufferIndexes;
	QSet<size_t> freeBufferIndexes;
	QSet<size_t> pendingTofreeIndexes;
	QVector<QDataBuffer> buffers;

#ifdef QFREERTOS
	QMutex mutex = QMutex(QMutex::NonRecursive, true);
#endif

};
#endif
