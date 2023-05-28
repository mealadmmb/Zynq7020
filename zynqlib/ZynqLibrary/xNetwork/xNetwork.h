#pragma once
#include <QSingleton.h>

struct netif;
class QString;
class QByteArray;

class xNetwork : public QSingleton<xNetwork>  {
	friend class QSingleton<xNetwork>;
	xNetwork();
	~xNetwork();
public:
	QString getIP() const;
	QString getNetmask() const;
	QString getGw() const;
	bool isReady() const { return inited; }
	void waitUntilReady();
	bool init(const QString& mac, const QString& ip, const QString& netmask, const QString& gw, bool _dhcpEnable = true);

private:
	bool inited = false;
	bool debug = true;
	bool enabled = false;
	netif* internalNetif;

	QByteArray stringToMac(const QString& mac);
	bool configPHY(const QString& macString);
	bool asignDefaultIPs(const QString& ip, const QString& netmask, const QString& gw);
};
