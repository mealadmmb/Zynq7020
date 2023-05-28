#pragma once

#include <functional>

#ifdef QFREERTOS
#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#else
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#endif
#include <QHash.h>



// create an instance, setInterval, connect callBack then start the timer
class QTimer {
public:
	QTimer() = default;
	~QTimer();
	void setInterval(int interval);
	void connect(std::function<void(void)> func);
	bool start();
	inline bool start(int interval) {
		setInterval(interval);
		return start();
	}
	bool stop();
	bool create();
	bool is_running() const noexcept;



private:
	TimerHandle_t _xQTimer = nullptr;
	int _interval{};
	std::function<void(void)> _func;
	bool created = false;
	bool started = false;

};

#else
#ifdef XSDK

#else
#include <chrono>
#include <thread>
#include <future>

class QTimer {
public:
	QTimer();
	~QTimer();
	void stop();
	void connect(std::function<void(void)> func);
	void setInterval(int interval);
	void start();
	bool is_running() const noexcept;

private:
	int _interval{};
	std::function<void(void)> _func;
	std::atomic<bool> _execute;
	std::thread _thd;
};

#endif
#endif
