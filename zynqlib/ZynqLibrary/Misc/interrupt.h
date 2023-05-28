#pragma once

#include "xscugic.h"
#include <QSingleton.h>
#include <QtGlobal.h>
#ifdef XPAR_PS7_SCUGIC_0_DEVICE_ID

// this class is meant to handle interrupts enabled in peripherals.
// implementing of API for any peripheral that is going to use interrupts have to inheritance this class
// NOTE: this class use PS xGuic
class InterruptController : public QSingleton<InterruptController> {
	friend class QSingleton<InterruptController>;
private:


	XScuGic *XScuGicInstance = nullptr;
	XScuGic_Config *Config;

	InterruptController();

public:

	enum TriggerType : quint8 {
		risingEdge 	= 0x03,
		highLevel 		= 0x01,
		invalid			= 0x00
	};
	bool isReady() { return (XScuGicInstance->IsReady == XIL_COMPONENT_IS_READY); }
	int connectHandler(u32 interruptID, Xil_InterruptHandler interruptHandler, void *CallBackRef);
//	int setType(int interruptID, int interruptType);
	void enableIinterrupt(int interruptID);
	void disableIinterrupt(int interruptID);
	void setPriority(int interruptID, quint8 priority);
	quint8 getPriority(int interruptID);

	void setTriggerType(int interruptID, TriggerType tt);
	TriggerType getTriggerType(int interruptID);

public:

};

#endif
