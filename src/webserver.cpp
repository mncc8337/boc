#include <screen.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

static WebServer server(80);

bool is_webserver_running = false;

File uploadFile;

extern void open_notification(std::string message);

void setup_webserver() {
    open_notification("Trying to connect\nto WiFi\n" WIFI_SSID);

    IPAddress staticIP(192, 168, 1, 250);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress primaryDNS(8, 8, 8, 8);
    IPAddress secondaryDNS(8, 8, 4, 4);

    WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned tries = 0;
    while(WiFi.status() != WL_CONNECTED && tries < 10) {
        delay(500);
        tries++;
    }
    if(WiFi.status() != WL_CONNECTED) {
        open_notification("Failed to\nconnect to\nWiFi");
        ESP_LOGE("Time", "Failed to connect to WiFi \"" WIFI_SSID "\"");
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        return;
    }

    server.serveStatic("/", LittleFS, "/index.html");
    server.serveStatic("/script.js", LittleFS, "/script.js");
    server.serveStatic("/data.log", LittleFS, "/data.log");

    server.begin();

    ESP_LOGI("WEB", "Server started at %s", WiFi.localIP().toString().c_str());

    is_webserver_running = true;
}

void end_webserver() {
    server.close();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    ESP_LOGI("WEB", "Server closed");
    is_webserver_running = false;
}

void loop_webserver() {
    server.handleClient();
}
