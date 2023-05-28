#pragma once
#include <QFile.h>
#include <utility>

class QTextFile : private QFile {

private:

public:

	using QFile::createFile;
	using QFile::open;
	using QFile::size;
	using QFile::isOpen;
	using QFile::close;
	using QFile::isEOF;
	using QFile::remove;
	using QFile::exists;
	using QFile::getPath;
	using QFile::write;
	using QFile::isEmpty;
	using QFile::QFileMode;
#ifdef STDCXX
	using QFile::getLastError;
#endif

	QTextFile(const QString& filePath) : QFile(filePath) {};
	QTextFile();
	~QTextFile();

	QStringList readLines();
	QString readAll();
	QString readLine(int lineNumber);


	void writeString(const QString& str);
	void writeLine(const QString& line);
	void insertLine(int lineNumber, const QString& line);

};
