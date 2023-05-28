#include <QTime.h>
#include <QString.h>
#include <time.h>
#ifdef XSDK
// 1 tick = (1/312.5Mhz)S = 3.2ns
quint64 QTime::nowTick(){
	XTime t;
	XTime_GetTime(&t);
	return t;
}
#endif

void QTime::delayMs(quint64 ms) {
#ifdef XSDK
	quint64 now = nowMs();
	while((nowMs() - now) < ms);
#elif defined(_GLIBCXX_HAS_GTHREADS)
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#endif
}

void QTime::delayUs(quint64 us) {
#ifdef XSDK
	quint64 now = nowUs();
	while((nowUs() - now) < us);
#elif defined(_GLIBCXX_HAS_GTHREADS)
	std::this_thread::sleep_for(std::chrono::microseconds (us));
#endif

}
void QTime::delayNs(quint64 ns) {

#ifdef XSDK
	quint64 now = nowNs();
	while((nowNs() - now) < ns);
#elif defined(_GLIBCXX_HAS_GTHREADS)
	std::this_thread::sleep_for(std::chrono::nanoseconds (ns));
#endif
}

void QTime::delayS(quint64 s) {

#ifdef XSDK
	quint64 now = nowS();
	while((nowS() - now) < s);
#elif defined(_GLIBCXX_HAS_GTHREADS)
	std::this_thread::sleep_for(std::chrono::seconds (s));
#endif

}

quint64 QTime::nowMs(){

#ifdef XSDK
	return TICK_TO_MS(nowTick());
#else
	return (std::chrono::system_clock::now().time_since_epoch().count() / 1e6);
#endif

}

quint64 QTime::nowUs() {

#ifdef XSDK
	return TICK_TO_US(nowTick());
#else
	return std::chrono::system_clock::now().time_since_epoch().count() / 1e3;
#endif

}

quint64 QTime::nowNs() {

#ifdef XSDK
	return TICK_TO_NS(nowTick());
#else
	return (std::chrono::system_clock::now().time_since_epoch().count());
#endif
}

quint64 QTime::nowS() {

#ifdef XSDK
	return TICK_TO_S(nowTick());
#else
	return (std::chrono::system_clock::now().time_since_epoch().count() / 1e9);
#endif

}

#ifdef XSDK
void QTime::setTimerTick(quint64 t) {
	XTime_SetTime(t);
}
#endif



// convert unix time to a date string in format of YYYYMMDDHHMMSS
QString QTime::nowDateTime(quint64 nowS) {
    char tChar[16];
    struct tm tm = *localtime(reinterpret_cast<const time_t *>(&nowS));

    return QString(tChar,
                   sprintf(
                           tChar,
                           "%04u%02u%02u%02u%02u%02u",
                           tm.tm_year + 1900,
                           tm.tm_mon + 1,
                           tm.tm_mday,
                           tm.tm_hour,
                           tm.tm_min,
                           tm.tm_sec));
}

// convert unix time to a date string in format of YYYYMMDD
QString QTime::nowDate(quint64 nowS) {
    return nowDateTime(nowS).left(8);
}


// convert unix time to a date string in format of HHMMSS
QString QTime::nowTime(quint64 nowS) {
    return nowDateTime(nowS).right(7);
}
