#include <QDataContainer.h>
#include <assert.h>
#ifdef QFREERTOS
#ifdef ESP_PLATFORM
#include <freertos/FreeRTOS.h>
#include <freertos/portable.h>
#else
#include <FreeRTOS.h>
#include <portable.h>
#endif
#endif

std::ostream &operator<<(std::ostream &os, const QDataContainer& qDataContainer) {

	if(!qDataContainer.ptr || !qDataContainer.size) {
		std::cout << "empty QDataContainer";
		return os;
	}
	else {
		for (size_t i = 0; i < qDataContainer.size; i++) {
			printf("%02X", *(qDataContainer.ptr + i));
		}
	}
	return os;
}

QDataContainer &QDataContainer::operator=(const QByteArray &qb) {
	deletePtr();
	ptr = new quint8[qb.size()];
	assert(ptr != nullptr);
	size = qb.size();
	std::copy(qb.begin(), qb.end(), ptr);
	return *this;
}

QByteArray QDataContainer::toQByteArray() const {
	if (ptr)
		return QByteArray((char*)ptr, size);
	return {};
}

void QDataContainer::deletePtr() {
	if (ptr) {
		delete ptr;
		ptr = nullptr;
		size = 0;
		capacity = 0;
	}
}

QDataContainer::QDataContainer(const QByteArray &qb) {

	capacity = qb.size();
	ptr = new quint8[capacity];
	assert(ptr != nullptr);
	size = capacity;
	std::copy(qb.begin(), qb.end(), ptr);
}

QByteArray QDataContainer::toQByteArrayAndDeletePtr() {
	QByteArray out = toQByteArray();
	deletePtr();
	return out;
}

QDataContainer::QDataContainer(quint8* src, size_t len) : capacity(len) {
	deletePtr();
	ptr = new quint8[capacity];
	assert(ptr != nullptr);
	size = capacity;
	std::copy(src, src + capacity, ptr);
}

QDataContainer::QDataContainer(size_t cap): capacity(cap) {
	ptr = new quint8[capacity];
	assert(ptr != nullptr);
}

QDataContainer::QDataContainer(const QString& str) {

	capacity = str.size();
	ptr = new quint8[capacity];
	assert(ptr != nullptr);
	size = capacity;
	std::copy(str.begin(), str.end(), ptr);
}
