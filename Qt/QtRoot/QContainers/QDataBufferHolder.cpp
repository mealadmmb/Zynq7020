#include <QDataBufferHolder.h>
#ifdef XSDK
QDataBufferHolder::QDataBufferHolder(size_t numberOfBuffers,
		size_t bufferSize) {

	for(size_t i = 0; i < numberOfBuffers; i++) {
		buffers.emplace_back(QDataBuffer(bufferSize));
		freeBufferIndexes.append(i);
	}
}

#ifdef LWIPSOCKET
bool QDataBufferHolder::fromQUdpSocket(QUdpSocket& sock, size_t maxlen) {

	if(freeBufferIndexes.isEmpty()) return false;

	//#ifdef QFREERTOS
	//	QMutexLocker locker(&mutex);
	//#endif
	size_t freeBufferIndex = freeBufferIndexes[0];

	auto& freeBuffer = buffers[freeBufferIndex];

	if(!freeBuffer.fromQUdpSocket(sock, maxlen)) return false;

	freeBufferIndexes.remove(freeBufferIndex);
	fullBufferIndexes.append(freeBufferIndex);

	return true;
}
#endif
#ifdef AXI_DMA
void QDataBufferHolder::toAxiDMA(AxiDMA& dma) {

	if(!fullBufferIndexes.isEmpty()) {
		//#ifdef QFREERTOS
		//	QMutexLocker locker(&mutex);
		//#endif
		size_t fullBufferIndex = fullBufferIndexes[0];

		auto& fullBuffer = buffers[fullBufferIndex];
		if(fullBuffer.toAxiDMA(dma)) {
			fullBufferIndexes.remove(fullBufferIndex);
			pendingTofreeIndexes.append(fullBufferIndex);
		}
		else {
			printf("a\n");
		}

	}
}
#endif

void QDataBufferHolder::freePendingBuffers() {

	//#ifdef QFREERTOS
	//	QMutexLocker locker(&mutex);
	//#endif

	auto copyOfPendingTofreeIndexes = pendingTofreeIndexes;
	for(auto& it : copyOfPendingTofreeIndexes) {
		buffers[it].reset();
		freeBufferIndexes.append(it);
		pendingTofreeIndexes.remove(it);
	}
}

#endif
