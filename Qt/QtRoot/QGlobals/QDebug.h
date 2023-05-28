#pragma once
#ifdef str
#undef str
#endif
#include <utility>
#include <QSingleton.h>
#include <QString.h>
#include <QFile.h>
#include <sstream>
#include <iostream>
#include <QTime.h>
class QDebug {

public:

	enum Output {
		Console,
		File
	};

	explicit QDebug(bool en = true, Output _output = Console, const char*  _outputFilePath = nullptr):
					debugEnable(en),
					output(_output)
	{
		if (output == File) {
			if(_outputFilePath == nullptr) {
				outputFile = new QFile(QTime::nowDate() + ".log");
			}
			outputFile = new QFile(_outputFilePath);
			if(!outputFile->open(QFile::Append)) {
				std::cout << "error in creating " << outputFile->getPath() << " file in QDebug constructor\n";
				debugEnable = false;
			}
		}
	}
	~QDebug() {
		if(debugLineFeed && debugEnable) {
			if(output == Console) {
				std::cout << std::endl;
			}
			else {
				std::stringbuf fb;
				std::ostream os(&fb);
				os << std::endl;
				*outputFile << os;
				delete outputFile;
			}
		}
	}
	void enable() {debugEnable = true;}
	void disable() {debugEnable = false;}
	inline QDebug &maybeSpace() {
		if (debugSpaceEnable) {
			if(output == Console)
				std::cout << ' ';
			else {
				std::stringbuf fb;
				std::ostream os(&fb);
				os << ' ';
				*outputFile << os;
			}
		}
		return *this;
	}
	inline QDebug &space() { debugSpaceEnable = true; std::cout << ' '; return *this; }
	inline QDebug &nospace() { debugSpaceEnable = false; return *this; }
	inline QDebug &nolinefeed() { debugLineFeed = false; return *this; }
	inline QDebug& quote() { debugQuoteEnable = true; return *this; }
	inline QDebug& noquote() {
		return *this;
	}

	template<class T>
	inline QDebug& operator<< (const T& a) {
		if (debugEnable) {
			if(output == Console) {
				std::cout << (debugQuoteEnable ? "\"" : "") << a << (debugQuoteEnable ? "\"" : "") << (debugSpaceEnable ? " " : "");
			}
			else {
				std::stringbuf fb;
				std::ostream os(&fb);
				os << (debugQuoteEnable ? "\"" : "") << a << (debugQuoteEnable ? "\"" : "") << (debugSpaceEnable ? " " : "");
				*outputFile << os;
			}
		}
		return *this;
	}
	inline QDebug& operator<< (const quint8& a) {
		if (debugEnable) {
			if(output == Console) {
				std::cout <<
						(debugQuoteEnable ? "\"" : "") <<
						static_cast<unsigned>(a) <<
						(debugQuoteEnable ? "\"" : "") <<
						(debugSpaceEnable ? " " : "");
			}
			else {
				std::stringbuf fb;
				std::ostream os(&fb);
				os <<
						(debugQuoteEnable ? "\"" : "") <<
						static_cast<unsigned>(a) <<
						(debugQuoteEnable ? "\"" : "") <<
						(debugSpaceEnable ? " " : "");
				*outputFile << os;
			}
		}
		return *this;
	}
	inline QDebug& operator<< (const qint8& a) {
		if (debugEnable) {
			if(output == Console) {
				std::cout <<
						(debugQuoteEnable ? "\"" : "") <<
						static_cast<signed>(a) <<
						(debugQuoteEnable ? "\"" : "") <<
						(debugSpaceEnable ? " " : "");
			}
			else {
				std::stringbuf fb;
				std::ostream os(&fb);
				os <<
						(debugQuoteEnable ? "\"" : "") <<
						static_cast<signed>(a) <<
						(debugQuoteEnable ? "\"" : "") <<
						(debugSpaceEnable ? " " : "");
				*outputFile << os;
			}
		}
		return *this;
	}


private:
	bool debugEnable = false;
	bool debugQuoteEnable = false;
	bool debugSpaceEnable = true;
	bool debugLineFeed = true;
	Output output;
	QFile* outputFile;


};

#define FileQDebug() QDebug(true, QDebug::File)
#define qDebug() QDebug()
