#pragma once

#include <xparameters.h>
#include <functional>
#include <memory>
#include <QByteArray.h>
#if XPAR_XUARTLITE_NUM_INSTANCES > 0

#include "xuartlite.h"
#if XPAR_XSCUGIC_NUM_INSTANCES > 0
#include "xil_exception.h"
#include "xscugic.h"


enum  xUartLiteInterruptType {
	receiveDone,
	sentDone
};
struct xUartLiteInterruptArg {
	xUartLiteInterruptType type;
	quint8* data;
	size_t size;
};

#endif

class UartLite {
public:
	UartLite(int deviceID, int interruptID);
	~UartLite();
	bool init(bool interruptEnable = false, std::function<void(xUartLiteInterruptArg*)> userInterruptCb = nullptr, int timeOut = 0);

	quint32 getBaudrate();


	int receive(quint8* buffer, size_t maxSize);
	int send(const void* data, size_t size);
	inline int send(const QByteArray& arr) { return send(arr.data(), arr.size()); }
	bool isSending() { return (XUartLite_IsSending(&xUartLiteInstance) != 0); }


	void internalSendInterruptHandler (void*, unsigned int);
	void internalReceiveInterruptHandler (void*, unsigned int);
	void finishReceiving();


private:
	int devID;
	bool intEn = false;
	int interruptID;
	XUartLite_Config* xUartLiteConfig = NULL;
	XUartLite xUartLiteInstance;
	std::function<void(xUartLiteInterruptArg*)> userInterruptCb;



	std::unique_ptr<quint8> sendBuffer;
	size_t bytesToSend = 0;
	size_t bytesSent = 0;
	bool sendPending = false;

	std::unique_ptr<quint8> receiveBuffer;
	size_t bytesToReceive = 0;
	size_t bytesReceived = 0;
	bool receivePending = false;
	quint64 lastReceiveNs = 0;
	quint64 receiveThresholdNs;
	quint64 timeOfOneUartFrameNs = 0;

	int SetupInterruptSystem(XUartLite *UartLitePtr);

};

#endif
