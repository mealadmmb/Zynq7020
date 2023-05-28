#pragma once
#include <QtGlobal.h>
#include <QByteArray.h>
#include <QString.h>
#include <algorithm>
#include <iostream>



#pragma pack(1)
struct QDataContainer {
	friend class QDataBuffer;
	friend class QCircularBuffer;
public:
	QDataContainer() {}
	QDataContainer(size_t cap);
	QDataContainer(quint8* src, size_t len);
	QDataContainer(const QByteArray& qb);
	QDataContainer(const QString& str);
	QDataContainer& operator=(const QByteArray& qb);
	QByteArray toQByteArray() const;
	QByteArray toQByteArrayAndDeletePtr();
	void deletePtr();
	const quint8* getPtr()const  { return ptr; }
	size_t getSize() const { return size; }
	size_t getCapacity() { return capacity; }

	friend std::ostream& operator<<(std::ostream& os, const QDataContainer& qDataContainer);
	//private:
	size_t capacity;
	quint8 * ptr = nullptr;
	size_t size = 0;
};
#pragma pack()
