
#include <UartLite.h>
#include "interrupt.h"
#if XPAR_XUARTLITE_NUM_INSTANCES > 0
#if XPAR_XSCUGIC_NUM_INSTANCES > 0
#include <QHash.h>
#include <QTimer.h>
#include <QTime.h>

static QTimer* receiverVerifierTimer = NULL;
static InterruptController* interruptController = InterruptController::getInstance();
static QHash <XUartLite* /* uartLite instance */ , UartLite* /* Interrupt Routine*/ > xUartLiteHash;


void xUartLiteInstancesReceiveVeriferTimerCb(void) {

	receiverVerifierTimer->stop();
	for(const auto& it : xUartLiteHash) {
		it->finishReceiving();
	}
	receiverVerifierTimer->start();
}



void xUartLiteGlobalSendHandler		(void *CallBackRef, unsigned int EventData) {
	XUartLite* xUartLiteInstance = (XUartLite* )CallBackRef;
	if(!xUartLiteHash.isEmpty()) {
		if(xUartLiteHash.contains(xUartLiteInstance)) {
			xUartLiteHash.value(xUartLiteInstance)->internalSendInterruptHandler(CallBackRef, EventData);
		}
	}
}
void xUartLiteGlobalReceiveHandler	(void *CallBackRef, unsigned int EventData) {
	XUartLite* xUartLiteInstance = (XUartLite* )CallBackRef;
	if(!xUartLiteHash.isEmpty()) {
		if(xUartLiteHash.contains(xUartLiteInstance)) {
			xUartLiteHash.value(xUartLiteInstance)->internalReceiveInterruptHandler(CallBackRef, EventData);
		}
	}
}



UartLite::UartLite(int deviceID, int interruptID) :
		devID(deviceID),
		interruptID(interruptID)
{}

bool UartLite::init(bool interruptEnable, std::function<void(xUartLiteInterruptArg*)> userInterruptCb, int timeOut) {

	int Status;

	/*
	 * Initialize the UartLite driver so that it's ready to use.
	 */
	Status = XUartLite_Initialize(&xUartLiteInstance, devID);
	if (Status != XST_SUCCESS) {
		return false;
	}

	xUartLiteConfig = XUartLite_LookupConfig(devID);
	if (xUartLiteConfig == NULL) {
		return false;
	}


	timeOfOneUartFrameNs = ((1.0 / getBaudrate()) * 10) * 1e9;



	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XUartLite_SelfTest(&xUartLiteInstance);
	if (Status != XST_SUCCESS) {
		return false;
	}

	intEn = interruptEnable;
	if(intEn) {
		if(userInterruptCb != NULL) {
			if(!interruptController->isReady()) return false;
			if(interruptController->connectHandler(
					interruptID,
					(Xil_ExceptionHandler)XUartLite_InterruptHandler,
					&xUartLiteInstance) != XST_SUCCESS) return false;
			interruptController->setPriority(interruptID, 0);
			interruptController->setTriggerType(interruptID, InterruptController::risingEdge);
			interruptController->enableIinterrupt(interruptID);

			/*
			 * Setup the handlers for the UartLite that will be called from the
			 * interrupt context when data has been sent and received, specify a
			 * pointer to the UartLite driver instance as the callback reference so
			 * that the handlers are able to access the instance data.
			 */
			XUartLite_SetSendHandler(&xUartLiteInstance, xUartLiteGlobalSendHandler, &xUartLiteInstance);
			XUartLite_SetRecvHandler(&xUartLiteInstance, xUartLiteGlobalReceiveHandler, &xUartLiteInstance);
			xUartLiteHash.insert(&xUartLiteInstance, this);
			this->userInterruptCb = userInterruptCb;

			/*
			 * Enable the interrupt of the UartLite so that interrupts will occur.
			 */
			XUartLite_EnableInterrupt(&xUartLiteInstance);

			if(timeOut) {
				if(receiverVerifierTimer == NULL ) {
					receiverVerifierTimer = new QTimer;
					receiverVerifierTimer->setInterval(timeOut);
					receiverVerifierTimer->connect(xUartLiteInstancesReceiveVeriferTimerCb);
					receiverVerifierTimer->create();
					receiverVerifierTimer->start();
				}
			}
		}
		else{
			printf("no userInterruptCb passed\r\n");
			return false;
		}
	}
	return true;
}

quint32 UartLite::getBaudrate() {
	if(xUartLiteConfig) {
		return xUartLiteConfig->BaudRate;
	}
	return 0;
}

void UartLite::internalSendInterruptHandler(void *CallBackRef, unsigned int EventData) {

	if(sendPending) {
		bytesSent += EventData;
		if(bytesSent >= bytesToSend) {
			xUartLiteInterruptArg arg { sentDone, NULL, bytesSent };
			this->userInterruptCb(&arg);
			bytesToSend = 0;
			bytesSent = 0;
			sendPending = false;
		}
	}
}

void UartLite::internalReceiveInterruptHandler(void *CallBackRef, unsigned int EventData) {


	if(receivePending) {
		lastReceiveNs = QTime::nowNs();
		bytesReceived += EventData;
		if(bytesReceived >= bytesToReceive) {
			//receive finished
			xUartLiteInterruptArg arg { receiveDone, receiveBuffer.get(), bytesReceived };
			receivePending = false;
			bytesToReceive = 0;
			bytesReceived = 0;


			this->userInterruptCb(&arg);
		}
		else {

			//receiveThreshold = (bytesToReceive - receiveBufferIndex) * timeOfOneByte + 1;
			XUartLite_Recv(&xUartLiteInstance, (u8*) (receiveBuffer.get() + bytesReceived), 1);
		}
	}
}

int UartLite::receive(quint8* buffer,size_t maxSize) {

	int out = -1;
	if(intEn == false) {
		XUartLite_ResetFifos(&xUartLiteInstance);
		out = XUartLite_Recv(&xUartLiteInstance, (u8*) buffer, maxSize);
	}
	else{
		bytesToReceive = maxSize;
		bytesReceived = 0;
		receiveBuffer.reset(new quint8[bytesToReceive]);
		receivePending = true;
		receiveThresholdNs = 10 * timeOfOneUartFrameNs;

		char c = 0;
		XUartLite_Recv(&xUartLiteInstance, (u8*) &c, 0); // call rec with zero size to stop previous receiving
		XUartLite_ResetFifos(&xUartLiteInstance);

		out = XUartLite_Recv(&xUartLiteInstance, (u8*) (receiveBuffer.get() + bytesReceived), 1);
	}
	return out;
}

int UartLite::send(const void* data, size_t size) {
	int out = -1;

	if(intEn == false) {
		XUartLite_ResetFifos(&xUartLiteInstance);
		out = XUartLite_Send(&xUartLiteInstance, (u8*) data, size);
	}
	else{
		bytesToSend = size;
		bytesSent = 0;
		sendBuffer.reset(new quint8[bytesToSend]);
		memcpy(sendBuffer.get(), data, bytesToSend);
		sendPending = true;

		char c = 0;
		XUartLite_Send(&xUartLiteInstance, (u8*) &c, 0);// call send with zero size to stop previous sending
		out =  XUartLite_Send(&xUartLiteInstance, (u8*) sendBuffer.get(), size);
	}
	return out;
}

UartLite::~UartLite() {
	if(intEn) {
		if(xUartLiteHash.contains(&xUartLiteInstance)) {
			xUartLiteHash.remove(&xUartLiteInstance);
		}
	}
	if(xUartLiteHash.isEmpty() && receiverVerifierTimer) {
		receiverVerifierTimer->stop();
		delete receiverVerifierTimer;
	}
}

void UartLite::finishReceiving() {
	if((QTime::nowNs() > (lastReceiveNs + receiveThresholdNs)) && receivePending && bytesReceived) {

		u8 c = 0;
		XUartLite_Recv(&xUartLiteInstance, &c, 1); // call rec with zero size to stop previous receiving
		interruptController->disableIinterrupt(interruptID);

		xUartLiteInterruptArg arg { receiveDone, receiveBuffer.get(), bytesReceived };
		receivePending = false;
		bytesToReceive = 0;
		bytesReceived = 0;
		this->userInterruptCb(&arg);
		interruptController->enableIinterrupt(interruptID);
	}
}

#endif
#endif
