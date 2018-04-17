/*
 * WiFi.h
 *
 */

#ifndef MAIN_WIFI_H_
#define MAIN_WIFI_H_

/*#include <string>
#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#define MESSAGE "Hello TCP Client!!"
#define LISTENQ 2

static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
static const char *TAG="sta_mode_tcp_server";

#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define WIFI_CONNECTED_BIT BIT0

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
	WiFi(bool initAsStation, bool initAsAP, std::string ssid, std::string password)
	{
		bool isConnected = false;
		if(initAsStation)
		{
			isConnected = connectTo(ssid, password);
		}
		if(!isConnected && initAsAP)
		{
			isConnected = createAP(ssid, password);
			//assert(isConnected);
		}
	}

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



	static void printWiFiIP(void *pvParam){
	    printf("print_WiFiIP task started \n");
	    xEventGroupWaitBits(wifi_event_group,CONNECTED_BIT,false,true,portMAX_DELAY);
	    tcpip_adapter_ip_info_t ip_info;
		ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
	    printf("IP :  %s\n", ip4addr_ntoa(&ip_info.ip));
	    vTaskDelete( NULL );
	}

	static void tcp_server(void *pvParam){
	    ESP_LOGI(TAG,"tcp_server task started \n");
	    struct sockaddr_in tcpServerAddr;
	    tcpServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	    tcpServerAddr.sin_family = AF_INET;
	    tcpServerAddr.sin_port = htons( 3000 );
	    int s, r;
	    char recv_buf[64];
	    static struct sockaddr_in remote_addr;
	    static unsigned int socklen;
	    socklen = sizeof(remote_addr);
	    int cs;//client socket
	    xEventGroupWaitBits(wifi_event_group,CONNECTED_BIT,false,true,portMAX_DELAY);
	    while(1){
	        s = socket(AF_INET, SOCK_STREAM, 0);
	        if(s < 0) {
	            ESP_LOGE(TAG, "... Failed to allocate socket.\n");
	            vTaskDelay(1000 / portTICK_PERIOD_MS);
	            continue;
	        }
	        ESP_LOGI(TAG, "... allocated socket\n");
	         if(bind(s, (struct sockaddr *)&tcpServerAddr, sizeof(tcpServerAddr)) != 0) {
	            ESP_LOGE(TAG, "... socket bind failed errno=%d \n", errno);
	            close(s);
	            vTaskDelay(4000 / portTICK_PERIOD_MS);
	            continue;
	        }
	        ESP_LOGI(TAG, "... socket bind done \n");
	        if(listen (s, LISTENQ) != 0) {
	            ESP_LOGE(TAG, "... socket listen failed errno=%d \n", errno);
	            close(s);
	            vTaskDelay(4000 / portTICK_PERIOD_MS);
	            continue;
	        }
	        while(1){
	            cs=accept(s,(struct sockaddr *)&remote_addr, &socklen);
	            ESP_LOGI(TAG,"New connection request,Request data:");
	            //set O_NONBLOCK so that recv will return, otherwise we need to impliment message end
	            //detection logic. If know the client message format you should instead impliment logic
	            //detect the end of message
	            fcntl(cs,F_SETFL,O_NONBLOCK);
	            do {
	                bzero(recv_buf, sizeof(recv_buf));
	                r = recv(cs, recv_buf, sizeof(recv_buf)-1,0);
	                for(int i = 0; i < r; i++) {
	                    putchar(recv_buf[i]);
	                }
	            } while(r > 0);

	            ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);

	            if( write(cs , MESSAGE , strlen(MESSAGE)) < 0)
	            {
	                ESP_LOGE(TAG, "... Send failed \n");
	                close(s);
	                vTaskDelay(4000 / portTICK_PERIOD_MS);
	                continue;
	            }
	            ESP_LOGI(TAG, "... socket send success");
	            close(cs);
	        }
	        ESP_LOGI(TAG, "... server will be opened in 5 seconds");
	        vTaskDelay(5000 / portTICK_PERIOD_MS);
	    }
	    ESP_LOGI(TAG, "...tcp_client task closed\n");
	}
};

void foofoo()
{
	ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
	WiFi wifi(true, false, "ssid", "password");
	//wifi_event_group = xEventGroupCreate();
	xTaskCreate(&(wifi.printWiFiIP),"printWiFiIP",2048,NULL,5,NULL);
	xTaskCreate(&(wifi.tcp_server),"tcp_server",4096,NULL,5,NULL);
}*/





#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

void manage_connection(void *param);

#define AP_SSID_HIDDEN 0
#define AP_MAX_CONNECTIONS 4
#define AP_AUTHMODE WIFI_AUTH_WPA2_PSK // the passpharese should be atleast 8 chars long
#define AP_BEACON_INTERVAL 100 // in milli seconds
#define LISTENQ 2
#define MESSAGE "Hello TCP Client!!"
static EventGroupHandle_t wifi_event_group;
const int CLIENT_CONNECTED_BIT = BIT0;
const int CLIENT_DISCONNECTED_BIT = BIT1;
const int AP_STARTED_BIT = BIT2;

static const char *TAG="ap_mode_tcp_server";
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_AP_START:
		printf("Event:ESP32 is started in AP mode\n");
        xEventGroupSetBits(wifi_event_group, AP_STARTED_BIT);
        break;

	case SYSTEM_EVENT_AP_STACONNECTED:
		xEventGroupSetBits(wifi_event_group, CLIENT_CONNECTED_BIT);
		break;

	case SYSTEM_EVENT_AP_STADISCONNECTED:
		xEventGroupSetBits(wifi_event_group, CLIENT_DISCONNECTED_BIT);
		break;
    default:
        break;
    }
    return ESP_OK;
}

class WiFi
{
public:
	WiFi(bool initAsStation, bool initAsAP, std::string ssid, std::string password)
	{
		startWiFi();

		bool isConnected = false;
		if(initAsStation)
		{
			isConnected = connectTo(ssid, password);
		}
		if(!isConnected && initAsAP)
		{
			isConnected = createAP(ssid, password);
			//assert(isConnected);
		}
	}

	bool connectTo(std::string ssid, std::string pwd, int attempts = 3)
	{
		// unsupported
		return false;
	}

	static void start_dhcp_server(){

	    	// initialize the tcp stack
		    tcpip_adapter_init();
	        // stop DHCP server
	        ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
	        // assign a static IP to the network interface
	        tcpip_adapter_ip_info_t info;
	        memset(&info, 0, sizeof(info));
	        IP4_ADDR(&info.ip, 192, 168, 1, 1);
	        IP4_ADDR(&info.gw, 192, 168, 1, 1);//ESP acts as router, so gw addr will be its own addr
	        IP4_ADDR(&info.netmask, 255, 255, 255, 0);
	        ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));
	        // start the DHCP server
	        ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));
	        printf("DHCP server started \n");
	}

	static bool createAP(std::string ssid, std::string pwd)
	{
	    esp_log_level_set("wifi", ESP_LOG_NONE); // disable wifi driver logging
	    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );

	    // configure the wifi connection and start the interface
		wifi_config_t ap_config;
	    strcpy((char*)ap_config.ap.ssid, ssid.c_str());
	    strcpy((char*)ap_config.ap.password, pwd.c_str());
		ap_config.ap.ssid_len = 0;
		ap_config.ap.channel = 0;
		ap_config.ap.authmode = AP_AUTHMODE;
		ap_config.ap.ssid_hidden = AP_SSID_HIDDEN;
		ap_config.ap.max_connection = AP_MAX_CONNECTIONS;
		ap_config.ap.beacon_interval = AP_BEACON_INTERVAL;
		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
	    ESP_ERROR_CHECK( esp_wifi_start() );
	    printf("ESP WiFi started in AP mode \n");

		xTaskCreate(&(WiFi::tcp_server),"tcp_server",4096,NULL,5,NULL);
		xTaskCreate(&(WiFi::print_sta_info),"print_sta_info",4096,NULL,5,NULL);
	}

	static void tcp_server(void *pvParam){
	    ESP_LOGI(TAG,"tcp_server task started \n");
	    struct sockaddr_in tcpServerAddr;
	    tcpServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	    tcpServerAddr.sin_family = AF_INET;
	    tcpServerAddr.sin_port = htons( 3000 );
	    int s, r;
	    char recv_buf[64];
	    static struct sockaddr_in remote_addr;
	    static unsigned int socklen;
	    socklen = sizeof(remote_addr);
	    int cs;//client socket
	    xEventGroupWaitBits(wifi_event_group,AP_STARTED_BIT,false,true,portMAX_DELAY);
	    while(1){
	        s = socket(AF_INET, SOCK_STREAM, 0);
	        if(s < 0) {
	            ESP_LOGE(TAG, "... Failed to allocate socket.\n");
	            vTaskDelay(1000 / portTICK_PERIOD_MS);
	            continue;
	        }
	        ESP_LOGI(TAG, "... allocated socket\n");
	         if(bind(s, (struct sockaddr *)&tcpServerAddr, sizeof(tcpServerAddr)) != 0) {
	            ESP_LOGE(TAG, "... socket bind failed errno=%d \n", errno);
	            close(s);
	            vTaskDelay(4000 / portTICK_PERIOD_MS);
	            continue;
	        }
	        ESP_LOGI(TAG, "... socket bind done \n");
	        if(listen (s, LISTENQ) != 0) {
	            ESP_LOGE(TAG, "... socket listen failed errno=%d \n", errno);
	            close(s);
	            vTaskDelay(4000 / portTICK_PERIOD_MS);
	            continue;
	        }
	        while(1){
	            cs=accept(s,(struct sockaddr *)&remote_addr, &socklen);
	            ESP_LOGI(TAG,"New connection request,Request data:");
	            //set O_NONBLOCK so that recv will return, otherwise we need to impliment message end
	            //detection logic. If know the client message format you should instead impliment logic
	            //detect the end of message
	            fcntl(cs,F_SETFL,O_NONBLOCK);
	            do {
	                bzero(recv_buf, sizeof(recv_buf));
	                r = recv(cs, recv_buf, sizeof(recv_buf)-1,0);
	                for(int i = 0; i < r; i++) {
	                    putchar(recv_buf[i]);
	                }
	            } while(r > 0);

	            ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);

	            if( write(cs , MESSAGE , strlen(MESSAGE)) < 0)
	            {
	                ESP_LOGE(TAG, "... Send failed \n");
	                close(s);
	                vTaskDelay(4000 / portTICK_PERIOD_MS);
	                continue;
	            }

                xTaskCreate(&manage_connection,"manage_connection",4096,&cs,5,NULL);

	            ESP_LOGI(TAG, "... socket send success");
	            //close(cs);
	        }
	        ESP_LOGI(TAG, "... server will be opened in 5 seconds");
	        vTaskDelay(5000 / portTICK_PERIOD_MS);
	    }
	    ESP_LOGI(TAG, "...tcp_client task closed\n");
	}

	// print the list of connected stations
	static void printStationList()
	{
		printf(" Connected stations:\n");
		printf("--------------------------------------------------\n");

		wifi_sta_list_t wifi_sta_list;
		tcpip_adapter_sta_list_t adapter_sta_list;

		memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
		memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));

		ESP_ERROR_CHECK(esp_wifi_ap_get_sta_list(&wifi_sta_list));
		ESP_ERROR_CHECK(tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list));

		for(int i = 0; i < adapter_sta_list.num; i++) {

			tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
	         printf("%d - mac: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x - IP: %s\n", i + 1,
					station.mac[0], station.mac[1], station.mac[2],
					station.mac[3], station.mac[4], station.mac[5],
					ip4addr_ntoa(&(station.ip)));
		}

		printf("\n");
	}

	static void print_sta_info(void *pvParam){
	    printf("print_sta_info task started \n");
	    while(1) {
			EventBits_t staBits = xEventGroupWaitBits(wifi_event_group, CLIENT_CONNECTED_BIT | CLIENT_CONNECTED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
			if((staBits & CLIENT_CONNECTED_BIT) != 0) printf("New station connected\n\n");
	        else printf("A station disconnected\n\n");
	        printStationList();
		}
	}

	void startWiFi()
	{
	    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
	    wifi_event_group = xEventGroupCreate();
	    start_dhcp_server();
	}
};

#endif /* MAIN_WIFI_H_ */
