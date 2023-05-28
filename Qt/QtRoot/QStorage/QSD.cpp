
#include "QSD.h"
#include <QStringList.h>
#include <QTime.h>
#include <iostream>
#ifdef QFF
#if defined(QFREERTOS) && !defined(QSTM32)
static QMutex qSdMutex(QMutex::NonRecursive, true);
#endif



bool SDCard::mount() {
	if(!_isMounted && isInserted()){
#if defined(QFREERTOS) && !defined(QSTM32)
		QMutexLocker locker(&qSdMutex);
#endif
		lastResult = f_mount(&fatfs,driveLetter.c_str(),1);
		if(lastResult == FR_OK)
			_isMounted = true;
	}
	return _isMounted;

}

SDCard::SDCard(const QString driveName, std::function<bool(void)> cardDetectCallback):
				QFFS(std::move(driveName)),
				sdCardDetectCb(cardDetectCallback)

{}

bool SDCard::unmount() {
	if(_isMounted){
		if(isInserted()){
#if defined(QFREERTOS) && !defined(QSTM32)
			QMutexLocker locker(&qSdMutex);
#endif
			lastResult = f_mount(0,driveLetter.c_str(),1);
			if(lastResult == FR_OK)
				_isMounted = false;
		}
		else {
			_isMounted = false;
		}
	}
	return _isMounted;
}

bool SDCard::format() {

#if defined(QFREERTOS) && !defined(QSTM32)
	QMutexLocker locker(&qSdMutex);
#endif
	lastResult = f_mkfs(QFFS::driveLetter.c_str(), FM_FAT32, 0, NULL, FF_MAX_SS);
	return (lastResult == FR_OK);

}

// must be really a nested directory like this : dir1/dir2/dir3/..
bool SDCard::createNestedDirectory(const QString& dir) {

	if(!isMounted()) return false;
	QStringList dirs = dir.split('/');
	QString tempDir;
	for(const auto& singleDir : dirs) {
		tempDir += singleDir;
		if(!createSingleDirectory(tempDir))
			if(!(lastResult == FR_EXIST || lastResult == FR_OK)) return false;
		tempDir += "/";
	}
	return true;
}

bool SDCard::isDirectoryExist(const QString& dir) {
	DIR directory;
	{
#if defined(QFREERTOS) && !defined(QSTM32)
		QMutexLocker locker(&qSdMutex);
#endif
		lastResult = f_opendir(&directory, dir.c_str());
		if(lastResult != FR_OK) return false;
	}
	{
#if defined(QFREERTOS) && !defined(QSTM32)
		QMutexLocker locker(&qSdMutex);
#endif
		lastResult = f_closedir(&directory);
		return true;
	}
}

// must be really a single directory without any '/'
bool SDCard::createSingleDirectory(const QString& dir) {

	if(!isMounted()) return false;
#if defined(QFREERTOS) && !defined(QSTM32)
	QMutexLocker locker(&qSdMutex);
#endif
	lastResult = f_mkdir(dir.c_str());
	return (lastResult == FR_OK || lastResult == FR_EXIST);
}

bool SDCard::isInserted() {
	if(!sdCardDetectCb) return true;
	int checkCounter = 0;
	for(int i = 0; i < 100; i++) {
		if(sdCardDetectCb())
			checkCounter++;
#ifdef QFREERTOS
		if(xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
			vTaskDelay(pdMS_TO_TICKS(1));
		else
			QTime::delayMs(1);
#else
		QTime::delayMs(1);
#endif
	}
	return (checkCounter > 80);
}

#endif

