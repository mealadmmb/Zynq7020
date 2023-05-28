#include "QString.h"
#include "assert.h"
#include "interrupt.h"
#include <QHash.h>
#include <QTime.h>
#include <QTimer.h>
#include <UartPs.h>
#if XPAR_XUARTPS_NUM_INSTANCES > 0

static InterruptController* interruptController = InterruptController::getInstance();
static QTimer* receiverVerifierTimer = NULL;
static QHash <XUartPs* /* XUartPs instance */ , UartPs* /* xUart class instance*/ > xUartHash;


void xUartInstancesReceiveVeriferTimerCb(void) {

	receiverVerifierTimer->stop();
	for(const auto& it : xUartHash) {
		it->finishReceiving();
	}
	receiverVerifierTimer->start();
}

void xUartGlobalInterruptHandler (void *CallBackRef, u32 Event, unsigned int EventData) {

	UartPs* uartPsInObj = (UartPs*)CallBackRef;
	uartPsInObj->internalGlobalInterruptHandler(Event, EventData);

}

UartPs::UartPs(quint32 deviceID, qint32 interruptID, quint32 baudRate) :
						xUartPsDeviceID(deviceID),
						xUartPsInterruptID(interruptID),
						baudRate(baudRate)
{
#if XUART_DEBUG_ENABLE
	std::cout << "constructing xUart device: " << deviceID << " in " << baudRate << "baudrate\r\n";
#endif



}

bool UartPs::changeBaudRate(quint32 newBaudRate) {
	bool res = false;
	res = (XUartPs_SetBaudRate(&xUartPsInstance, newBaudRate) == XST_SUCCESS);
	if(res) {
		baudRate = getBaudrate();
		timeOfOneUartFrameNs = ((1.0 / getBaudrate()) * 10) * 1e9;
	}
	return res;
}

bool UartPs::init(bool interruptEnable,
		std::function<void(const xUartInterruptArg&)> userInterruptCb, int timeoutMs) {

	bool out = false;
	Config = XUartPs_LookupConfig(xUartPsDeviceID);
	if (NULL == Config) return false;
	if (XUartPs_CfgInitialize(&xUartPsInstance, Config, Config->BaseAddress) != XST_SUCCESS) out = false;
	if (XUartPs_SetBaudRate(&xUartPsInstance, baudRate) != XST_SUCCESS) out = false;
	if (XUartPs_SelfTest(&xUartPsInstance) != XST_SUCCESS) out = false;
	out = true;
	timeOfOneUartFrameNs = ((1.0 / getBaudrate()) * 10) * 1e9;

	xUartPsIntEn = interruptEnable;
	if(xUartPsIntEn && userInterruptCb != nullptr && xUartPsInterruptID >= 0) {
		if(!interruptController->isReady()) return false;
		if(interruptController->connectHandler(
				xUartPsInterruptID,
				(Xil_ExceptionHandler)XUartPs_InterruptHandler,
				&xUartPsInstance) != XST_SUCCESS) return false;
		interruptController->enableIinterrupt(xUartPsInterruptID);

		XUartPs_SetRecvTimeout(&xUartPsInstance, 0);
		XUartPs_SetFifoThreshold(&xUartPsInstance, 63);

		XUartPs_SetHandler(&xUartPsInstance, (XUartPs_Handler)xUartGlobalInterruptHandler, this);

		/*
		 * Enable the interrupt of the UART so interrupts will occur, setup
		 * a local loopback so data that is sent will be received.
		 */
		u32 IntrMask = XUARTPS_IXR_RXOVR;

		if (xUartPsInstance.Platform == XPLAT_ZYNQ_ULTRA_MP) {
			IntrMask |= XUARTPS_IXR_RBRK;
		}

		XUartPs_SetInterruptMask(&xUartPsInstance, IntrMask);

		this->userInterruptCb = userInterruptCb;


		if(receiverVerifierTimer == NULL && timeoutMs) {
			receiverVerifierTimer = new QTimer;
			receiverVerifierTimer->setInterval(timeoutMs);
			receiverVerifierTimer->connect(xUartInstancesReceiveVeriferTimerCb);
			receiverVerifierTimer->create();
			receiverVerifierTimer->start();
		}
		out = true;
	}
	return out;
}


void UartPs::internalGlobalInterruptHandler(u32 Event, unsigned int EventData) {
	/* All of the data has been sent */
	if (Event == XUARTPS_EVENT_SENT_DATA) {
		internalSendInterruptHandler(Event, EventData);
	}

	/* All of the data has been received */
	else if (Event == XUARTPS_EVENT_RECV_DATA) {
		internalReceiveInterruptHandler(Event, EventData);
	}

	/* in case of any other interrupt */
	else internalErrorInterruptHandler(Event, EventData);

}

void UartPs::internalSendInterruptHandler(u32 Event, unsigned int EventData) {

	if(sendPending) {
		bytesSent += EventData;
		if(bytesSent >= bytesToSend) {
			xUartInterruptArg arg { sentDone, NULL, bytesSent };
			this->userInterruptCb(arg);
			bytesToSend = 0;
			bytesSent = 0;
			sendPending = false;
		}
	}
}

void UartPs::internalReceiveInterruptHandler(u32 Event, unsigned int EventData) {

	if(receivePending) {
		lastReceiveNs = QTime::nowNs();
		bytesReceived += EventData;
		if(bytesReceived >= bytesToReceive) {
			//receive finished
			xUartInterruptArg arg { receiveDone, receiveBuffer.get(), bytesReceived };
			receivePending = false;
			bytesToReceive = 0;
			bytesReceived = 0;


			this->userInterruptCb(arg);
		}
		else if(receiveBuffer) {

			//receiveThreshold = (bytesToReceive - receiveBufferIndex) * timeOfOneByte + 1;
			XUartPs_Recv(&xUartPsInstance, (u8*) (receiveBuffer.get() + bytesReceived), bytesToReceive - bytesReceived - 1);
		}
	}
}

void UartPs::internalErrorInterruptHandler(u32 Event, unsigned int EventData) {
	printf("event: %08lx, eventData: %08x\r\n", Event, EventData);
}

UartPs::~UartPs() {
	if(xUartPsIntEn) {
		if(xUartHash.contains(&xUartPsInstance)) {
			xUartHash.remove(&xUartPsInstance);
		}
	}
	if(xUartHash.isEmpty() && receiverVerifierTimer) {
		receiverVerifierTimer->stop();
		delete receiverVerifierTimer;
	}
}

int UartPs::receive(const void * data, size_t maxSize) {

	int out = -1;
	if(xUartPsIntEn == false && data) {
		out = XUartPs_Recv(&xUartPsInstance, (u8*) data, maxSize);
	}
	else{
		bytesToReceive = maxSize;
		bytesReceived = 0;
		receiveBuffer.reset(new quint8[bytesToReceive]);
		receivePending = true;
		receiveThresholdNs = 10 * timeOfOneUartFrameNs;

		char c = 0;
		XUartPs_Recv(&xUartPsInstance, (u8*) &c, 0); // call rec with zero size to stop previous receiving


		out = XUartPs_Recv(&xUartPsInstance, (u8*) (receiveBuffer.get() + bytesReceived), bytesToReceive);
	}
	return out;
}

int UartPs::send(const void* data, size_t size) {
	int out = -1;
	if(xUartPsIntEn == false) {
		out = XUartPs_Send(&xUartPsInstance, (u8*) data, size);
	}
	else{
		bytesToSend = size;
		bytesSent = 0;
		sendBuffer.reset(new quint8[bytesToSend]);
		memcpy(sendBuffer.get(), data, bytesToSend);
		sendPending = true;

		char c = 0;
		XUartPs_Send(&xUartPsInstance, (u8*) &c, 0);// call send with zero size to stop previous sending
		out = XUartPs_Send(&xUartPsInstance, (u8*) sendBuffer.get(), size);
	}
	return out;
}

void UartPs::finishReceiving() {

	if((QTime::nowNs() > (lastReceiveNs + receiveThresholdNs)) && receivePending && bytesReceived) {

		char c = 0;
		XUartPs_Recv(&xUartPsInstance, (u8*) &c, 0); // call rec with zero size to stop previous receiving
		interruptController->disableIinterrupt(xUartPsInterruptID);

		xUartInterruptArg arg { receiveDone, receiveBuffer.get(), bytesReceived };
		receivePending = false;
		bytesToReceive = 0;
		bytesReceived = 0;
		this->userInterruptCb(arg);
		interruptController->enableIinterrupt(xUartPsInterruptID);
	}
}

#endif
