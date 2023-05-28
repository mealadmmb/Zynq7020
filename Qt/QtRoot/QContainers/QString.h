#pragma once
#include <string>
#include <QVector.h>

class QStringList;
class QByteArray;
class QString;
struct QStringIterator {

	friend QString;

	using iterator_category = std::random_access_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = char;
	using pointer           = value_type*;  // or also value_type*
	using reference         = value_type&;  // or also value_type&

	explicit QStringIterator(typename std::string::iterator it) : it(it) {}

	void operator++(int) { // it++
		it++;
	}

	void operator++() { // ++it
		++it;
	}

	void operator--(int) { // it--
		it--;
	}

	void operator--() { // --it
		--it;
	}

	bool operator==(const QStringIterator& other) {
		return it == other.it;
	}

	bool operator!=(const QStringIterator& other) {
		return it != other.it;
	}

	reference operator*() {
		return it.operator*();
	}

	pointer operator->() {
		return it.operator->();
	}

private:
	typename std::string::iterator get_std_iterator(){
		return it;
	}

	typename std::string::iterator it;
};

struct QStringConstIterator {

	friend QString;

	using iterator_category = std::random_access_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = char;
	using pointer           = const value_type*;  // or also value_type*
	using reference         = const value_type&;  // or also value_type&

	explicit QStringConstIterator(typename std::string::const_iterator it) : it(it) {}

	void operator++(int) { // it++
		it++;
	}

	void operator++() { // ++it
		++it;
	}

	bool operator==(const QStringConstIterator& other) {
		return it == other.it;
	}

	bool operator!=(const QStringConstIterator& other) {
		return it != other.it;
	}

	reference operator*() {
		return it.operator*();
	}

	pointer operator->() {
		return it.operator->();
	}

private:
	typename std::string::const_iterator get_std_iterator(){
		return it;
	}

	typename std::string::const_iterator it;
};



class QString : public std::string {
public:
	using std::string::string;
	friend QString operator+(const char* c, const QString& str);

	QString() = default;
	QString(const char* c);
	QString(std::string s);
	explicit QString(const QByteArray& BA);

	// QStringIterator begin() {
	//     return QStringIterator(std::string::begin());
	// }
	// QStringIterator end() {
	//     return QStringIterator(std::string::end());
	// }

	// const QStringConstIterator constBegin() const {
	//     return QStringConstIterator(std::string::cbegin());
	// }
	// const QStringConstIterator constEnd() const {
	//     return QStringConstIterator(std::string::cend());
	// }

	QString& append(const QByteArray& arr);

	template<class T>
	QString& append(const T& t) {
		std::string::append(t);
		return *this;
	}

	QStringList split(const QString& sep, bool SkipEmptyParts = false) const;
	QStringList split(char sep, bool SkipEmptyParts = false) const;
	int indexOf(char c) const;
	QVector<int> indexesOf (char c) const;
	QString chop(int n) const;
	QString mid(int pos, int n = -1) const;
	QString left(size_t index) const;
	QString right(size_t index) const;
	bool startsWith(const QString& startWith) const;
	int toInt(bool* stat) const;
	void remove(const QString& str);
	void remove(char c);
	uint64_t toULongLong(bool* stat) const;
	double toDouble(bool* stat) const;
	QString leftJustified(size_t width, char fill = ' ', bool trunc = false ) const;
	QString rightJustified(size_t width, char fill = ' ', bool trunc = false ) const;
	bool contains(const QString &str, bool caseSensitivity = false) const;
	QByteArray toUtf8() const;
	uint64_t checksum() const;



	QString& operator=(std::string& str);
	QString& operator=(const char* c);
	QString& operator=(const QByteArray& array);


	QString& operator<<(const QString& str);
	QString& operator<<(const size_t & i);
	QString& operator<<(const QByteArray& arr);


	QString operator+(const QString& str);
	QString operator+(const char* c);

	static QString	number(int n, int base = 10);
	static QString	number(unsigned int n, int base = 10);

	static QString	number(long n, int base = 10);
	static QString	number(unsigned long n, int base = 10);
	static QString	number(long long n, int base = 10);
	static QString	number(unsigned long long n, int base = 10);

	static QString	number(double n, char format = 'g', int precision = 6);

	QString reverse() const;

	QString toLower() const;
	QString toUpper() const;

	QByteArray toHex() const;

	std::string toStdString() const;
	static QString fromStdString(const std::string &str);

	bool isEmpty() const;

private:
	template<class T>
	static inline QString anyDigitToString(T num, int base = 10) {
		QString out;
		bool isNegative = false;

		/* Handle 0 explicitely, otherwise empty string is printed for 0 */
		if (num == 0)
			return QString("0");

		// In standard itoa(), negative numbers are handled only with
		// base 10. Otherwise numbers are considered unsigned.
		if (num < 0 && base == 10)
		{
			isNegative = true;
			num = -num;
		}

		// Process individual digits
		while (num != 0)
		{
			int rem = num % base;
			out.push_back((rem > 9)? (rem-10) + 'a' : rem + '0');
			num = num/base;
		}

		// If number is negative, append '-'
		if (isNegative)
			out.push_back('-');
		// Reverse the string

//		switch (base) {
//		case 2:
//			out += "b0";
//			break;
//		case 8:
//			out += "c0";
//			break;
//		case 10:
//			break;
//		case 16:
//			out += "x0";
//			break;
//		}
		return out.reverse();
	}





};


//QString operator+(const char* c, const QString& str);

#define QStringLiteral(str) QString(str)

namespace std {
template <> struct hash<QString> {
	size_t operator()(const QString & x) const {
		std::hash<std::string> h;
		return h(x);

		/*
            const std::string* pointer = &x;
            return h(*pointer);
		 */
	}
};
}


