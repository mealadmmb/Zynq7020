#include <QDate.h>
#include <exception>
class BadDate: public std::exception
{
};
QDate::QDate(int _y, int _m, int _d):
		year(_y), month(_m), day(_d)
{
	if(!QDate::isValid(year, month, day))
		throw BadDate();

}

bool QDate::isLeapYear(int year) {
	return (year % 400 == 0) && (year % 100 == 0);
}

bool QDate::isValid(int year, int month, int day) {
	return true;
}

QDate QDate::addDays(int64_t ndays) {
	return QDate();
}

