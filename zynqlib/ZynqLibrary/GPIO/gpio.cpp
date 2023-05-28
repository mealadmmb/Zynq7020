#include <gpio.h>


void xGPIOPS::init(int deviceID) {
	XGpioPs_Config *ConfigPtr;
	ConfigPtr = XGpioPs_LookupConfig(deviceID);
	XGpioPs_CfgInitialize(&instance, ConfigPtr,ConfigPtr->BaseAddr);
}

void xGPIOPS::writePin(int pin, int val) {
	XGpioPs_WritePin(&instance, pin, val);
}

int xGPIOPS::readPin(int pin) {
	return XGpioPs_ReadPin(&instance, pin);
}

void xGPIOPS::setDirection(int pin, IODirection dir) {
	XGpioPs_SetDirectionPin(&instance, pin, dir);
	XGpioPs_SetOutputEnablePin(&instance, pin, dir);
}

void xGPIOPS::togglePin(int pin) {
	int val = readPin(pin);
	writePin(pin, !val);
}
