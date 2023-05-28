#pragma once
#include "xparameters.h"
#if XPAR_XAXIDMA_NUM_INSTANCES > 0
#include "xaxidma.h"
#include <interrupt.h>
#include <QDataBuffer.h>
#include <functional>



class QDebug;
class AxiDMA {
	friend void MM2SglobalInterruptHandler(void* arg);
	friend void S2MMglobalInterruptHandler(void* arg);
	friend class QCircularBuffer;
public:

	enum AxiDMAInterruptType {
		MM2SDone,
		S2MMDone
	};

	struct AxiDMAInterruptArg {
		AxiDMAInterruptType type;
	};

	AxiDMA(
			int deviceId,
			int mm2sIntID = -1,
			int s2mmIntID = -1,
			std::function<void (const AxiDMA::AxiDMAInterruptArg&)> intCB = nullptr,
			bool debugEnable = false):
				deviceID(deviceId),
				MM2SInterruptId(mm2sIntID),
				S2MMInterruptId(s2mmIntID),
				interruptCB(intCB),
				dbgEnable(debugEnable) {}
	bool init();
	bool simpleTransferMM2S(UINTPTR bufferPtr, u32 bufferSize);
	bool simpleTransferMM2S(QDataBuffer& buffer);
	bool simpleTransferMM2S(QDataContainer& container);


	bool simpleTransferS2MM(UINTPTR bufferPtr, u32 bufferSize);

	bool isInited() { return inited; }
	bool isTxBussy();
	bool isRxBussy();


private:

	QDebug dbg();
	void MM2SinterruptHandler();
	void S2MMinterruptHandler();
	void errorInterrruptHandler();

	const int deviceID;
	const int MM2SInterruptId;
	const int S2MMInterruptId;
	bool inited = false;

	// user never should start any transfer in this interrupt routine
	const std::function<void (const AxiDMA::AxiDMAInterruptArg&)> interruptCB;
	const bool dbgEnable;
	XAxiDma instance;
	XAxiDma_Config* axiDmaConfig;

};



#endif
