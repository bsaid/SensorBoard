/*
 * WiFi.h
 *
 */

#ifndef MAIN_WIFI_H_
#define MAIN_WIFI_H_

#include <string>
#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define WIFI_CONNECTED_BIT BIT0

/* FreeRTOS event group to signal when we are connected to wifi */
EventGroupHandle_t tcp_event_group;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	switch (event->event_id) {
	case SYSTEM_EVENT_STA_START:
		esp_wifi_connect();
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		esp_wifi_connect();
		xEventGroupClearBits(tcp_event_group, WIFI_CONNECTED_BIT);
		break;
	case SYSTEM_EVENT_STA_CONNECTED:
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		ESP_LOGI("WiFi:", "got ip:%s\n",
				 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
		xEventGroupSetBits(tcp_event_group, WIFI_CONNECTED_BIT);
		break;
	case SYSTEM_EVENT_AP_STACONNECTED:
		ESP_LOGI("WiFi:", "station:" MACSTR " join,AID=%d\n",
				 MAC2STR(event->event_info.sta_connected.mac),
				 event->event_info.sta_connected.aid);
		xEventGroupSetBits(tcp_event_group, WIFI_CONNECTED_BIT);
		break;
	case SYSTEM_EVENT_AP_STADISCONNECTED:
		ESP_LOGI("WiFi:", "station:" MACSTR "leave,AID=%d\n",
				 MAC2STR(event->event_info.sta_disconnected.mac),
				 event->event_info.sta_disconnected.aid);
		xEventGroupClearBits(tcp_event_group, WIFI_CONNECTED_BIT);
		break;
	default:
		break;
	}
	return ESP_OK;
}

class WiFi
{
	const int MAX_STA_CONN  = 10;

public:
	WiFi(){}

	bool connectTo(std::string ssid, std::string pwd, int attempts = 3)
	{
		tcp_event_group = xEventGroupCreate();

		tcpip_adapter_init();
		ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

		wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
		ESP_ERROR_CHECK(esp_wifi_init(&cfg));
		wifi_config_t wifi_config;
		strcpy( (char*)wifi_config.sta.ssid, ssid.c_str());
		strcpy( (char*)wifi_config.sta.password, pwd.c_str());

		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
		ESP_ERROR_CHECK(esp_wifi_start() );

		ESP_LOGI("WiFi:", "wifi_init_sta finished.");
		ESP_LOGI("WiFi:", "connect to ap SSID:%s password:%s \n", ssid.c_str(), pwd.c_str());
		return false;
	}

	bool createAP(std::string ssid, std::string pwd)
	{
		tcp_event_group = xEventGroupCreate();

		tcpip_adapter_init();
		ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

		wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
		ESP_ERROR_CHECK(esp_wifi_init(&cfg));
		wifi_config_t wifi_config;
		strcpy((char*)wifi_config.ap.ssid, ssid.c_str());
		strcpy((char*)wifi_config.ap.password, pwd.c_str());
		wifi_config.ap.ssid_len = 0;
		wifi_config.ap.max_connection = MAX_STA_CONN;
		wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
		if (pwd.size() == 0) {
			wifi_config.ap.authmode = WIFI_AUTH_OPEN;
		}

		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
		ESP_ERROR_CHECK(esp_wifi_start());

		ESP_LOGI("WiFi:", "wifi_init_softap finished.SSID:%s password:%s \n", ssid.c_str(), pwd.c_str());
		return false;
	}
};

#endif /* MAIN_WIFI_H_ */
