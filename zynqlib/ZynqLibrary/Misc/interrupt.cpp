#include <interrupt.h>
#include "stdio.h"

#ifdef FREERTOS
extern XScuGic xInterruptController;
#endif

InterruptController::InterruptController() {

#ifdef FREERTOS
	XScuGicInstance = &xInterruptController;
#else
	XScuGicInstance = new XScuGic;
#endif
	int Status;
	if(!isReady()) {
		Config = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);
		if (NULL == Config) {
			printf("error in XScuGic_LookupConfig()\r\n");
			return;
		}

		Status = XScuGic_CfgInitialize(XScuGicInstance, Config,
				Config->CpuBaseAddress);
		if (Status != XST_SUCCESS) {
			printf("error in XScuGic_CfgInitialize()\r\n");
			return ;
		}

		/*
		 * Connect the interrupt controller interrupt handler to the
		 * hardware interrupt handling logic in the processor.
		 */
		Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler) XScuGic_InterruptHandler,
				XScuGicInstance);


		/* Enable interrupts */
		Xil_ExceptionEnable();
	}
}

int InterruptController::connectHandler(u32 interruptID,
		Xil_InterruptHandler interruptHandler,
		void *CallBackRef)
{
	if(isReady()) {
		int Status;
		/*
		 * Connect a device driver handler that will be called when an
		 * interrupt for the device occurs, the device driver handler
		 * performs the specific interrupt processing for the device
		 */
		Status = XScuGic_Connect(XScuGicInstance, interruptID,
				interruptHandler,
				CallBackRef);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		return XST_SUCCESS;
	}
	return XST_FAILURE;
}

//int InterruptController::setType(int interruptID, int interruptType) {
//
//	if(isReady()) {
//		int mask;
//		interruptType &= INT_TYPE_MASK;
//		mask = XScuGic_DistReadReg(XScuGicInstance, INT_CFG0_OFFSET + (interruptID/16)*4);
//		mask &= ~(INT_TYPE_MASK << (interruptID%16)*2);
//		mask |= interruptType << ((interruptID%16)*2);
//		XScuGic_DistWriteReg(XScuGicInstance, INT_CFG0_OFFSET + (interruptID/16)*4, mask);
//
//		printf("type: %X\r\n", INT_CFG0_OFFSET + (interruptID/16)*4);
//		return true;
//	}
//	return false;
//}

void InterruptController::enableIinterrupt(int interruptID) {

	/* Enable the interrupt for the device */
	if(isReady()) XScuGic_Enable(XScuGicInstance, interruptID);
}

void InterruptController::disableIinterrupt(int interruptID) {

	/* Disable the interrupt for the device */
	if(isReady()) XScuGic_Disable(XScuGicInstance, interruptID);
}

void InterruptController::setPriority(int interruptID, quint8 priority) {
	if(isReady()) {
		InterruptController::TriggerType trigger = getTriggerType(interruptID);
		XScuGic_SetPriorityTriggerType(XScuGicInstance, interruptID, priority, (quint8)trigger);
	}
}

quint8 InterruptController::getPriority(int interruptID) {
	quint8 out = 0;
	if(isReady()) {
		quint8 priority, trigger;
		XScuGic_GetPriorityTriggerType(XScuGicInstance, interruptID, &priority, &trigger);
		out = priority;
	}
	return out;
}

void InterruptController::setTriggerType(int interruptID, TriggerType tt) {
	if(isReady()) {
		quint8 priority = getPriority(interruptID);
		XScuGic_SetPriorityTriggerType(XScuGicInstance, interruptID, priority, (quint8)tt);
	}
}

InterruptController::TriggerType InterruptController::getTriggerType(int interruptID) {
	TriggerType out = TriggerType::invalid;
	if(isReady()) {
		quint8 priority, trigger;
		XScuGic_GetPriorityTriggerType(XScuGicInstance, interruptID, &priority, &trigger);
		out = (TriggerType)trigger;
	}
	return out;
}
