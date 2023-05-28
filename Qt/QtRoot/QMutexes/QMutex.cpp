#include "QMutex.h"
#ifdef QFREERTOS
#include <iostream>

QMutex::QMutex(RecursionMode mode, bool areSamePriorityTask) :
mode(mode),
samePriority(areSamePriorityTask)
{
	switch(mode) {
	case Recursive: {
		mutex = xSemaphoreCreateRecursiveMutex();
		if(mutex == NULL) {
			std::cout << "QMutex -> mutex creation failed\r\n";
		}
	}
	break;
	case NonRecursive: {
		mutex = xSemaphoreCreateMutex();
		if(mutex == NULL) {
			std::cout << "QMutex -> mutex creation failed\r\n";
		}
	}
	break;

	default: {

	}
	break;
	}
}

QMutex::~QMutex() {
	if(mutex != NULL)
		vSemaphoreDelete(mutex);
}

void QMutex::lock() {
	if(mutex != NULL && xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
		switch(mode) {
		case Recursive: {
			if(xSemaphoreTakeRecursive(mutex, portMAX_DELAY) == pdTRUE) {
				lockCount++;
				locked = true;
			}
			else {
			}
		}
		break;

		case NonRecursive: {
			if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
				locked = true;
			}
			else {

			}
		}
		break;
		default: {

		}
		break;
		}
	}
}

void QMutex::unlock() {
	if(mutex != NULL && xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
		switch(mode) {
		case Recursive: {
			if(xSemaphoreGiveRecursive(mutex) == pdTRUE) {
				lockCount--;
				if(lockCount == 0) {
					locked = false;
				}
			}
			else {

			}
		}
		break;

		case NonRecursive: {
			if(xSemaphoreGive(mutex) == pdTRUE) {
				locked = false;

			}
			else {

			}
		}
		break;
		default: {

		}
		break;
		}
	}
	if(samePriority && (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)) {
		taskYIELD();
	}
	else {
		//printf("b\n");
	}
}



#endif

