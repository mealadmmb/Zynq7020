#pragma  once

#ifdef QFF
#include "ff.h"
#include <QStringList.h>

#ifdef QSTM32
#define FF_MAX_SS _MAX_SS
#endif

class QFFS {

public:

	explicit QFFS(QString driveLetter);
	virtual ~QFFS();

	virtual bool mount() = 0;
	virtual bool unmount() = 0;
	virtual bool createSingleDirectory(const QString& dir) = 0;
	virtual bool createNestedDirectory(const QString& dir) = 0;
	virtual bool isDirectoryExist(const QString& dir) = 0;
	QString getLetter();
	bool isMounted() const {return _isMounted;}
	quint64 capacity() const;
	quint64 freeSpace() const;

protected:

	FATFS  fatfs;
	const QString driveLetter;
	bool _isMounted;

};
#endif
