#pragma once
#include "xparameters.h"
#include <functional>
#include <optional>
#include <QtGlobal.h>
#include <QDebug.h>
#if XPAR_XCANPS_NUM_INSTANCES > 0
#include "xcanps.h"



class xCanPs {

	friend void globalCanSendHandler(void *callBackRef);
	friend void globalCanReceiveHandler(void *callBackRef);
	friend void globalCanErrorHandler(void *callBackRef, u32 ErrorMask);
	friend void globalCanEventHandler(void *callBackRef, u32 IntrMask);


	/* Maximum CAN frame length in word */
	static constexpr quint32 XCANPS_MAX_FRAME_SIZE_IN_WORDS = (XCANPS_MAX_FRAME_SIZE / sizeof(u32));

public:

	class FrameID {
		friend class xCanPs;
	public:

		FrameID() :
			standardID(0),
			substituteRemoteTXReq(0),
			extensionID(0),
			extendedID(0),
			remoteTXRequest(0)
	{}

		// TxRxFrame0 MUST be true 32bit value
		FrameID(quint32 TxRxFrame0) :
			standardID((TxRxFrame0 & XCANPS_IDR_ID1_MASK) >> XCANPS_IDR_ID1_SHIFT),
			substituteRemoteTXReq((TxRxFrame0 & XCANPS_IDR_SRR_MASK) >> XCANPS_IDR_SRR_SHIFT),
			extensionID((TxRxFrame0 & XCANPS_IDR_IDE_MASK) >> XCANPS_IDR_IDE_SHIFT),
			extendedID((TxRxFrame0 & XCANPS_IDR_ID2_MASK) >> XCANPS_IDR_ID2_SHIFT),
			remoteTXRequest((TxRxFrame0 & XCANPS_IDR_RTR_MASK))
		{}

		FrameID& operator=(const quint32& TxRxFrame0) {

			standardID = (TxRxFrame0 & XCANPS_IDR_ID1_MASK) >> XCANPS_IDR_ID1_SHIFT;
			substituteRemoteTXReq = (TxRxFrame0 & XCANPS_IDR_SRR_MASK) >> XCANPS_IDR_SRR_SHIFT;
			extensionID = (TxRxFrame0 & XCANPS_IDR_IDE_MASK) >> XCANPS_IDR_IDE_SHIFT;
			extendedID = (TxRxFrame0 & XCANPS_IDR_ID2_MASK) >> XCANPS_IDR_ID2_SHIFT;
			remoteTXRequest = TxRxFrame0 & XCANPS_IDR_RTR_MASK;
			return *this;
		}

		FrameID(quint32 _standardID, quint32 _extendedID) :
			standardID(_standardID & 0x7FF),
			substituteRemoteTXReq(0),
			extensionID((_extendedID & 0x3FFFF) > 0),
			extendedID(_extendedID & 0x3FFFF),
			remoteTXRequest(0)
		{}

		operator quint32() const {
			return (
					(standardID << XCANPS_IDR_ID1_SHIFT) |
					(substituteRemoteTXReq << XCANPS_IDR_SRR_SHIFT) |
					(extensionID << XCANPS_IDR_IDE_SHIFT) |
					(extendedID <<  XCANPS_IDR_ID2_SHIFT) |
					(remoteTXRequest)
			);
		}
		inline QString toString() const {
			QString out;
			out <<
					"StandardID: " << QString::number(standardID, 16) <<
					", SubstituteRemoteTXReq: " << QString::number(substituteRemoteTXReq, 16) <<
					", ExtensionID: " << QString::number(extensionID, 16) <<
					", ExtendedID: " << QString::number(extendedID, 16) <<
					", RemoteTXRequest: " << QString::number(remoteTXRequest, 16);
			return  out;
		}

	private:
		quint32 standardID	            : 11;
		quint32 substituteRemoteTXReq	: 1;
		quint32 extensionID	            : 1;
		quint32 extendedID              : 18;
		quint32 remoteTXRequest         : 1;
	};
	struct Payload {
		quint8 data[8];
		quint8 size;
	};
	struct ReceiveFrame {
		FrameID sender;
		Payload payload;
		quint32 timestamp;
	};
	enum  InterruptType : quint8{
		receiveDone,
		sentDone
	};
	struct InterruptArg {
		InterruptType type;
		ReceiveFrame receivedFrame;
	};

	xCanPs(quint32 _deviceID, quint32 _clock, qint32 _interruptID = -1, bool _debugEnable = false);
	bool init(quint32 _bitRate = 500000, bool _interrupEnable = false, std::function<void(const InterruptArg&)> _userCb = nullptr);
	bool send(FrameID ID, const Payload& paylaod);
	bool receive();

private:


	quint32 deviceID;
	const quint32 xCanClock;
	qint32 interruptID;
	bool debugEnable;
	XCanPs instance;

	/*
	 * Buffers to hold frames to send and receive. These are declared as global so
	 * that they are not on the stack.
	 * These buffers need to be 32-bit aligned
	 */
	quint32 TxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
	quint32 RxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];

	std::function<void(const InterruptArg&)> userCb = nullptr;


	// BitRate = CanClock / ((BRPR + 1) * (3 + TS1 + TS2))
	struct BitRateCoefs {
		quint8 SJW = 0x03; 	// SyncJumpWidth
		quint8 BRPR; 		// baudRate prescalar (0 ~ 255)
		quint8 TS1; 		// time segment 1 (0 ~ 15)
		quint8 TS2; 		// time segment 2 (0 ~ 7)
	} bitRateCoefs;

	inline QDebug dbg() {
		QDebug out(debugEnable);
		out << "CanPS(" << deviceID << ") :";
		return out;
	}

	void internalSendHandler();
	void internalReceiveHandler();
	void internalErrorHandler(u32 ErrorMask);
	void internalEventHandler(u32 IntrMask);
	std::optional<BitRateCoefs> extractCoefs(quint32 desiredBitrate);

};

#endif
