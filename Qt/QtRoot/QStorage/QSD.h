#pragma once
#ifdef QFF
#include "QFFS.h"
#include <functional>
#ifdef QFREERTOS
#include <QMutex.h>
#include <QMutexLocker.h>
#endif

class SDCard : public QFFS {

public:
	enum cardStatus : bool {
		inserted = true,
		ejected = false
	};
	explicit SDCard(const QString driveName, std::function<bool(void)> cardDetectCallback = nullptr);
	bool mount() override;
	bool unmount() override;
	bool createSingleDirectory(const QString& dir) override;
	bool createNestedDirectory(const QString& dir) override;
	bool isDirectoryExist(const QString& dir) override;
	bool format();
	bool isInserted();
	cardStatus status() { return (cardStatus)isInserted(); }
	FRESULT getLastError() { return lastResult; }

private:

	FRESULT lastResult;
	std::function<bool(void)> sdCardDetectCb = nullptr;
};
#endif
