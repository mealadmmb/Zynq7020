#include <QDataBuffer.h>
#include <stdio.h>
#ifdef LWIPSOCKET
#include <QUdpSocket.h>
#endif
#ifdef AXI_DMA
#include <AxiDMA.h>
#endif

void QDataBuffer::free() {
	if(ptr) {
		delete ptr;
		ptr = nullptr;
		bufferedCount = 0;
	}
}

QDataBuffer::QDataBuffer() {
}

bool QDataBuffer::allocate(size_t size) {
	if(ptr) return false;
	ptr = nullptr;
	ptr = new quint8[size];
	if(ptr) return true;
	return false;
}

std::ostream &operator<<(std::ostream &os, const QDataBuffer& qDataBuffer) {

	if(!qDataBuffer.ptr || !qDataBuffer.bufferedCount || !qDataBuffer.capacity) {
		os << "empty QDataBuffer";
		return os;
	}
	else {
		for (size_t i = 0; i < qDataBuffer.bufferedCount; i++) {
			printf("%02X", *(qDataBuffer.ptr + i));
		}
	}
	return os;
}

QDataBuffer &QDataBuffer::operator<<(const QDataContainer &newData) {

	if(getFreeSpace() < newData.size)
		if(!reserve((newData.size - getFreeSpace()) * 2)) return *this;

	std::copy(newData.ptr, newData.ptr + newData.size, getCurser());
	bufferedCount += newData.size;

	return *this;
}

void QDataBuffer::reset() {
	if (ptr){
		bufferedCount = 0;
	}
}


QDataBuffer::QDataBuffer(size_t max)  : capacity(max) {
	ptr = new quint8[capacity];
	if(ptr == nullptr)
		std::cout << "error in allocate buffer\n";
}

QDataBuffer &QDataBuffer::operator<<(const quint8 &newData) {

	if(getFreeSpace() < 1)
		if(!reserve((1 - getFreeSpace()) * 2)) return *this;

	*(getCurser()) = newData;
	bufferedCount += 1;

	return *this;
}

bool QDataBuffer::reserve(size_t n) {

	quint8* temp = nullptr;
	temp = new quint8[capacity + n];
	if(temp == nullptr) return false;

	std::copy(ptr, getCurser(), temp);

	delete ptr;
	ptr = temp;
	capacity += n;
	return true;

}



#ifdef LWIPSOCKET
size_t QDataBuffer::fromQUdpSocket(QUdpSocket& sock, size_t maxlen) {
	if(maxlen > getFreeSpace()) return 0;
	auto receivedBytes = sock.readDatagram((char*)getCurser(), maxlen);
	if(receivedBytes < 0) return 0;
	bufferedCount += receivedBytes;
	return receivedBytes;
}
#endif

#ifdef AXI_DMA
int QDataBuffer::toAxiDMA(AxiDMA& dma) {
	return dma.simpleTransferMM2S((UINTPTR)getPtr(), getBufferedCount());
}
#endif
