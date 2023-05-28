#include "QFFS.h"
#ifdef QFF


QFFS::QFFS(QString driveLetter):
driveLetter(std::move(driveLetter))
{}

QString QFFS::getLetter() {
	return driveLetter;
}

quint64 QFFS::freeSpace() const {
	if(isMounted()) {
		quint64 out = 1;
		out *= fatfs.free_clst;
		out *= fatfs.csize;
		out *= FF_MAX_SS;
		return out;
	}
	else
		return 0;
}
quint64 QFFS::capacity() const {
	if(isMounted()) {
		quint64 out = 1;
		out *= (fatfs.n_fatent - 2);
		out *= fatfs.csize;
		out *= FF_MAX_SS;
		return out;
	}
	else
		return 0;
}

QFFS::~QFFS() {}

#endif
