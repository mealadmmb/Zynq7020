
#include <QTextFile.h>
#include <QStringList.h>
#include <iostream>

QTextFile::QTextFile() {
}

QTextFile::~QTextFile() {
}

QStringList QTextFile::readLines() {
	return readAll().split('\n');
}
void QTextFile::writeLine(const QString& line) {
	QFile::write(line);
	QFile::write("\n");
	QFile::flush();
}

void QTextFile::writeString(const QString& str) {
	QFile::write(str);
	QFile::flush();
}

QString QTextFile::readAll() {
	qfile_size_t availableBytes = QFile::size();
	if(availableBytes <= 0) return QString();
	QByteArray bytesRead = QFile::read(availableBytes);
	return QString(bytesRead.data(), bytesRead.size());
}


