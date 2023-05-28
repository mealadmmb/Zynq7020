#pragma  once

#ifdef QFF
#include "ff.h"
typedef int64_t qfile_size_t;
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
#include <QMutex.h>
#include <QMutexLocker.h>
#endif
#else
#include <fstream>
#include <QHash.h>
typedef std::streamsize qfile_size_t;
#endif
#include <utility>
#include <QString.h>
#include <QStringList.h>
#include <QByteArray.h>
#include <QtGlobal.h>


class QFile {
public:

	enum QFileMode {
#ifdef QFF

		ReadOnly				= FA_READ,  // read access to a existing file, if does not exist open will fail
		WriteOnly				= FA_CREATE_ALWAYS | FA_WRITE, // write access to a file, if does not exist will be created, if exist will be overwritten
		ReadWrite				= FA_CREATE_ALWAYS | FA_READ | FA_WRITE, // read/write access to a file, if does not exist will be created, if exist will be overwritten due to write


		Append					= FA_OPEN_APPEND | FA_WRITE, // open file and set file pointer to end and give write access. if file does not exist it will be created
		ReadWriteAppend			= FA_OPEN_APPEND | FA_WRITE | FA_READ // open file and set file pointer to end and give read/write access. if file does not exist it will be created
#else
		ReadOnly,
		WriteOnly,
		ReadWrite,
		Append,
		ReadWriteAppend
#endif
	};
	explicit QFile(QString filePath);
	QFile();
	~QFile();
	void setPath(QString filePath);
	qfile_size_t findPos(const quint8& c);
	qfile_size_t findPos(const QString& str);
	qfile_size_t findPos(const QByteArray& arr);

	QByteArray read(qfile_size_t count);
	qfile_size_t read(void* data, qfile_size_t count);

	qfile_size_t write(const void* data, qfile_size_t count);
	qfile_size_t write(const QByteArray& arr);
	qfile_size_t write(const QString& str);
	bool flush();

	qfile_size_t pos();
	void seek(qfile_size_t pos);
	void rewind();
	void seekToEOF();

#ifdef QFF
	inline FRESULT getLastError() {
		return res;
	}
#endif

	static bool createFile(const QString& filePath);
	static bool exists(const QString& filePath);


	static bool createSingleDirectory(const QString& dir);
	static bool createNestedDirectory(const QString& dir);

	bool open(QFileMode mode = ReadOnly);
	qfile_size_t size() const;
	bool isEmpty() const { return (size() == 0); }
	quint32 Crc();
	bool isOpen() const;
	bool close();
	bool isEOF() const;
	bool remove();
	QString getPath() const { return path;}
	QFileMode getOpenMode() const { return mode; }
    QFile& operator<<(std::ostream &os);




private:

	uint32_t crc32_for_byte(uint32_t r) {
		for(int j = 0; j < 8; ++j)
			r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
		return r ^ (uint32_t)0xFF000000L;
	}

#ifdef QFF
	bool isOpened{};
	FIL file = {};
	FILINFO info = {};
	FRESULT res{};
#else
	void fillFileModeMap();
	QHash<QFileMode, std::ios_base::openmode> fileMode ;
	std::fstream file;
#endif
	QFileMode mode;
	QString path;

};
