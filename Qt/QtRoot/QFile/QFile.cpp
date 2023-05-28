#include <QFile.h>
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
static QMutex qfileMutex(QMutex::Recursive, true);
#endif

QFile::QFile(QString filePath):
			path(std::move(filePath))
{
#ifndef QFF
	fillFileModeMap();
#endif
}

QFile::QFile() {

#ifndef QFF
	fillFileModeMap();
#endif
}


qfile_size_t QFile::size() const {
#ifdef QFF
	return f_size(&file);
#else
	if(!file.is_open())
		return -1;
	struct stat sb;

	if (!stat(path.c_str(), &sb)) return sb.st_size;
	else return -1;
#endif
}

QByteArray QFile::read(qfile_size_t count)
{

	QByteArray buff;
	buff.resize(count);
#ifdef QFF
	UINT br;
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
	QMutexLocker locker(&qfileMutex);
#endif
	res = f_read(&file, buff.data(), count, &br);
#else
	qfile_size_t br = file.read(buff.data(), count).gcount();
#endif
	if (br != count) {
		buff.resize(br);
	}
	return buff;
}

qfile_size_t QFile::read(void *data, qfile_size_t count) {
#ifdef QFF
	UINT br = 0; // bytes read
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
	QMutexLocker locker(&qfileMutex);
#endif
	res = f_read(&file, data, count,&br);
	return br;
#else
	return file.read((char*)data, count).gcount();
#endif
}

qfile_size_t QFile::pos() {
#ifdef QFF
	return f_tell(&file);
#else
	return file.tellg();
#endif
}

void QFile::seek(qfile_size_t pos) {
#ifdef QFF

#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
	QMutexLocker locker(&qfileMutex);
#endif
	res = f_lseek(&file, pos);
#else
	file.seekg(pos);
#endif
}

bool QFile::open(QFile::QFileMode _mode) {
	mode = _mode;
#ifdef QFF
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
	QMutexLocker locker(&qfileMutex);
#endif
	res = f_open(&file, path.c_str(), mode);
	res ? isOpened = false : isOpened = true;
	return isOpened;
#else
	if(!fileMode.contains(mode)) {
		std::cout << "filemode map error\n";
		return false;
	}
	file.open(path, fileMode.value(mode));
	return file.good();
#endif
}



bool QFile::isOpen() const {
#ifdef QFF
	return isOpened;
#else
	return file.is_open();
#endif
}

bool QFile::close() {
#ifdef QFF
	bool out = false;
	if(isOpened) {
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
		QMutexLocker locker(&qfileMutex);
#endif
		if(f_close(&file) == FR_OK) {
			out = true;
			isOpened = false;
		}
	}
	else {
		out = true;
	}
	return out;
#else
	file.close();
	return file.is_open();
#endif
}

qfile_size_t QFile::write(const void* data, qfile_size_t count) {
#ifdef QFF
	UINT bw = 0;// bytes written
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
	QMutexLocker locker(&qfileMutex);
#endif
	res = f_write(&file, data, count, &bw);
	if(res == FR_OK)
		return bw;
	return -1;
#else
	auto beforeWritePos = pos();
	file.write((char*)data, count);
	return (pos() - beforeWritePos);
#endif
}

#ifndef QFF
void QFile::fillFileModeMap() {
	fileMode = {
			{ReadOnly, std::ios_base::in | std::ios_base::binary},
			{WriteOnly, std::ios_base::out | std::ios_base::binary},
			{ReadWrite, std::ios_base::in | std::ios_base::out | std::ios_base::binary}, // this cause create new file each time open called, even the file exist
			{Append, std::ios_base::out | std::ios_base::binary | std::ios_base::app}, // if file does't exist while opening, it will create the file. if exist, the file will open and append data to end of it in each write operation
			{ReadWriteAppend, std::ios_base::out | std::ios_base::in | std::ios_base::app | std::ios_base::binary} // this type of openmode cause of seek to end of the file before each writing, but can seek to anywhere while reading
	};
}

#endif

qfile_size_t QFile::write(const QByteArray& arr) {
	return this->write(arr.data(), arr.size());
}

bool QFile::flush() {
	bool out = false;
#ifdef QFF
	if(isOpened) {
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
		QMutexLocker locker(&qfileMutex);
#endif
		res = f_sync(&file);
		out = (res == FR_OK);
	}
#else
	file.flush();
	return true;
#endif
	return out;
}

qfile_size_t QFile::write(const QString& str) {
	return this->write(str.data(), str.size());
}

QFile::~QFile(){
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
	QMutexLocker locker(&qfileMutex);
#endif
	close();
}

void QFile::setPath(QString filePath) {
	if(!isOpen())
		path = filePath;
}
bool QFile::isEOF() const {
#ifdef QFF
	return f_eof(&file);
#else
	return file.eof();
#endif
}
bool QFile::remove() {
	bool out = false;
#ifdef QFF
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
	QMutexLocker locker(&qfileMutex);
#endif
	if(isOpened) close();
	FRESULT res;
	res = f_unlink(path.c_str());
	return (res == FR_OK);
#else

#endif
	return out;
}

bool QFile::exists(const QString& filePath) {
	bool out = false;
#ifdef QFF
	if(filePath.isEmpty()) return false;

	FRESULT _res;
	FILINFO _inf;
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
	QMutexLocker locker(&qfileMutex);
#endif
	_res = f_stat(filePath.c_str(), &_inf);
	out = (_res == FR_OK);
#else

	struct stat st = {0};
	return stat(filePath.c_str(), &st) != -1;
#endif
	return out;
}


// must be really a nested directory like this : dir1/dir2/dir3/..
bool QFile::createNestedDirectory(const QString& dir) {
	
	bool out = true;
	QStringList dirs = dir.split('/');
	QString tempDir;
	for(const auto& singleDir : dirs) {
		tempDir += singleDir;
		std::cout << "creating directory: " << tempDir << " ..\n";
		if(!createSingleDirectory(tempDir)){
			out = false;
			break;
		}
		tempDir += "/";
	}
	return out;
}

// must be really a single directory without any '/'
bool QFile::createSingleDirectory(const QString& dir) {
	bool out = false;
#ifdef QFF
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
	QMutexLocker locker(&qfileMutex);
#endif
	FRESULT r;
	r = f_mkdir(dir.c_str());
	out =  (r == FR_OK || r == FR_EXIST);
#else
	if(exists(dir))
		out = true;
	else
		out = (mkdir(dir.c_str(), 0777) == 0);
#endif
	return out;
}

bool QFile::createFile(const QString& filePath) {
	bool out = false;
#ifdef QFF
	{
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
		QMutexLocker locker(&qfileMutex);
#endif
		if(exists(filePath)) return true;
	}
	FIL _file = {};
	FRESULT _res;
	{
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
		QMutexLocker locker(&qfileMutex);
#endif
		_res = f_open(&_file, filePath.c_str(), WriteOnly);
		if (_res != FR_OK) return false;
	}
	{
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
		QMutexLocker locker(&qfileMutex);
#endif
		_res = f_close(&_file);
	}
	out = (_res == FR_OK);
#else

#endif
	return out;
}


void QFile::rewind() {
#ifdef QFF
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
	QMutexLocker locker(&qfileMutex);
#endif
	f_rewind(&file);
#else
	QFile::seek(0);
#endif
}

void QFile::seekToEOF()  {
#if defined(QFREERTOS) && !defined(QSTM32) && !defined(ESP_PLATFORM)
	QMutexLocker locker(&qfileMutex);
#endif
	seek(size());
}


quint32 QFile::Crc() {
	quint32 out = 0;
	static uint32_t table[0x100];
	if(!*table)
		for(size_t i = 0; i < 0x100; ++i)
			table[i] = crc32_for_byte(i);

	if(!isOpen()){
		out = 0xFFFFFFFF;
		return out;
	}

	auto fileSize = size();

	for(size_t i = 0; i < fileSize; ++i) {
		quint8 c;
		read(&c, 1);
		out = table[(quint8) out ^ c] ^ out >> 8;
	}
	return out;
}

QFile &QFile::operator<<(std::ostream &os) {
    std::stringstream ss;
    ss << os.rdbuf();
    write(ss.str());
    flush();
    return *this;
}


