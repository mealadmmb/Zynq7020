#pragma once
#include <QString.h>
#include <memory>
class QTextStream {

public:
	QTextStream() = default;
	explicit QTextStream(QString* str);

	QTextStream& operator<<(const QString& str);
	QTextStream& operator<<(const double& d);

private:
	QString* string;
};
