#pragma once
#include "xparameters.h"
#if XPAR_XDMA_FIFO_NUM_INSTANCES > 0

#include "xil_types.h"
#include <cstdlib>
#include <functional>
#include <xdma_fifo.h>
#include <interrupt.h>

class xDmaFIFO {
	friend void dmaManagerDoneInterruptHandler(void *param);
	friend void dmaManagerHalfInterruptHandler(void *param);
public:

	xDmaFIFO(
			int deviceID,
			std::function<void(void*)> halfHandler,
			std::function<void(void*)> doneHandler,
			int halfInterruptID,
			int doneInterruptID);
	~xDmaFIFO() = default;
	int init();
	void start();
	void stop();

	inline void clearIntrrupt(){
		XDma_fifo_InterruptClear(&dmaInstance, 0xffffffff);
	}


private:

	int initDMA();
	int initInterrupts();

	XDma_fifo dmaInstance;
	int deviceID;
	std::function<void(void*)> halfInterruptHandler;
	std::function<void(void*)> doneInterruptHandler;
	int halfInterruptID;
	int doneInterruptID;
	volatile bool started = false;

};

#endif
