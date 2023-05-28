#include <QStringList.h>
#include <iostream>
#include <utility>


QStringList& QStringList::operator<<(const QString& str) {
	this->append(str);
	return *this;
}


std::ostream &operator<<(std::ostream &os, const QStringList &qStringList) {
	if(qStringList.empty())
		std::cout << "empty QStringList\n";
	else
		for(const auto& it : qStringList)
			std::cout << it << std::endl;
	return os;
}

QString& operator<< (QString& str, const QStringList& list) {

	if (list.empty()) {
		str << "empty QStringList\n";
	}
	else{
		str.reserve(str.size() + 100);
		for(const auto& item : list){
			str << item << "\n";
		}
	}
	return str;
}
