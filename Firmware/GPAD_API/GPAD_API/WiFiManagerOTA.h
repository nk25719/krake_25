#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <WiFiManager.h>
#include <LittleFS.h>

extern const char *DEFAULT_SSID;

namespace WifiOTA
{
    class Manager
    {
    public:
        Manager(WiFiClass &wifi, Print &print);
        ~Manager();

        void initialize();
        void connect(const char *const accessPointSsid);
        void setConnectedCallback(std::function<void()> callBack);
        void setApStartedCallback(std::function<void()> callback);
        wifi_mode_t getMode();
        IPAddress getAddress();

    private:
        WiFiClass &wifi;
        Print &print;
        WiFiManager wifiManager;
        std::function<void()> connectedCallback;
        std::function<void()> apStartedCallback;

        void ssidSaved();
        void ipSet();
        void apStarted();
        void startPortal(const char *const accessPointSsid);
    };

    void initLittleFS();
};

#endif // WIFI_MANAGER_H
