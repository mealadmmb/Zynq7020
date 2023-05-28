#include <QTextStream.h>

QTextStream::QTextStream(QString *str)
: string(str)
{}

QTextStream &QTextStream::operator<<(const QString &str) {
	*string << str;
	return *this;
}

QTextStream &QTextStream::operator<<(const double &d) {
	*string << QString(std::to_string(d));
	return *this;
}
