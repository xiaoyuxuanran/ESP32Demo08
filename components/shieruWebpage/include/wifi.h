#include "suwp_type.h"

class suwp_wifi
{
private:
    suwp_err_t wifi_init(void);
    suwp_err_t wifi_deinit(void);

    bool apModeEnabled = false;
    bool stationModeEnabled = false;

public:
    suwp_wifi();
    ~suwp_wifi();

private:
    char ap_ssid[20] = "shieru";
    char ap_password[20] = "3286811313";
    uint8_t ap_channel = 1; // 1~13
    uint8_t ap_max_connection = 8;

public:
    void wifi_set_ap_ssid(const char *newssid);
    void wifi_set_ap_password(const char *newpassword);
    void wifi_ap_start(void);
    void wifi_ap_stop(void);

private:
    char station_ssid[20] = "ss";
    char station_password[20] = "3286811313";

public:
    void wifi_set_station_ssid(const char *newssid);
    void wifi_set_station_password(const char *newpassword);
    void wifi_station_start(void);
    void wifi_station_stop(void);
};
