#pragma once
#include <QVector.h>
#include <algorithm>
#include <string>

class QString;

class QByteArray : public QVector<char> {
public:
	using QVector<char>::QVector;
	explicit QByteArray(int size);
	explicit QByteArray(const char* data, size_t size);
	QByteArray(int size, char value);
	QByteArray() = default;
	size_t indexOf(char c) const;
	QByteArray mid(int pos, int n = -1) const;
	QByteArray left(size_type index) const;
	QByteArray right(size_type index) const;
	QString toHexString() const;
	QByteArray toHex(char separator = -1) const;
	void printHex() const;
	QByteArray toLower() const;
	QByteArray toUpper() const;
	QString toQString() const;

	QByteArray& append(const char *c, size_t len);
	QByteArray& append(const char c);
	QByteArray& append(const QString& str);
	QByteArray& append(const std::string& str);
	QByteArray& append(const QByteArray& arr);

	inline const char* constData() const {
		return this->data();
	}


	QByteArray& operator<<(const QString& str);

	friend std::ostream& operator<<(std::ostream& os, const QByteArray& qByteArray);

private:


};
