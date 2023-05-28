#pragma once
#ifdef XSDK
#include <xtime_l.h>
#define TICK_TO_NS(t) 	(t * ((1e9) / COUNTS_PER_SECOND))
#define TICK_TO_US(t)	(TICK_TO_NS(t) / 1e3)
#define TICK_TO_MS(t)	(TICK_TO_US(t) / 1e3)
#define TICK_TO_S(t)	(TICK_TO_MS(t) / 1e3)

#define NS_TO_TICK(ns) 	((1e9 / COUNTS_PER_SECOND) / ns)
#define US_TO_TICK(us) 	(NS_TO_TICK(us * 1e3))
#define MS_TO_TICK(ms) 	(US_TO_TICK(ms * 1e3))
#define S_TO_TICK(s) 	(MS_TO_TICK(s * 1e3))
#else
#include <chrono>
#include <thread>
#endif
#include <QtGlobal.h>
#include <QSingleton.h>


class QString;
class QTime : public QSingleton<QTime> {

	friend class QSingleton<QTime>;
public:
	static void delayMs(quint64 ms);
	static void delayUs(quint64 us);
	static void delayNs(quint64 ns);
	static void delayS(quint64 s);

#ifdef XSDK
	static quint64 nowTick();
	static void setTimerTick(quint64 t);
#endif
	static quint64 nowMs();
	static quint64 nowUs();
	static quint64 nowNs();
	static quint64 nowS();
    static QString nowDateTime(quint64 nowS = QTime::nowS());
    static QString nowDate(quint64 nowS = QTime::nowS());
    static QString nowTime(quint64 nowS = QTime::nowS());

private:

};

