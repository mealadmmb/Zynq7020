#include <AxiDMA.h>
#ifdef str
#undef str
#endif

#include <QDebug.h>
#if XPAR_XAXIDMA_NUM_INSTANCES > 0

static InterruptController* interruptController = InterruptController::getInstance();

void MM2SglobalInterruptHandler(void* arg) {
	AxiDMA* axiDmaInstance = (AxiDMA*) arg;
	axiDmaInstance->MM2SinterruptHandler();
}

void S2MMglobalInterruptHandler(void* arg) {
	AxiDMA* axiDmaInstance = (AxiDMA*) arg;
	axiDmaInstance->S2MMinterruptHandler();
}

QDebug AxiDMA::dbg() {
	QDebug out(dbgEnable);
	out << "AxiDMA" << deviceID << ":";
	return out;
}

bool AxiDMA::init() {

	int Status;
	axiDmaConfig = XAxiDma_LookupConfig(deviceID);
	if (!axiDmaConfig) {
		dbg() << "No config found";
		return false;
	}

	/* Initialize DMA engine */
	Status = XAxiDma_CfgInitialize(&instance, axiDmaConfig);

	if (Status != XST_SUCCESS) {
		dbg() << "Initialization failed";
		return false;
	}

	if(XAxiDma_HasSg(&instance)){
		dbg() << "Device configured as SG mode";
		return false;
	}

	if(MM2SInterruptId > 0 && interruptCB) {

		interruptController->setTriggerType(MM2SInterruptId, InterruptController::risingEdge);
		interruptController->setPriority(MM2SInterruptId, 240);
		interruptController->connectHandler(MM2SInterruptId, (Xil_ExceptionHandler)MM2SglobalInterruptHandler, this);
		interruptController->enableIinterrupt(MM2SInterruptId);

		XAxiDma_IntrDisable(&instance, XAXIDMA_IRQ_ALL_MASK,
				XAXIDMA_DMA_TO_DEVICE);
		XAxiDma_IntrEnable(&instance, XAXIDMA_IRQ_ALL_MASK,
				XAXIDMA_DMA_TO_DEVICE);
	}

	if(S2MMInterruptId > 0 && interruptCB) {

		interruptController->setTriggerType(S2MMInterruptId, InterruptController::risingEdge);
		interruptController->setPriority(S2MMInterruptId, 240);
		interruptController->connectHandler(S2MMInterruptId, (Xil_ExceptionHandler)S2MMglobalInterruptHandler, this);
		interruptController->enableIinterrupt(S2MMInterruptId);

		XAxiDma_IntrDisable(&instance, XAXIDMA_IRQ_ALL_MASK,
				XAXIDMA_DEVICE_TO_DMA);
		XAxiDma_IntrEnable(&instance, XAXIDMA_IRQ_ALL_MASK,
				XAXIDMA_DEVICE_TO_DMA);
	}
	return true;
}



void AxiDMA::MM2SinterruptHandler() {
	u32 IrqStatus;
	int TimeOut = 10000;

	/* Read pending interrupts */
	IrqStatus = XAxiDma_IntrGetIrq(&instance, XAXIDMA_DMA_TO_DEVICE);

	/* Acknowledge pending interrupts */


	XAxiDma_IntrAckIrq(&instance, IrqStatus, XAXIDMA_DMA_TO_DEVICE);

	/*
	 * If no interrupt is asserted, we do not do anything
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		dbg() << "useless interrupt calling";
		return;
	}

	/*
	 * If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		dbg() << "error happened in MM2S interrupt, reseting Interrupt..";

		/*
		 * Reset should never fail for transmit channel
		 */
		XAxiDma_Reset(&instance);

		while (TimeOut) {
			if (XAxiDma_ResetIsDone(&instance)) {
				dbg() << "Interrupt was reset";
				errorInterrruptHandler();
				return;
			}
			TimeOut -= 1;
		}
	}

	/*
	 * If Completion interrupt is asserted, then set the TxDone flag
	 */
	if ((IrqStatus & XAXIDMA_IRQ_IOC_MASK)) {


		AxiDMAInterruptArg arg {AxiDMAInterruptType::MM2SDone};
		interruptCB(arg);
	}
	else
		dbg() << "no complete interrupt assertion";
}

void AxiDMA::S2MMinterruptHandler() {

	u32 IrqStatus;
	int TimeOut = 10000;

	/* Read pending interrupts */
	IrqStatus = XAxiDma_IntrGetIrq(&instance, XAXIDMA_DEVICE_TO_DMA);

	/* Acknowledge pending interrupts */
	XAxiDma_IntrAckIrq(&instance, IrqStatus, XAXIDMA_DEVICE_TO_DMA);

	/*
	 * If no interrupt is asserted, we do not do anything
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		return;
	}

	/*
	 * If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {
		dbg() << "error happened in MM2S interrupt, reseting Interrupt.." <<
				QString::number(IrqStatus & XAXIDMA_IRQ_ERROR_MASK, 16).toUpper();

		/* Reset could fail and hang
		 * NEED a way to handle this or do not call it??
		 */
		XAxiDma_Reset(&instance);

		while (TimeOut) {
			if (XAxiDma_ResetIsDone(&instance)) {
				dbg() << "Interrupt was reset";
				errorInterrruptHandler();
				return;
			}
			TimeOut -= 1;
		}
	}

	/*
	 * If completion interrupt is asserted, then set RxDone flag
	 */
	if ((IrqStatus & XAXIDMA_IRQ_IOC_MASK)) {

		dbg() << "calling user interrupt callback with S2MMDone flag";
		AxiDMAInterruptArg arg {AxiDMAInterruptType::S2MMDone};
		interruptCB(arg);
	}
}

bool AxiDMA::simpleTransferMM2S(QDataBuffer& buffer) {
	return simpleTransferMM2S((UINTPTR)buffer.getPtr(), buffer.getBufferedCount());
}

bool AxiDMA::isTxBussy() {
	bool out = false;
	if(!(XAxiDma_ReadReg(instance.TxBdRing.ChanBase,
			XAXIDMA_SR_OFFSET) & XAXIDMA_HALTED_MASK)) {
		if (XAxiDma_Busy(&instance,XAXIDMA_DMA_TO_DEVICE))
			out = true;
	}
	return out;
}

bool AxiDMA::isRxBussy() {
	bool out = false;
	if(!(XAxiDma_ReadReg(instance.TxBdRing.ChanBase,
			XAXIDMA_SR_OFFSET) & XAXIDMA_HALTED_MASK)) {
		if (XAxiDma_Busy(&instance,XAXIDMA_DEVICE_TO_DMA))
			out = true;
	}
	return out;
}

void AxiDMA::errorInterrruptHandler() {
	dbg() << "errorInterrruptHandler";
}





bool AxiDMA::simpleTransferMM2S(UINTPTR bufferPtr, u32 bufferSize) {

	/* Flush the SrcBuffer before the DMA transfer, in case the Data Cache
	 * is enabled
	 */
	Xil_DCacheFlushRange((UINTPTR)bufferPtr, bufferSize);
#ifdef __aarch64__
	Xil_DCacheFlushRange((UINTPTR)bufferPtr, bufferPtr);
#endif

	int out = XAxiDma_SimpleTransfer(&instance,(UINTPTR) bufferPtr,
			bufferSize, XAXIDMA_DMA_TO_DEVICE);
	if (out != XST_SUCCESS) {
		dbg() << "simpleTransferMM2S error: " << out;
	}
	return (out == XST_SUCCESS);
}

bool AxiDMA::simpleTransferS2MM(UINTPTR bufferPtr, u32 bufferSize) {

	/* Flush the SrcBuffer before the DMA transfer, in case the Data Cache
	 * is enabled
	 */
	Xil_DCacheFlushRange((UINTPTR)bufferPtr, bufferSize);
#ifdef __aarch64__
	Xil_DCacheFlushRange((UINTPTR)bufferPtr, bufferPtr);
#endif
	int out = XAxiDma_SimpleTransfer(&instance,(UINTPTR) bufferPtr,
			bufferSize, XAXIDMA_DEVICE_TO_DMA);

	if (out != XST_SUCCESS) {
		dbg() << "simpleTransferS2MM error: " << out;
	}
	return (out == XST_SUCCESS);
}

bool AxiDMA::simpleTransferMM2S(QDataContainer& container) {
	return simpleTransferMM2S((UINTPTR)container.ptr, container.size);

}
#endif
