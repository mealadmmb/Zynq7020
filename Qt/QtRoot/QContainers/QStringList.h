#pragma once
#include <QVector.h>
#include <QString.h>

class QStringList :  public QVector<QString>{
public:
	using QVector<QString>::QVector;
	QStringList& operator<<(const QString& str);


private:


};


std::ostream &operator<<(std::ostream &os, const QStringList &qStringList);
QString& operator<< (QString& str, const QStringList &qStringList);
