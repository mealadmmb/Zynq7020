#pragma once
#include <cstdint>
class QDate{
public:
	QDate(int year, int month, int day);
	QDate() = default;
	static bool isLeapYear(int year);
	static bool isValid(int year, int month, int day);
	QDate addDays(int64_t ndays);

private:
	int year, month, day;
};
