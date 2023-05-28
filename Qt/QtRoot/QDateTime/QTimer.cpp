#include <QTimer.h>


#ifdef QFREERTOS

static QHash<TimerHandle_t, std::function<void(void)>> timerHandleToCallbackHash;

static void xQTimerSharedCallback(TimerHandle_t pxTimer){

	if(timerHandleToCallbackHash.contains(pxTimer))
		timerHandleToCallbackHash.value(pxTimer)();
}

bool QTimer::stop()
{
	if(created && started) {
		if(xTimerStop(_xQTimer, pdMS_TO_TICKS(1000)) != pdFAIL){
			started = false;
		}
	}
	return !started;
}

bool QTimer::create() {

	if(!created && _interval > 0) {

		_xQTimer = xTimerCreate
				(
						"xQTimer",
						/* The timer period in ticks, must be
							 greater than 0. */
						pdMS_TO_TICKS(_interval),
						/* The timers will auto-reload themselves
							 when they expire. */
						pdTRUE,
						/* The ID is used to store a count of the
							 number of times the timer has expired, which
							 is initialized to 0. */
						( void * ) 0,
						/* Each timer calls the same callback when
							 it expires. */
						xQTimerSharedCallback
				);
		if(_xQTimer){
			timerHandleToCallbackHash.insert(_xQTimer, _func);
			created = true;
		}
	}
	return created;

}

bool QTimer::start() {
	if(created && !started){
		if( xTimerStart( _xQTimer, 0 ) != pdPASS ){
			timerHandleToCallbackHash.erase(timerHandleToCallbackHash.find(_xQTimer));
			started = false;
		}
		else {
			started = true;
		}
	}
	return started;
}

bool QTimer::is_running() const noexcept
		{
	return started;
		}

QTimer::~QTimer()
{
	xTimerDelete(_xQTimer, 0);
	timerHandleToCallbackHash.erase(_xQTimer);
}


void QTimer::connect(std::function<void(void)> func) {
	this->_func = std::move(func);
}

void QTimer::setInterval(int interval) {
	this->_interval = interval;
}
#else
#ifdef XSDK

#else
void QTimer::stop()
{
	_execute.store(false, std::memory_order_release);
	if( _thd.joinable() )
		_thd.join();
}

void QTimer::start()
{
	if( _execute.load(std::memory_order_acquire) ) {
		stop();
	};
	int thInterval = this->_interval;
	std::function<void(void)>& thFunc = this->_func;
	_execute.store(true, std::memory_order_release);
	_thd = std::thread([this, thInterval, thFunc]()
			{
		while (_execute.load(std::memory_order_acquire)) {
			thFunc();
			std::this_thread::sleep_for(
					std::chrono::milliseconds(thInterval));
		}
			});
}

bool QTimer::is_running() const noexcept
		{
	return ( _execute.load(std::memory_order_acquire) && _thd.joinable() );
		}

QTimer::~QTimer()
{
	if( _execute.load(std::memory_order_acquire) ) {
		stop();
	};
}

QTimer::QTimer()
:_execute(false)
{}

void QTimer::connect(std::function<void(void)> func) {
	this->_func = std::move(func);
}

void QTimer::setInterval(int interval) {
	this->_interval = interval;
}

#endif
#endif

