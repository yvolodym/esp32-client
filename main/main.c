#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "espnow.h"

#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_CHANNEL 1

static const char *TAG = "ESPNOW_CLIENT";

// MAC-Adresse des Servers (anpassen!)
static uint8_t server_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Callback für gesendete Daten
static void espnow_send_cb(uint8_t *mac_addr, uint8_t status)
{
    if (status == 0) {
        ESP_LOGI(TAG, "Daten erfolgreich gesendet");
    } else {
        ESP_LOGW(TAG, "Fehler beim Senden, Status: %d", status);
    }
}

// Callback für empfangene Daten
static void espnow_recv_cb(uint8_t *mac_addr, uint8_t *data, uint8_t len)
{
    ESP_LOGI(TAG, "Daten empfangen von: %02X:%02X:%02X:%02X:%02X:%02X",
             mac_addr[0], mac_addr[1], mac_addr[2],
             mac_addr[3], mac_addr[4], mac_addr[5]);
    
    ESP_LOGI(TAG, "Daten: %.*s", len, data);
}

// WiFi Initialisierung
static void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(ESPNOW_WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE));
    
    ESP_LOGI(TAG, "WiFi initialisiert, Kanal: %d", ESPNOW_WIFI_CHANNEL);
}

// ESP-NOW Initialisierung
static esp_err_t espnow_init(void)
{
    esp_err_t ret;
    
    ret = esp_now_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW Init fehlgeschlagen: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Callbacks registrieren
    esp_now_register_send_cb(espnow_send_cb);
    esp_now_register_recv_cb(espnow_recv_cb);
    
    // Peer (Server) hinzufügen
    esp_now_peer_info_t peer;
    memset(&peer, 0, sizeof(esp_now_peer_info_t));
    peer.channel = ESPNOW_WIFI_CHANNEL;
    peer.ifidx = ESP_IF_WIFI_STA;
    peer.encrypt = false;
    memcpy(peer.peer_addr, server_mac, 6);
    
    ret = esp_now_add_peer(&peer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Peer hinzufügen fehlgeschlagen: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "ESP-NOW initialisiert");
    return ESP_OK;
}

// Task zum periodischen Senden von Daten
static void espnow_send_task(void *pvParameter)
{
    char data[64];
    uint32_t counter = 0;
    
    while (1) {
        snprintf(data, sizeof(data), "Client Nachricht #%lu", counter++);
        
        esp_err_t ret = esp_now_send(server_mac, (uint8_t *)data, strlen(data));
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Senden fehlgeschlagen: %s", esp_err_to_name(ret));
        }
        
        vTaskDelay(pdMS_TO_TICKS(5000)); // Alle 5 Sekunden senden
    }
}

void app_main(void)
{
    // NVS initialisieren
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Eigene MAC-Adresse ausgeben
    uint8_t mac[6];
    esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
    ESP_LOGI(TAG, "Client MAC: %02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    
    // WiFi und ESP-NOW initialisieren
    wifi_init();
    
    if (espnow_init() == ESP_OK) {
        // Task zum Senden starten
        xTaskCreate(espnow_send_task, "espnow_send_task", 2048, NULL, 4, NULL);
    }
    
    ESP_LOGI(TAG, "ESP-NOW Client gestartet");
}