



#include <xDmaFIFO.h>
#if XPAR_XDMA_FIFO_NUM_INSTANCES > 0
#include <GPIO/gpio.h>
#include "xparameters.h"
#include "assert.h"
#include <QHash.h>

static InterruptController* interruptController = InterruptController::getInstance();
static QHash <int /* Interrupt ID*/ , std::function<void(void*)> /* Interrupt Routine*/ > dmaManagerHash;



void dmaManagerDoneInterruptHandler(void *param)
{
	xDmaFIFO* dmaObj = (xDmaFIFO*) param;
	dmaObj->started = false;
	dmaObj->doneInterruptHandler(nullptr);
}

void dmaManagerHalfInterruptHandler(void *param)
{
	xDmaFIFO* dmaObj = (xDmaFIFO*) param;
	dmaObj->started = false;
	dmaObj->halfInterruptHandler(nullptr);
}



xDmaFIFO::xDmaFIFO(
		int deviceID,
		std::function<void(void*)> halfHandler,
		std::function<void(void*)> doneHandler,
		int halfInterruptID,
		int doneInterruptID) :
		deviceID(deviceID),
		halfInterruptHandler(halfHandler),
		doneInterruptHandler(doneHandler),
		halfInterruptID(halfInterruptID),
		doneInterruptID(doneInterruptID)

{}

int xDmaFIFO::init() {

	int stat = XST_FAILURE;
	stat = initDMA();
	assert(stat == XST_SUCCESS);

	stat = initInterrupts();
	assert(stat == XST_SUCCESS);
	return stat;
}

int xDmaFIFO::initInterrupts() {
	int status;

	// Connect half and done interrupt to handler
	if(halfInterruptID > 0 && halfInterruptHandler){

		status = interruptController->connectHandler(
				halfInterruptID,
				(Xil_ExceptionHandler) dmaManagerHalfInterruptHandler,
				(void*) this);
		if(status != XST_SUCCESS) return 0x12;

		// Set interrupt type of half and done to rising edge
		interruptController->setTriggerType(halfInterruptID, InterruptController::risingEdge);

		// Enable half and done interrupts in the controller
		interruptController->enableIinterrupt(halfInterruptID);
	}


	if(doneInterruptID > 0 && doneInterruptHandler){
		status = interruptController->connectHandler(
				doneInterruptID,
				(Xil_ExceptionHandler) dmaManagerDoneInterruptHandler,
				(void*) this);
		if(status != XST_SUCCESS) return 0x13;

		// Set interrupt type of half and done to rising edge
		interruptController->setTriggerType(doneInterruptID, InterruptController::risingEdge);

		// Enable half and done interrupts in the controller
		interruptController->enableIinterrupt(doneInterruptID);
	}
	return XST_SUCCESS;
}

int xDmaFIFO::initDMA() {
	int status = XDma_fifo_Initialize(&dmaInstance, deviceID);
	if (status != XST_SUCCESS) {
		return status;
	}

	XDma_fifo_InterruptGlobalDisable(&dmaInstance);
	XDma_fifo_InterruptDisable(&dmaInstance, 0x0F);

	XDma_fifo_InterruptGlobalEnable(&dmaInstance);
	XDma_fifo_InterruptEnable(&dmaInstance, 0xFFFFFFFF);

	return XST_SUCCESS;
}


void xDmaFIFO::start() {

	XDma_fifo_Start(&dmaInstance);
	started = true;
}

void xDmaFIFO::stop() {
	XDma_fifo_DisableAutoRestart(&dmaInstance);
	started = false;
}

#endif
