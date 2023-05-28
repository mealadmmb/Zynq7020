#include  <QVector.h>
#include <iostream>

template<class T>
std::ostream& operator<<(std::ostream& os, const QVector<T>& v) {
	if(v.size()) {
		for (auto item : v) {
			std::cout << "\"" << item << "\"";
		}
	}
	return os;
}
