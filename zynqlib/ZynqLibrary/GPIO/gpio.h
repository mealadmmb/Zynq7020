#pragma once
#include "xparameters.h"
#if XPAR_XGPIOPS_NUM_INSTANCES > 0

#include "xgpiops.h"
#include <QSingleton.h>

class xGPIOPS : public QSingleton<xGPIOPS> {
	friend class QSingleton<xGPIOPS>;
	XGpioPs instance;
public:

	enum IODirection {
		input = 0x00,
		output = 0x01
	};

	void init(int deviceID);

	void setDirection(int pin, IODirection dir);

	void writePin(int pin, int val);
	int readPin(int pin);
	void togglePin(int pin);
};

#endif
