#include <xCan.h>

#if XPAR_XCANPS_NUM_INSTANCES > 0
#include "interrupt.h"
#include <QHash.h>
#include <QTime.h>
#include <QTimer.h>

static InterruptController* interruptController = InterruptController::getInstance();
static QHash<XCanPs*, xCanPs*> xCanHandlersHash;

void globalCanSendHandler(void *callBackRef) {
	XCanPs* in = (XCanPs*)callBackRef;
	if(xCanHandlersHash.contains(in)) { xCanHandlersHash[in]->internalSendHandler(); }
}
void globalCanReceiveHandler(void *callBackRef) {
	XCanPs* in = (XCanPs*)callBackRef;
	if(xCanHandlersHash.contains(in)) { xCanHandlersHash[in]->internalReceiveHandler(); }

}
void globalCanErrorHandler(void *callBackRef, u32 ErrorMask) {
	XCanPs* in = (XCanPs*)callBackRef;
	if(xCanHandlersHash.contains(in)) { xCanHandlersHash[in]->internalErrorHandler(ErrorMask); }

}
void globalCanEventHandler(void *callBackRef, u32 IntrMask) {
	XCanPs* in = (XCanPs*)callBackRef;
	if(xCanHandlersHash.contains(in)) { xCanHandlersHash[in]->internalEventHandler(IntrMask); }

}


xCanPs::xCanPs(quint32 _deviceID, quint32 _clock, qint32 _interruptID, bool _debugEnable) :
		deviceID(_deviceID),
		xCanClock(_clock),
		interruptID(_interruptID),
		debugEnable(_debugEnable)
{}

bool xCanPs::init(quint32 _bitRate, bool _interrupEnable,
		std::function<void(const InterruptArg&)> _userCb) {

	userCb = _userCb;
	int Status;
	XCanPs_Config *ConfigPtr;

	/*
	 * Initialize the Can device.
	 */
	ConfigPtr = XCanPs_LookupConfig(deviceID);
	if (ConfigPtr == NULL) {
		dbg() << "XCanPs_LookupConfig failed";
		return (XST_FAILURE == XST_SUCCESS);
	}
	XCanPs_CfgInitialize(&instance,
			ConfigPtr,
			ConfigPtr->BaseAddr);

	/*
	 * Run self-test on the device, which verifies basic sanity of the
	 * device and the driver.
	 */
	Status = XCanPs_SelfTest(&instance);
	if (Status != XST_SUCCESS) {
		dbg() << "XCanPs_SelfTest failed";
		return (XST_FAILURE == XST_SUCCESS);
	}


	/*
	 * Enter Configuration Mode if the device is not currently in
	 * Configuration Mode.
	 */
	XCanPs_EnterMode(&instance, XCANPS_MODE_CONFIG);
	while(XCanPs_GetMode(&instance) != XCANPS_MODE_CONFIG);

	/*
	 * Setup Baud Rate Prescaler Register (BRPR) and
	 * Bit Timing Register (BTR).
	 */

	std::optional<BitRateCoefs> coefs = extractCoefs(_bitRate);
	if(coefs) {
		bitRateCoefs = *coefs;
		XCanPs_SetBaudRatePrescaler(&instance, bitRateCoefs.BRPR);
		XCanPs_SetBitTiming(&instance, bitRateCoefs.SJW,
				bitRateCoefs.TS2,
				bitRateCoefs.TS1);
	}
	else { return false; }


	if(_interrupEnable && interruptID && userCb) {

		/*
		 * Set interrupt handlers.
		 */
		XCanPs_SetHandler(&instance, XCANPS_HANDLER_SEND,
				(void *)globalCanSendHandler, (void *)&instance);
		XCanPs_SetHandler(&instance, XCANPS_HANDLER_RECV,
				(void *)globalCanReceiveHandler, (void *)&instance);
		XCanPs_SetHandler(&instance, XCANPS_HANDLER_ERROR,
				(void *)globalCanErrorHandler, (void *)&instance);
		XCanPs_SetHandler(&instance, XCANPS_HANDLER_EVENT,
				(void *)globalCanEventHandler, (void *)&instance);


		if(!interruptController->isReady()) return false;
		if(interruptController->connectHandler(
				interruptID,
				(Xil_ExceptionHandler)XCanPs_IntrHandler,
				&instance) != XST_SUCCESS) return false;
		interruptController->setPriority(interruptID, 0);
		interruptController->setTriggerType(interruptID,InterruptController::risingEdge);
		interruptController->enableIinterrupt(interruptID);
		xCanHandlersHash.insert(&instance, this);


		/*
		 * Enable all interrupts in CAN device.
		 */
		XCanPs_IntrEnable(&instance, XCANPS_IXR_ALL);

	}


	/*
	 * Enter NORMAL Mode.
	 */
	XCanPs_EnterMode(&instance, XCANPS_MODE_NORMAL);
	while(XCanPs_GetMode(&instance) != XCANPS_MODE_NORMAL);
	return true;
}

std::optional<xCanPs::BitRateCoefs> xCanPs::extractCoefs(quint32 desiredBitrate) {

	BitRateCoefs coefs;
	bool coefsExtracted = false;
	for(coefs.BRPR = 0; coefs.BRPR < 255 && !coefsExtracted; coefs.BRPR++) {
		for(coefs.TS1 = 0; coefs.TS1 < 15 && !coefsExtracted; coefs.TS1++) {
			for(coefs.TS2 = 0; coefs.TS2 < 7 && !coefsExtracted; coefs.TS2++) {
				if((xCanClock / ((coefs.BRPR + 1) * (3 + coefs.TS1 + coefs.TS2))) == desiredBitrate) {
					coefsExtracted = true;
					break;
				}
			}
			if (coefsExtracted) break;
		}
		if (coefsExtracted) break;
	}

	if(coefsExtracted) {
		dbg() <<
				"coefs extracted for bitrate: " << desiredBitrate <<
				". coefs:" <<
				"BRPR:" << QString::number(coefs.BRPR) <<
				", TS1:" << QString::number(coefs.TS1) <<
				", TS2:" << QString::number(coefs.TS2);

		return coefs;

	} else { dbg() << "nothing found for bitrate:" << desiredBitrate; return {}; }
}

bool xCanPs::send(FrameID ID, const Payload& paylaod) {

	quint8 *FramePtr;
	int Index;
	int Status;

	/*
	 * Create correct values for Identifier and Data Length Code Register.
	 */
	TxFrame[0] = ID;
	TxFrame[1] = (u32)XCanPs_CreateDlcValue((u32)paylaod.size);

	/*
	 * Now fill in the data field with known values so we can verify them
	 * on receive.
	 */
	FramePtr = (quint8 *)(&TxFrame[2]);
	for (Index = 0; Index < paylaod.size; Index++) {
		*FramePtr++ = paylaod.data[Index];
	}

	/*
	 * Now wait until the TX FIFO is not full and send the frame.
	 */
	while (XCanPs_IsTxFifoFull(&instance) == TRUE);

	Status = XCanPs_Send(&instance, TxFrame);
	if (Status != XST_SUCCESS) {
		/*
		 * The frame could not be sent successfully.
		 */
		return false;
	}
	return true;
}

void xCanPs::internalSendHandler() {
	InterruptArg out;
	out.type = InterruptType::sentDone;
	userCb(out);
}

void xCanPs::internalReceiveHandler() {
	ReceiveFrame out;
	if(XCanPs_Recv(&instance, RxFrame) == XST_SUCCESS) {
		XCanPs_ClearTimestamp(&instance);

		out.sender = RxFrame[0];
		out.timestamp = RxFrame[1] & XCANPS_DLCR_TIMESTAMP_MASK;

		out.payload.size = (RxFrame[1] & XCANPS_DLCR_DLC_MASK) >> XCANPS_DLCR_DLC_SHIFT;
		memcpy(out.payload.data, &RxFrame[2], out.payload.size);

		InterruptArg arg { receiveDone, out };
		userCb(arg);
	}
}

void xCanPs::internalErrorHandler(u32 ErrorMask) {
	if(ErrorMask & XCANPS_ESR_ACKER_MASK) {
		dbg() << "Error : " << "XCANPS_ESR_ACKER_MASK";
	}

	else if(ErrorMask & XCANPS_ESR_BERR_MASK) {
		dbg() << "Error : " << "XCANPS_ESR_BERR_MASK";
	}

	else if(ErrorMask & XCANPS_ESR_STER_MASK) {
		dbg() << "Error : " << "XCANPS_ESR_STER_MASK";
	}

	else if(ErrorMask & XCANPS_ESR_FMER_MASK) {
		dbg() << "Error : " << "XCANPS_ESR_FMER_MASK";
	}

	else if(ErrorMask & XCANPS_ESR_CRCER_MASK) {
		dbg() << "Error : " << "XCANPS_ESR_CRCER_MASK";
	}
}

void xCanPs::internalEventHandler(u32 IntrMask) {

	if (IntrMask & XCANPS_IXR_BSOFF_MASK) {
		/*
		 * Entering Bus off status interrupt requires
		 * the CAN device be reset and reconfigured.
		 */
		XCanPs_Reset(&instance);
		XCanPs_EnterMode(&instance, XCANPS_MODE_CONFIG);
		while(XCanPs_GetMode(&instance) != XCANPS_MODE_CONFIG);

		/*
		 * Setup Baud Rate Prescaler Register (BRPR) and
		 * Bit Timing Register (BTR).
		 */
		XCanPs_SetBaudRatePrescaler(&instance, bitRateCoefs.BRPR);
		XCanPs_SetBitTiming(&instance, bitRateCoefs.SJW,
				bitRateCoefs.TS2,
				bitRateCoefs.TS1);
		dbg() << "Event : " << "XCANPS_IXR_BSOFF_MASK," << " CAN reseted and reconfigured!";
		return;
	}

	else if(IntrMask & XCANPS_IXR_RXOFLW_MASK) {
		dbg() << "Event : " << "XCANPS_IXR_RXOFLW_MASK";
	}

	else if(IntrMask & XCANPS_IXR_RXUFLW_MASK) {
		dbg() << "Event : " << "XCANPS_IXR_RXUFLW_MASK";
	}

	else if(IntrMask & XCANPS_IXR_TXBFLL_MASK) {
		dbg() << "Event : " << "XCANPS_IXR_TXBFLL_MASK";
	}

	else if(IntrMask & XCANPS_IXR_TXFLL_MASK) {
		dbg() << "Event : " << "XCANPS_IXR_TXFLL_MASK";
	}

	else if (IntrMask & XCANPS_IXR_WKUP_MASK) {
		dbg() << "Event : " << "XCANPS_IXR_WKUP_MASK";
	}

	else if (IntrMask & XCANPS_IXR_SLP_MASK) {
		dbg() << "Event : " << "XCANPS_IXR_SLP_MASK";
	}

	else if (IntrMask & XCANPS_IXR_ARBLST_MASK) {
		dbg() << "Event : " << "XCANPS_IXR_ARBLST_MASK";
	}
}
#endif
