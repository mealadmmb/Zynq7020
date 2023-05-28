#pragma once
#include <xparameters.h>
#if XPAR_XAXIS_SWITCH_NUM_INSTANCES > 0
#include <xaxis_switch.h>

class QDebug;

class AxiSwitch {

	XAxis_Switch_Config* config = NULL;
	XAxis_Switch instance;
	int deviceID;
	bool dbgEnable;
	QDebug dbg();
public:
	AxiSwitch(int _deviceID, bool debugEnable = false);
	bool init();
	bool portEnable(int slaveIndex, int masterIndex);
	bool portDisable(int masterIndex);

};

#endif

