/*
 *  wifi_app.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
 */

/*
 * wifi global status
 * 0 -> disconnected
 * 1 -> connected
 * 2 -> got ip
 *
 * wifi connection status esp_err_t
 * 		- ESP_OK: succeed
 *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
 *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
 *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
 *    - ESP_ERR_WIFI_SSID: SSID of AP which station connects is invalid
 */

#include "wifi_app.h"

#define EXAMPLE_ESP_MAXIMUM_RETRY 100

static EventGroupHandle_t s_wifi_event_group;

#define WIFI_CONNECTED_BIT 0x00000001
#define WIFI_FAIL_BIT 0x00000002

static const char *TAG = "wifi station";
static int s_retry_num = 0;
wifi_ap_record_t wifi_info;
esp_err_t wifi_evt_stat;
wifi_config_t wifi_config;

const char szURL[] = "platform.iotera.io";

ip_addr_t ip_Addr;
ip4_addr_t ip;
ip4_addr_t gw;
ip4_addr_t msk;
bool bConnected = false;
bool bDNSFound = false;

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
        wifi_global_stat = 1;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        wifi_global_stat = 0;
        turn_on_rgb(RGB_MAGENTA);
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            printf("retry to connect to the AP\r\n");
            wifi_evt_stat = esp_wifi_connect();
            if (wifi_evt_stat == ESP_OK)
            {
                wifi_global_stat = 1;
            }
            s_retry_num++;
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            printf("connect to the AP fail\r\n");
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip: \r\n" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        wifi_global_stat = 2;
        bConnected = true;

        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(uint8_t *ssid, uint8_t *pass, wifi_auth_mode_t auth_mode)
{
    printf("starting wifi_init_sta\r\n");
    // wifi_config_t wifi_config;
    for (int i = 0; i < sizeof(wifi_config.sta.ssid); i++)
    {
        wifi_config.sta.ssid[i] = *ssid;
        ssid++;
    }
    for (int i = 0; i < sizeof(wifi_config.sta.password); i++)
    {
        wifi_config.sta.password[i] = *pass;
        pass++;
    }

    wifi_config.sta.threshold.authmode = auth_mode;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT)
    {
        printf("connected to ap SSID:%s password:%s\r\n", wifi_config.sta.ssid, wifi_config.sta.password);
        turn_on_rgb(RGB_BLUE);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        printf("Failed to connect to SSID:%s, password:%s\r\n", wifi_config.sta.ssid, wifi_config.sta.password);
    }
    else
    {
        printf("UNEXPECTED EVENT\r\n");
    }

    /* The event will not be processed after unregister */
    /*
    esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip);
    esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id);
    vEventGroupDelete(s_wifi_event_group);
    */
}

void wifi_init(uint8_t *ssid, uint8_t *pass, wifi_auth_mode_t auth_mode)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta(ssid, pass, auth_mode);
}

void dns_found_cb(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
    if (ipaddr != NULL)
    {
        ip_Addr = *ipaddr;
        bDNSFound = true;
    }
}

void wifi_resolve_dns()
{
    IP_ADDR4(&ip_Addr, 0, 0, 0, 0);
    printf("Getting IP for URL: %s\n", szURL);
    dns_gethostbyname(szURL, &ip_Addr, dns_found_cb, NULL);
    while (!bDNSFound)
        ;

    printf("DNS found: %i.%i.%i.%i\n",
           ip4_addr1(&ip_Addr.u_addr.ip4),
           ip4_addr2(&ip_Addr.u_addr.ip4),
           ip4_addr3(&ip_Addr.u_addr.ip4),
           ip4_addr4(&ip_Addr.u_addr.ip4));
}

esp_err_t wifi_reconnect(void)
{
    esp_err_t wifi_st = ESP_OK;
    wifi_st = esp_wifi_connect();
    return wifi_st;
}

uint8_t read_wifi_stat(void)
{
    esp_err_t stat;

    stat = esp_wifi_sta_get_ap_info(&wifi_info);
    if (stat == ESP_OK)
    {
        wifi_global_stat = 1;
        // printf("connected to ssid\r\n");
    }
    else if (stat == ESP_ERR_WIFI_NOT_CONNECT)
    {
        wifi_global_stat = 0;
        printf("not connected to ssid\r\n");
    }
    RSSI_WIFI = wifi_info.rssi;
    return wifi_global_stat;
}
