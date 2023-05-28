#pragma once
#ifdef QFREERTOS
#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#else
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif
class QMutex {
	friend class QMutexLocker;
public:
	enum RecursionMode{
		Recursive,
		NonRecursive
	} ;
	QMutex(RecursionMode mode = NonRecursive, bool areSamePriorityTask = true);
	~QMutex();
	void lock();
	void unlock();
	bool isLocked() { return locked; }

private:
	SemaphoreHandle_t mutex = NULL;
	bool locked = false;
	RecursionMode mode;
	int lockCount = 0;
	bool samePriority = false;
};

#endif
