#include <QCircularBuffer.h>
#include <QDebug.h>
#include <memory>
#include <cstring>

#ifdef LWIPSOCKET
#include <QUdpSocket.h>
#endif

QDebug QCircularBuffer::dbg() {
	QDebug out(dbgEnable);
	out << name << ":";
	return out;
}


QCircularBuffer::QCircularBuffer(size_t size, QString _name, quint8 fillWith, bool _dbg) :
_capacity(size),
name(_name),
dbgEnable(_dbg)
{
	basePtr = new quint8[_capacity];
	readPtr = basePtr;
	writePtr = basePtr;
	if(!basePtr || !writePtr || !readPtr) {
		dbg() << "error in allocating " << _capacity << " bytes\n";
	}
	if(basePtr && fillWith != 0) {
		memset(basePtr, fillWith, _capacity);
	}
}

#ifdef LWIPSOCKET

size_t QCircularBuffer::toQUdpSocket(const QHostAddress& host, quint16 port, size_t size) {

	size_t out = 0;
	qint64 transferOut = 0;
	size_t availableSize = usedSpace();
	if (availableSize == 0) {
		dbg() << "buffer empty";
		return out;
	}

	if (size == 0) {
		dbg() << "requested with zero size";
		return out;
	}

	if(size > availableSize) {
//		size = availableSize;
		dbg() << "requested size larger than available data";
		return out;
	}
	if (size > QUdpSocket::maxFragmentPayloadSize) {  // 65507 is max UDP standard payload size
		dbg() << "UDP max payload size limit";
		return out;
	}


	static QUdpSocket socket;

	if(readPtr > writePtr) {
		if((readPtr + size) == end()) {
			transferOut = socket.writeDatagram(readPtr, size, host, port);
			if(transferOut != size) { dbg() << "toQUdpSocket failed 1 with size:" << size; return out; }

			out += size;
			writtenBytes.fetch_sub(size);
			readPtr = basePtr;
		}
		else if((readPtr + size) > end()) { // we need to pass data to LAN in two stage cause of separation of data in buffer
			size = end() - readPtr;
			transferOut = socket.writeDatagram(readPtr, size, host, port);
			if(transferOut != size) { dbg() << "toQUdpSocket failed 2 with size:" << size; return out; }
			out += size;
			writtenBytes.fetch_sub(size);
			readPtr = basePtr;
		}
		else {
			transferOut = socket.writeDatagram(readPtr, size, host, port);
			if(transferOut != size) { dbg() << "toQUdpSocket failed with 3 size:" << size; return out; }

			out += size;
			writtenBytes.fetch_sub(size);
			readPtr += size;

		}
	}
	else {
		transferOut = socket.writeDatagram(readPtr, size, host, port);
		if(transferOut != size) { dbg() << "toQUdpSocket failed with size:" << size; return out; }

		out += size;
		writtenBytes.fetch_sub(size);
		readPtr += size;

	}
	return out;

}

size_t QCircularBuffer::fromQUdpSocket(QUdpSocket& socket, size_t size) {
	size_t out = 0;
	if(freeSpace() < size) { return out; }
	// need to split writing
	if ((writePtr + size) == end()) {
		qint64 readDatagramOut = socket.readDatagram((char*)writePtr, size);
		if(readDatagramOut <= 0) { dbg() << "readDatagram failed\r\n"; return out; }
		out += readDatagramOut;
		writtenBytes.fetch_add(size);
		writePtr = basePtr;
	}

	else if((writePtr + size) > end()) {
		std::unique_ptr<char> temp(new char[size]);

		qint64 readDatagramOut = socket.readDatagram(temp.get(), size);
		if(readDatagramOut <= 0) { dbg() << "fromQUdpSocket failed\r\n"; return out; }
		out += readDatagramOut;
		size_t tailSize = end() - writePtr;

		std::copy(temp.get(), temp.get() + tailSize, writePtr);
		//memcpy(writePtr, temp, tailSize);
		writtenBytes.fetch_add(tailSize);
		writePtr = basePtr;

		std::copy(temp.get() + tailSize, temp.get() + size, writePtr);
		//memcpy(writePtr, temp + tailSize, size - tailSize);
		writtenBytes.fetch_add(size - tailSize);
		writePtr += (size - tailSize);

	}
	else {
		qint64 readDatagramOut = socket.readDatagram((char*)writePtr, size);
		if(readDatagramOut <= 0) { dbg() << "fromQUdpSocket failed\r\n"; return out; }
		out += readDatagramOut;
		writePtr += out;
		writtenBytes.fetch_add(out);
	}
	return out;
}
#endif



size_t QCircularBuffer::toFile(QFile& file, size_t size) {

	size_t out = 0;
	size_t availableSize = usedSpace();
	if (availableSize == 0 || size == 0) { return out; }
	if (availableSize <= size) { size = availableSize; }
	if(!file.isOpen() || file.getOpenMode() == QFile::ReadOnly) { return out; }
	if(readPtr > writePtr) {
		if((readPtr + size) == end()) {
			size = file.write(readPtr, size);
			out += size;
			writtenBytes.fetch_sub(size);
			readPtr = basePtr;
		}
		else if((readPtr + size) > end()) {
			size_t firstWriteCount = end() - readPtr;
			firstWriteCount = file.write(readPtr, firstWriteCount);
			out += firstWriteCount;
			writtenBytes.fetch_sub(firstWriteCount);
			readPtr = basePtr;
		}
		else {
			size = file.write(readPtr, size);
			writtenBytes.fetch_sub(size);
			readPtr += size;
			out += size;
		}
	}
	else {
		size = file.write(readPtr, size);
		writtenBytes.fetch_sub(size);
		readPtr += size;
		out += size;
	}
	file.flush();
	return out;

}



void QCircularBuffer::erase() {
	readPtr = writePtr = basePtr;
	writtenBytes.store(0);
}

#ifdef AXI_DMA

size_t QCircularBuffer::fromAxiDMA(AxiDMA& dma, size_t size) {

	size_t out = 0;
	bool tranferOut = 0;
	if(freeSpace() < size) { dbg() << "no space"; return out; }
	// need to split writing
	if ((writePtr + size) == end()) {
		tranferOut = dma.simpleTransferS2MM((UINTPTR)writePtr, size);
		if(!tranferOut) { dbg() << "fromAxiDMA failed 1 with size:" << size; return out; }
		out += size;
		writtenBytes.fetch_add(size);
		writePtr = basePtr;
	}

	else if((writePtr + size) > end()) {
		size = end() - writePtr;
		tranferOut = dma.simpleTransferS2MM((UINTPTR)writePtr, size);
		if(!tranferOut) { dbg() << "fromAxiDMA failed 2 with size:" << size; return out; }
		out += size;
		writtenBytes.fetch_add(size);
		writePtr = basePtr;
	}
	else {
		tranferOut = dma.simpleTransferS2MM((UINTPTR)writePtr, size);
		if(!tranferOut) { dbg() << "fromAxiDMA failed 3 with size:" << size; return out; }
		out += size;
		writtenBytes.fetch_add(size);
		writePtr += size;

	}
	return out;

}
size_t QCircularBuffer::toAxiDMA(AxiDMA& dma, size_t size) {

	size_t out = 0;
	bool transferOut = 0;
	size_t availableSize = usedSpace();
	if (availableSize == 0 || (availableSize % 4) != 0) { return out; }
	if (availableSize <= size) { size = availableSize; }
	if (size == 0 || (size % 4) != 0) { return out; }


	if(readPtr > writePtr) {
		if((readPtr + size) == end()) {
			transferOut = dma.simpleTransferMM2S((UINTPTR)readPtr, size);
			if(!transferOut) { dbg() << "simpleTransferMM2S1 failed\n"; return out; }

			out += size;
			writtenBytes.fetch_sub(size);
			readPtr = basePtr;
		}
		else if((readPtr + size) > end()) { // we need to pass data to DMA in two stage cause of separation of data in buffer

			size_t firstWriteCount = end() - readPtr;
			transferOut = dma.simpleTransferMM2S((UINTPTR)readPtr, firstWriteCount);
			if(!transferOut) { dbg() << "simpleTransferMM2S2 failed\n"; return out; }

			out += firstWriteCount;
			writtenBytes.fetch_sub(firstWriteCount);
			readPtr = basePtr;
		}
		else {
			transferOut = dma.simpleTransferMM2S((UINTPTR)readPtr, size);
			if(!transferOut) { dbg() << "simpleTransferMM2S4 failed\n"; return out; }

			out += size;
			writtenBytes.fetch_sub(size);
			readPtr += size;
		}
	}
	else {
		transferOut = dma.simpleTransferMM2S((UINTPTR)readPtr, size);
		if(!transferOut) { dbg() << "simpleTransferMM2S5 failed\n"; return out; }

		out += size;
		writtenBytes.fetch_sub(size);
		readPtr += size;
	}
	return out;
}

bool QCircularBuffer::getU8(quint8& out) {
	if(isEmpty()) { dbg() << "empty buffer"; return false; }
	out = *readPtr;
	writtenBytes.fetch_sub(1);
	readPtr += 1;
	return true;
}

bool QCircularBuffer::getU32(quint32& out) {
	if(usedSpace() < 4) { dbg() << "not enough available data in buffer"; return false; }

	quint8* ptr = (quint8*)&out;

	if(!getU8(ptr[0])) { dbg() << "read failed"; return false; }
	if(!getU8(ptr[1])) { dbg() << "read failed"; return false; }
	if(!getU8(ptr[2])) { dbg() << "read failed"; return false; }
	if(!getU8(ptr[3])) { dbg() << "read failed"; return false; }

	return true;
}

bool QCircularBuffer::getU64(quint64& out) {
	if(usedSpace() < 8) { dbg() << "not enough available data in buffer"; return false; }

	quint8* ptr = (quint8*)&out;

	if(!getU8(ptr[0])) { dbg() << "read failed"; return false; }
	if(!getU8(ptr[1])) { dbg() << "read failed"; return false; }
	if(!getU8(ptr[2])) { dbg() << "read failed"; return false; }
	if(!getU8(ptr[3])) { dbg() << "read failed"; return false; }

	if(!getU8(ptr[4])) { dbg() << "read failed"; return false; }
	if(!getU8(ptr[5])) { dbg() << "read failed"; return false; }
	if(!getU8(ptr[6])) { dbg() << "read failed"; return false; }
	if(!getU8(ptr[7])) { dbg() << "read failed"; return false; }

	return true;
}

#endif


