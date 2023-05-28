#include <QElapsedTimer.h>
#include <QTime.h>

void QElapsedTimer::start() {
	if(!running) {
		startedEpoch = QTime::nowNs();
		running = true;
	}
}

void QElapsedTimer::stop() {
	running = false;
}

uint64_t QElapsedTimer::nsecsElapsed() {
	if(running)
		elapsedTimeNs = QTime::nowNs() - startedEpoch;
	return elapsedTimeNs;
}

void QElapsedTimer::restart() {

	if(running) {
		startedEpoch = QTime::nowNs();
		elapsedTimeNs = 0;
		running = true;
	}
}

uint64_t QElapsedTimer::usecsElapsed() {
	return (nsecsElapsed() / 1e3);
}

uint64_t QElapsedTimer::msecsElapsed() {
	return (nsecsElapsed() / 1e6);
}

uint64_t QElapsedTimer::secsElapsed() {
	return (nsecsElapsed() / 1e9);
}
