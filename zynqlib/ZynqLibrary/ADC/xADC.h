#pragma once
#include <xparameters.h>
#if XPAR_XADCPS_NUM_INSTANCES > 0
#include <xadcps.h>
#include <QtGlobal.h>
class xAdcPs {


	int deviceID;
	XAdcPs_Config *configPtr;
	XAdcPs instance;


public:
	enum Channels {
		temperature = 0x00,
	};
	xAdcPs(int _deviceID) :
		deviceID(_deviceID)
	{}
	bool init() {

		configPtr = XAdcPs_LookupConfig(deviceID);
		if (configPtr == NULL) return false;

		XAdcPs_CfgInitialize(&instance, configPtr,
				configPtr->BaseAddress);

		if(XAdcPs_SelfTest(&instance) != XST_SUCCESS) return false;
		XAdcPs_SetSequencerMode(&instance, XADCPS_SEQ_MODE_SAFE);
		return true;
	}
	static float rawToTemperature(const quint16& raw) {
		return XAdcPs_RawToTemperature(raw);
	}
	static float rawToVoltage(const quint16& raw) {
		return XAdcPs_RawToVoltage(raw);
	}
	quint16 readRaw(Channels channel) {
		return XAdcPs_GetAdcData(&instance, channel);
	}
	float readSocTemperature() {
		return rawToTemperature(readRaw(temperature));

	}
};
#endif



