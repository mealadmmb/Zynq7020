#include <iostream>
#include "fstream"
#include <optional>
#include <QString.h>
#include "QStringStream.h"
#include <QStringList.h>
#include <QFile.h>
#include <QByteArray.h>
#include <QHash.h>
#include <QDate.h>
#include <QDateTime.h>
#include <QVector.h>
#include <QTimer.h>
#include <QDebug.h>
#include <QElapsedTimer.h>
#include <QCircularBuffer.h>
#include <QDataBufferHolder.h>
#include <QTextStream.h>
#include <QUdpSocket.h>
#include <QDataStream.h>
#include <QSet.h>
#include <QList.h>
#include <QDataBuffer.h>
#include <QQueue.h>
#include <QTextFile.h>
#include <QDataContainer.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <exception>
#include <QCsvFileWriter.h>
#include <variant>
#include <QHostAddress.h>
#include <QSharedPointer.h>
#include <QUniquePointer.h>
#include "stdlib.h"



using namespace std;

void f (QVector<int> v) {
    QTime::delayMs(1000);
    cout << "v size: " << v.size() << ", v capacity: " << v.capacity() << endl;
    for(auto i = 0; i < v.size(); i++) {
        cout << "index [" << i << "] = " <<  v[i] << "\t";
    }
}

void g(const QHash<int, QString>& hash ) {
    cout << hash << endl;
}


QString NMEA = "$ARC,charmshahr,3.272;3.527;3.400;3.400*0.015:3.400*0.015:3.400*0.015:3.400*0.015:3.400*0.015:3.400*0.015:3.400*0.015:3.400*0.015:3.275*0.015:3.400*0.015:3.400*0.015:3.525*0.015,3,35.79211024:51.085555987:2518.0;10;50000;1,;;12.352;;;6,12;50;;,1:3:3:3:32:9:9:9:9:13:13:13;0,0:;1:;2:50*1,14725";
QString cfgRollRadar = "$CFG,Role=Radar,1197";
QString cfgRollAircraft = "$CFG,Role=Aircraft,1519";
QVector<int> vec(10, 0xAA);


uint8_t arr1[] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee};


void o(const void* p) {

    QString* str = (QString*)p;
    cout << *str;
}


class line : public QCsvFileWriter::QCsvLine {
public:
    QString col1;
    QString col2;
    QString get() const override {
        QString out;
        out <<
        col1 << sep <<
        col2;
        return out;
    }

};




class derived {
public:
    template<typename T, enable_if_t<is_signed<T>::value, bool> = true >
            void ff(T& u) {
                cout << "value: " << u << "\n";
            }
    template<typename T, enable_if_t<is_signed<T>::value, bool> = true >
            void operator>>(T& other) { other = i; }

            union {
                quint32 ui;
                qint32 i;
                float f;
                quint8 r[4];
            };
};


class A {
public:
    void method() {
        QDebug() << "method called";
    }
};






bool en = true;

int main() {
#if (en)
    QDebug() << "hello";
#endif
    return 0;

}

