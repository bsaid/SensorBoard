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
#include "esp_err.h"
#include <errno.h>

#ifndef EX_CONF
#define EX_CONF
#define CONFIG_TCP_PERF_WIFI_MODE_AP 1 //TRUE:AP FALSE:STA
#define CONFIG_TCP_PERF_SERVER 1 //TRUE:server FALSE:client
#define CONFIG_TCP_PERF_TX 1 //TRUE:send FALSE:receive
#define CONFIG_TCP_PERF_DELAY_DEBUG 1 //TRUE:show delay time info
#define CONFIG_TCP_PERF_WIFI_SSID "esp"
#define CONFIG_TCP_PERF_WIFI_PWD "1234567890"
#define CONFIG_TCP_PERF_SERVER_PORT 4567
#define CONFIG_TCP_PERF_PKT_SIZE 1460
#endif

/*test options*/
#define EXAMPLE_ESP_WIFI_MODE_AP CONFIG_TCP_PERF_WIFI_MODE_AP //TRUE:AP FALSE:STA
#define EXAMPLE_ESP_TCP_MODE_SERVER CONFIG_TCP_PERF_SERVER //TRUE:server FALSE:client
#define EXAMPLE_ESP_TCP_PERF_TX CONFIG_TCP_PERF_TX //TRUE:send FALSE:receive
#define EXAMPLE_ESP_TCP_DELAY_INFO CONFIG_TCP_PERF_DELAY_DEBUG //TRUE:show delay time info

/*AP info and tcp_server info*/
#define EXAMPLE_DEFAULT_SSID CONFIG_TCP_PERF_WIFI_SSID
#define EXAMPLE_DEFAULT_PWD CONFIG_TCP_PERF_WIFI_PASSWORD
#define EXAMPLE_DEFAULT_PORT CONFIG_TCP_PERF_SERVER_PORT
#define EXAMPLE_DEFAULT_PKTSIZE CONFIG_TCP_PERF_PKT_SIZE
#define EXAMPLE_MAX_STA_CONN 1 //how many sta can be connected(AP mode)

#ifdef CONFIG_TCP_PERF_SERVER_IP
#define EXAMPLE_DEFAULT_SERVER_IP CONFIG_TCP_PERF_SERVER_IP
#else
#define EXAMPLE_DEFAULT_SERVER_IP "192.168.4.1"
#endif /*CONFIG_TCP_PERF_SERVER_IP*/



#define EXAMPLE_PACK_BYTE_IS 97 //'a'
#define TAG "tcp_perf:"

#define WIFI_CONNECTED_BIT BIT0





/* FreeRTOS event group to signal when we are connected to wifi */
EventGroupHandle_t tcp_event_group;

/*socket*/
static int server_socket = 0;
static struct sockaddr_in server_addr;
static struct sockaddr_in client_addr;
static unsigned int socklen = sizeof(client_addr);
static int connect_socket = 0;
bool g_rxtx_need_restart = false;

int g_total_data = 0;

#if EXAMPLE_ESP_TCP_PERF_TX && EXAMPLE_ESP_TCP_DELAY_INFO

int g_total_pack = 0;
int g_send_success = 0;
int g_send_fail = 0;
int g_delay_classify[5] = { 0 };

#endif /*EXAMPLE_ESP_TCP_PERF_TX && EXAMPLE_ESP_TCP_DELAY_INFO*/




class WiFi
{

public:
	static bool connectTo(std::string ssid, std::string pwd, int attempts = 3)
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

	static bool createAP(std::string ssid, std::string pwd)
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
		wifi_config.ap.max_connection = EXAMPLE_MAX_STA_CONN;
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
	        ESP_LOGI(TAG, "got ip:%s\n",
	                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
	        xEventGroupSetBits(tcp_event_group, WIFI_CONNECTED_BIT);
	        break;
	    case SYSTEM_EVENT_AP_STACONNECTED:
	        ESP_LOGI(TAG, "station:" MACSTR " join,AID=%d\n",
	                 MAC2STR(event->event_info.sta_connected.mac),
	                 event->event_info.sta_connected.aid);
	        xEventGroupSetBits(tcp_event_group, WIFI_CONNECTED_BIT);
	        break;
	    case SYSTEM_EVENT_AP_STADISCONNECTED:
	        ESP_LOGI(TAG, "station:" MACSTR "leave,AID=%d\n",
	                 MAC2STR(event->event_info.sta_disconnected.mac),
	                 event->event_info.sta_disconnected.aid);
	        xEventGroupClearBits(tcp_event_group, WIFI_CONNECTED_BIT);
	        break;
	    default:
	        break;
	    }
	    return ESP_OK;
	}

	//send data
	static void send_data(/*void *pvParameters*/)
	{
	    int len = 0;
	    char *databuff = (char *)malloc(EXAMPLE_DEFAULT_PKTSIZE * sizeof(char));
	    memset(databuff, EXAMPLE_PACK_BYTE_IS, EXAMPLE_DEFAULT_PKTSIZE);
	    vTaskDelay(100 / portTICK_RATE_MS);
	    ESP_LOGI(TAG, "start sending...");
	#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO
	    //delaytime
	    struct timeval tv_start;
	    struct timeval tv_finish;
	    unsigned long send_delay_ms;
	#endif /*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/
	    while (1) {
	        int to_write = EXAMPLE_DEFAULT_PKTSIZE;

	#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO
	        g_total_pack++;
	        gettimeofday(&tv_start, NULL);
	#endif /*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/

	        //send function
	        while (to_write > 0) {
	            len = send(connect_socket, databuff + (EXAMPLE_DEFAULT_PKTSIZE - to_write), to_write, 0);
	            if (len > 0) {
	                g_total_data += len;
	                to_write -= len;
	            } else {
	                int err = get_socket_error_code(connect_socket);
	#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO
	                g_send_fail++;
	#endif /*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/

	                if (err != ENOMEM) {
	                    show_socket_error_reason("send_data", connect_socket);
	                    break;
	                }
	            }
	        }

	#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO
	        gettimeofday(&tv_finish, NULL);
	#endif /*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/
	        if (g_total_data > 0) {
	#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO
	            g_send_success++;
	            send_delay_ms = (tv_finish.tv_sec - tv_start.tv_sec) * 1000
	                            + (tv_finish.tv_usec - tv_start.tv_usec) / 1000;
	            if (send_delay_ms < 30) {
	                g_delay_classify[0]++;
	            } else if (send_delay_ms < 100) {
	                g_delay_classify[1]++;
	            } else if (send_delay_ms < 300) {
	                g_delay_classify[2]++;
	            } else if (send_delay_ms < 1000) {
	                g_delay_classify[3]++;
	            } else {
	                g_delay_classify[4]++;
	            }
	#endif /*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/
	        } else {
	            break;
	        }
	    }
	    g_rxtx_need_restart = true;
	    free(databuff);
	    vTaskDelete(NULL);
	}

	//receive data
	static void recv_data(void *pvParameters)
	{
	    int len = 0;
	    char *databuff = (char *)malloc(EXAMPLE_DEFAULT_PKTSIZE * sizeof(char));
	    while (1) {
	        int to_recv = EXAMPLE_DEFAULT_PKTSIZE;
	        while (to_recv > 0) {
	            len = recv(connect_socket, databuff + (EXAMPLE_DEFAULT_PKTSIZE - to_recv), to_recv, 0);
	            if (len > 0) {
	                g_total_data += len;
	                to_recv -= len;
	            } else {
	                show_socket_error_reason("recv_data", connect_socket);
	                break;
	            }
	        }
	        if (g_total_data > 0) {
	            continue;
	        } else {
	            break;
	        }
	    }

	    g_rxtx_need_restart = true;
	    free(databuff);
	    vTaskDelete(NULL);
	}


	//use this esp32 as a tcp server. return ESP_OK:success ESP_FAIL:error
	static esp_err_t create_tcp_server()
	{
	    ESP_LOGI(TAG, "server socket....port=%d\n", EXAMPLE_DEFAULT_PORT);
	    server_socket = socket(AF_INET, SOCK_STREAM, 0);
	    if (server_socket < 0) {
	        show_socket_error_reason("create_server", server_socket);
	        return ESP_FAIL;
	    }

	    server_addr.sin_family = AF_INET;
	    server_addr.sin_port = htons(EXAMPLE_DEFAULT_PORT);
	    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
	        show_socket_error_reason("bind_server", server_socket);
	        close(server_socket);
	        return ESP_FAIL;
	    }
	    if (listen(server_socket, 5) < 0) {
	        show_socket_error_reason("listen_server", server_socket);
	        close(server_socket);
	        return ESP_FAIL;
	    }
	    connect_socket = accept(server_socket, (struct sockaddr *)&client_addr, &socklen);
	    if (connect_socket < 0) {
	        show_socket_error_reason("accept_server", connect_socket);
	        close(server_socket);
	        return ESP_FAIL;
	    }
	    /*connection established，now can send/recv*/
	    ESP_LOGI(TAG, "tcp connection established!");
	    return ESP_OK;
	}

	//use this esp32 as a tcp client. return ESP_OK:success ESP_FAIL:error
	static esp_err_t create_tcp_client()
	{
	    ESP_LOGI(TAG, "client socket....serverip:port=%s:%d\n",
	    		EXAMPLE_DEFAULT_SERVER_IP, EXAMPLE_DEFAULT_PORT);
	    connect_socket = socket(AF_INET, SOCK_STREAM, 0);
	    if (connect_socket < 0) {
	        show_socket_error_reason("create client", connect_socket);
	        return ESP_FAIL;
	    }
	    server_addr.sin_family = AF_INET;
	    server_addr.sin_port = htons(EXAMPLE_DEFAULT_PORT);
	    server_addr.sin_addr.s_addr = inet_addr(EXAMPLE_DEFAULT_SERVER_IP);
	    ESP_LOGI(TAG, "connecting to server...");
	    if (connect(connect_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
	        show_socket_error_reason("client connect", connect_socket);
	        return ESP_FAIL;
	    }
	    ESP_LOGI(TAG, "connect to server success!");
	    return ESP_OK;
	}

	//wifi_init_sta
	static void wifi_init_sta()
	{
	    tcp_event_group = xEventGroupCreate();

	    tcpip_adapter_init();
	    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

	    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	    wifi_config_t wifi_config;
	    strcpy( (char*)wifi_config.sta.ssid, CONFIG_TCP_PERF_WIFI_SSID);
	    strcpy( (char*) wifi_config.sta.password, CONFIG_TCP_PERF_WIFI_PWD);

	    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
	    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
	    ESP_ERROR_CHECK(esp_wifi_start() );

	    ESP_LOGI(TAG, "wifi_init_sta finished.");
	    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s \n",
	    		CONFIG_TCP_PERF_WIFI_SSID, CONFIG_TCP_PERF_WIFI_PWD);
	}

	//wifi_init_softap
	static void wifi_init_softap()
	{
	    tcp_event_group = xEventGroupCreate();

	    tcpip_adapter_init();
	    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	    wifi_config_t wifi_config;
	    strcpy( (char*)wifi_config.ap.ssid, CONFIG_TCP_PERF_WIFI_SSID);
	    wifi_config.ap.ssid_len = 0;
	    wifi_config.ap.max_connection = EXAMPLE_MAX_STA_CONN;
	    strcpy( (char*)wifi_config.ap.password, CONFIG_TCP_PERF_WIFI_PWD);
	    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
	    if (strlen(CONFIG_TCP_PERF_WIFI_PWD) == 0) {
	        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
	    }

	    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
	    ESP_ERROR_CHECK(esp_wifi_start());

	    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s \n",
	    		CONFIG_TCP_PERF_WIFI_SSID, CONFIG_TCP_PERF_WIFI_PWD);
	}




	static int get_socket_error_code(int socket)
	{
	    int result;
	    u32_t optlen = sizeof(int);
	    int err = getsockopt(socket, SOL_SOCKET, SO_ERROR, &result, &optlen);
	    if (err == -1) {
	        ESP_LOGE(TAG, "getsockopt failed:%s", strerror(err));
	        return -1;
	    }
	    return result;
	}

	static int show_socket_error_reason(const char *str, int socket)
	{
	    int err = get_socket_error_code(socket);

	    if (err != 0) {
	        ESP_LOGW(TAG, "%s socket error %d %s", str, err, strerror(err));
	    }

	    return err;
	}

	static int check_working_socket()
	{
	    int ret;
	#if ESP_TCP_MODE_SERVER
	    ESP_LOGD(TAG, "check server_socket");
	    ret = get_socket_error_code(server_socket);
	    if (ret != 0) {
	        ESP_LOGW(TAG, "server socket error %d %s", ret, strerror(ret));
	    }
	    if (ret == ECONNRESET) {
	        return ret;
	    }
	#endif
	    ESP_LOGD(TAG, "check connect_socket");
	    ret = get_socket_error_code(connect_socket);
	    if (ret != 0) {
	        ESP_LOGW(TAG, "connect socket error %d %s", ret, strerror(ret));
	    }
	    if (ret != 0) {
	        return ret;
	    }
	    return 0;
	}

	static void close_socket()
	{
	    close(connect_socket);
	    close(server_socket);
	}
};

#endif /* MAIN_WIFI_H_ */
