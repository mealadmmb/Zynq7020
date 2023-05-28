#pragma once
#include "xparameters.h"
#include <QByteArray.h>
#include <functional>
#include <memory>
#if XPAR_XUARTPS_NUM_INSTANCES > 0
#include "xuartps.h"
#define DEFAULT_BAUDRATE 921600

class QString;
class UartPs {

public:

	enum  xUartInterruptType {
		receiveDone,
		sentDone
	};
	struct xUartInterruptArg {
		xUartInterruptType type;
		quint8* data;
		size_t size;
	};

	quint32 getBaudrate() { return xUartPsInstance.BaudRate; }
	bool changeBaudRate(uint32_t newBaudRate);

	UartPs(quint32 deviceID, qint32 interruptID = -1, quint32 baudRate = DEFAULT_BAUDRATE);
	~UartPs();
	bool init(bool interruptEnable = false, std::function<void(const xUartInterruptArg&)> userInterruptCb = nullptr, int timeoutMs = 0);
	bool isReady() { return (xUartPsInstance.IsReady == XIL_COMPONENT_IS_READY); }
	bool isSending() { return XUartPs_IsSending(&xUartPsInstance); }
	void internalGlobalInterruptHandler 	(u32 Event, unsigned int EventData);
	void finishReceiving();


	int receive(const void * data, size_t maxSize);
	int send(const void* data, size_t size);
	inline int send(const QByteArray& arr) { return send(arr.data(), arr.size()); }
private:

	std::function<void(const xUartInterruptArg&)> userInterruptCb;

	quint32 xUartPsDeviceID;
	qint32 xUartPsInterruptID;
	XUartPs xUartPsInstance;
	bool xUartPsIntEn = false;
	XUartPs_Config *Config;
	XUartPsFormat xUartPsFormat;
	quint32 baudRate;

	std::unique_ptr<quint8> sendBuffer;
	quint32 bytesToSend = 0;
	quint32 bytesSent = 0;
	bool sendPending = false;

	std::unique_ptr<quint8> receiveBuffer;
	quint32 bytesToReceive = 0;
	quint32 bytesReceived = 0;
	bool receivePending = false;

	quint64 lastReceiveNs = 0;
	quint64 receiveThresholdNs;
	quint64 timeOfOneUartFrameNs = 0;


	void internalSendInterruptHandler 		(u32 Event, unsigned int EventData);
	void internalReceiveInterruptHandler 	(u32 Event, unsigned int EventData);
	void internalErrorInterruptHandler 		(u32 Event, unsigned int EventData);



};

#endif
