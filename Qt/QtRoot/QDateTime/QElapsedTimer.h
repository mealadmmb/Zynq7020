#pragma once
#include <QtGlobal.h>

class QElapsedTimer {
public:

	void start();
	void stop();
	void restart();
	uint64_t nsecsElapsed();
	uint64_t usecsElapsed();
	uint64_t msecsElapsed();
	uint64_t secsElapsed();

private:
	uint64_t startedEpoch = {};
	uint64_t elapsedTimeNs = {};
	bool running = {};
};
