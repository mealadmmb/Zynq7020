
#include <FreeRTOS.h>
#include <task.h>
#include <xNetwork/xNetwork.h>
#include "netif/xadapter.h"
#include "lwip/init.h"
#include "lwip/inet.h"
#include "lwip/sys.h"
#if LWIP_DHCP==1
#include "lwip/dhcp.h"
#endif

#include <QString.h>
#include <QByteArray.h>



#if LWIP_DHCP==1
static void dhcpTaskHandler(struct netif *netif) {
	int mscnt = 0;
	dhcp_start(netif);

	printf("dhcpTaskHandler started\n");
	while (1) {
		vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
		dhcp_fine_tmr();
		mscnt += DHCP_FINE_TIMER_MSECS;
		if (mscnt >= DHCP_COARSE_TIMER_SECS*1000) {
			dhcp_coarse_tmr();
			mscnt = 0;
		}
	}
}
#endif

xNetwork::xNetwork() {
	internalNetif = new netif();
}

xNetwork::~xNetwork() {
	if(internalNetif) delete internalNetif;
}

bool xNetwork::init(const QString& mac, const QString& ip, const QString& netmask, const QString& gw, bool dhcpEnable) {

	enabled = true;
	if(mac.isEmpty()) return false; // no MAC address configured
	if(ip.isEmpty() || netmask.isEmpty() || gw.isEmpty()) return false; // no IPs configured to assign
	if(dhcpEnable && LWIP_DHCP != 1) return false; // dhcp not configured in bsp configuration


	/* initialize lwIP before calling sys_thread_new */
	lwip_init();

	if(!configPHY(mac)) return false;


	if(dhcpEnable) {
#if LWIP_DHCP==1
		sys_thread_new("dhcpTaskHandler",
				(void(*)(void*))dhcpTaskHandler, internalNetif,
				1024 * 1024, DEFAULT_THREAD_PRIO);
		int mscnt = 0;
		while (1) {
			vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
			if (internalNetif->ip_addr.addr) {
				if(debug) printf("DHCP request success\n");
				break;
			}
			mscnt += DHCP_FINE_TIMER_MSECS;
			if (mscnt >= 10000) {
				if(debug) printf("ERROR: DHCP request timed out\n");
				asignDefaultIPs(ip, netmask, gw);
				break;
			}
		}
#endif
	}
	else asignDefaultIPs(ip, netmask, gw);
	inited = true;
	return true;
}


bool xNetwork::configPHY(const QString& macString) {

	/* Add network interface to the netif_list, and set it as default */
	QByteArray mac = stringToMac(macString);
	if(mac.isEmpty()) return false;
	if (!xemac_add(internalNetif, NULL, NULL, NULL, (unsigned char*)mac.data(),
			XPAR_XEMACPS_0_BASEADDR)) {
		if(debug) printf("Error adding N/W interface\n");
		return false;
	}

	netif_set_default(internalNetif);

	/* specify that the network if is up */
	netif_set_up(internalNetif);


	/* start packet receive thread - required for lwIP operation */
	sys_thread_new("xemacif_input_thread",
			(void(*)(void*))xemacif_input_thread, internalNetif,
			1024 * 1024, DEFAULT_THREAD_PRIO);
	return true;
}

QByteArray xNetwork::stringToMac(const QString& mac) {
	QByteArray out;
	QString macString = mac;
	// check format by size. mac string has to be in such format "XX:XX:XX:XX:XX:XX"
	if(macString.size() != 17) return out;
	macString.remove(":");
	return macString.toHex();
}

bool xNetwork::asignDefaultIPs(const QString& ip, const QString& netmask, const QString& gw) {
	if(inet_aton(ip.c_str(), &internalNetif->ip_addr) == 0) return false;
	if(inet_aton(netmask.c_str(), &internalNetif->netmask) == 0) return false;
	if(inet_aton(gw.c_str(), &internalNetif->gw) == 0) return false;
	return true;
}


QString xNetwork::getIP() const {
	QString ip;
	ip <<
			ip4_addr1(&internalNetif->ip_addr) << "." <<
			ip4_addr2(&internalNetif->ip_addr) << "." <<
			ip4_addr3(&internalNetif->ip_addr) << "." <<
			ip4_addr4(&internalNetif->ip_addr);
	return ip;
}
QString xNetwork::getNetmask() const {
	QString netmask;
	netmask <<
			ip4_addr1(&internalNetif->netmask) << "." <<
			ip4_addr2(&internalNetif->netmask) << "." <<
			ip4_addr3(&internalNetif->netmask) << "." <<
			ip4_addr4(&internalNetif->netmask);
	return netmask;

}
QString xNetwork::getGw() const {
	QString gw;
	gw <<
			ip4_addr1(&internalNetif->gw) << "." <<
			ip4_addr2(&internalNetif->gw) << "." <<
			ip4_addr3(&internalNetif->gw) << "." <<
			ip4_addr4(&internalNetif->gw);
	return gw;
}

void xNetwork::waitUntilReady() {
	while(!isReady() && enabled) vTaskDelay(pdMS_TO_TICKS(1000));
}


