#include <QHostAddress.h>
#include <QByteArray.h>
#include <QStringList.h>
#include <iostream>
#include <utility>
QHostAddress::QHostAddress(SpecialAddress address) {
	switch(address) {
	case Any: {
		Address = "";
		break;

	default:

		break;
	}

	}
}

QHostAddress::QHostAddress(QString address) :
		Address(std::move(address))
{
}

QByteArray QHostAddress::toByteArray() const {
	QByteArray out;
	QStringList bytesList = Address.split('.');
	for(int i = bytesList.size() - 1; i >= 0 ; i--) {
		bool ok = false;
		char byte = bytesList[i].toInt(&ok) & 0xFF;
		if(ok)
			out.push_back(byte);
		else
			return QByteArray();
	}

	return out;
}


std::ostream &operator<<(std::ostream &os, const QHostAddress &qHostAddress) {
	if(qHostAddress.toString().isEmpty()) {
		std::cout << "empty QHostAddress\n";
		return os;
	}
	std::cout << qHostAddress.toString();
	return os;
}

QString QHostAddress::toString() const {
	return this->Address;
}
