#include <AxiSwitch.h>

#if XPAR_XAXIS_SWITCH_NUM_INSTANCES > 0
#include <QDebug.h>

QDebug AxiSwitch::dbg() {
	QDebug out(dbgEnable);
	out << "AxiSwitch" << deviceID << ":";
	return out;
}

AxiSwitch::AxiSwitch(int _deviceID, bool debugEnable) :
	deviceID(_deviceID),
	dbgEnable(debugEnable) {

}

bool AxiSwitch::init() {

	int status;
	config = XAxisScr_LookupConfig(deviceID);
	if (NULL == config) {
		dbg() << "no config found";
		return false;
	}

	status = XAxisScr_CfgInitialize(&instance, config, config->BaseAddress);
	if (status != XST_SUCCESS) {
		dbg() << "AXI4-Stream initialization failed";
	}

	return (status == XST_SUCCESS);
}

bool AxiSwitch::portEnable(int slaveIndex, int masterIndex) {

	/* Disable register update */
	XAxisScr_RegUpdateDisable(&instance);

	XAxisScr_MiPortEnable(&instance, masterIndex, slaveIndex);

	/* Enable register update */
	XAxisScr_RegUpdateEnable(&instance);

	/* Check for MI port enable */
	return (bool)XAxisScr_IsMiPortEnabled(&instance, masterIndex, slaveIndex);
}

bool AxiSwitch::portDisable(int masterIndex) {

	/* Disable register update */
	XAxisScr_RegUpdateDisable(&instance);

	XAxisScr_MiPortDisable(&instance, masterIndex);

	/* Enable register update */
	XAxisScr_RegUpdateEnable(&instance);

	/* Check for MI port enable */
	return (bool)XAxisScr_IsMiPortDisabled(&instance, masterIndex);
}

#endif
