#pragma once
#include <QDate.h>
#include <QTime.h>
#include <QtGlobal.h>

class QDateTime{
public:
	QDateTime(QDateTime &&other) noexcept ;
	QDateTime(const QDateTime &other);
	explicit QDateTime(const QDate &date);
	QDateTime() = default;

	inline QDate date(){ return bDate;}
	inline  QTime time(){return bTime;}
#ifdef XSDK
	void setMSecsSinceEpoch(qint64 msecs);
#endif
	static qint64 currentMSecsSinceEpoch();


private:
	QTime bTime{};
	QDate bDate{};


};
