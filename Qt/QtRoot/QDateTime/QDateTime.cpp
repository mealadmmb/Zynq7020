#include <QDateTime.h>
#ifdef XSDK
void QDateTime::setMSecsSinceEpoch(qint64 msecs) {
	QTime::setTimerTick(MS_TO_TICK(msecs));
}
#endif

QDateTime::QDateTime(QDateTime &&other) noexcept {

}

QDateTime::QDateTime(const QDateTime &other) {

}

QDateTime::QDateTime(const QDate &date) {

}

qint64 QDateTime::currentMSecsSinceEpoch() {
	return QTime::nowMs();
}
