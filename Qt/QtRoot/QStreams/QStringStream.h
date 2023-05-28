#pragma once
#include <QStringStream.h>

class QStringStream : public QString {
public:
	using QString::QString;
	explicit QStringStream(QString* str): QString(*str){};

};
