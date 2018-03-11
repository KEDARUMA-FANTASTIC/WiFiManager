#pragma once

/***
'WiFiManager [Baena]' extended WiFiManager and implemented it.
Therefore, we take over all the functions of the original WiFiManager.

The main functions that have been extended are
- Serial monitor
- Persistence of Captive Portal
- Custom Menu
- Appearance of a modern site

You will be able to do the following things
- Operation can be confirmed at any time with a serial monitor from the mobile terminal.
- Captive Portal permanence enables connection even without net environment.
- It is possible to implement a dedicated device setting menu with the custom menu function.

'WiFiManager [Baena]'はWiFiManagerを拡張して実装しました。
從って、オリジナルのwifimanagerの機能を全て引き継いでいます。

拡張した主な機能は
- シリアルモニター
- Captive Portalの永続化
- カスタムメニュー
- 今風なサイトの外観

次の事が可能になります
- モバイル端末からシリアルモニターでいつでも動作確認ができます。
- Captive Portalの永続化によりネット環境が無くとも接続が可能。
- カスタムメニュー機能で専用のデバイス設定メニューを実装する事が可能。

Built by KEDARUMA-FANTASTIC https://github.com/KEDARUMA-FANTASTIC
***/

/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

/* setting platformio.ini
// debug log enabled, serial0 is monitored
// デバッグログ有効、serial0をモニター
 build_flags = -g3 -D DEBUG=1 -D WIFIMAN_DEBUG=1 -D NO_GLOBAL_SERIAL -D USE_GLOBAL_HACK_SERIAL -D WIFIMAN_MONITOR_DEVICE=HKSerial
// debug log enabled, serial1 is monitored
// デバッグログ有効、serial1をモニター
build_flags = -g3 -D DEBUG=1 -D WIFIMAN_DEBUG=1 -D NO_GLOBAL_SERIAL1 -D USE_GLOBAL_HACK_SERIAL1 -D WIFIMAN_MONITOR_DEVICE=HKSerial1
// debug log disable, serial monitored disable
// デバッグログ無効、シリアルモニター無効
build_flags = -g3
*/

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#endif

#ifdef ESP32
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#endif

#include <memory>

#ifdef ESP8266
extern "C" {
  #include "user_interface.h"
}
#define ESP_getChipId()   (ESP.getChipId())
#endif

#ifdef ESP32
#include <esp_wifi.h>
#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())
#endif

#ifdef WIFIMAN_MONITOR_DEVICE
#include "HackSerial.h"
#endif

#if defined(WIFIMAN_MONITOR_DEVICE) && !defined(WIFIMAN_MONITOR_BUF_SIZE)
#define WIFIMAN_MONITOR_BUF_SIZE  1024
#endif

#ifndef WIFIMAN_DEBUG
#define WIFIMAN_DEBUG   1
#endif
#ifndef DEBUG
#undef WIFIMAN_DEBUG
#define WIFIMAN_DEBUG 0
#endif

#if (WIFIMAN_DEBUG)
#define DEBUG_WM(...)      WiFiManager::_DEBUG_WM(__VA_ARGS__)
#else
#define DEBUG_WM(...)      do {} while(0)
#endif

// Frequently used character strings are shared and ROM and saved
// よく使う文字列は共通化とROM化し節約する
namespace wifiman_const {
const char FS_CONTENT_LENGTH[] PROGMEM = "Content-Length";
const char FS_LOCATION[] PROGMEM = "Location";
const char FS_TEXT_HTML[] PROGMEM = "text/html";
const char FS_TEXT_PLAIN[] PROGMEM = "text/plain";
const char FS_HR[] PROGMEM = "{hr}";
const char FS_SBJ[] PROGMEM = "{sbj}";
const char FS_SB[] PROGMEM = "{sb}";
const char FS_T[] PROGMEM = "{t}";
const char FS_TT[] PROGMEM = "{tt}";
const char FS_I[] PROGMEM = "{i}";
const char FS_N[] PROGMEM = "{n}";
const char FS_P[] PROGMEM = "{p}";
const char FS_L[] PROGMEM = "{l}";
const char FS_V[] PROGMEM = "{v}";
const char FS_C[] PROGMEM = "{c}";
const char FS_AIP[] PROGMEM = "{aip}";
const char FS_IPT[] PROGMEM = "{ip}";
const char FS_IP[] PROGMEM = "ip";
const char FS_GW[] PROGMEM = "gw";
const char FS_SN[] PROGMEM = "sn";
const char FS_15[] PROGMEM = "15";
const char FS_DEVICE_INFORMATION[] PROGMEM = "Device Information";
#ifdef WIFIMAN_MONITOR_DEVICE
const char FS_SERIAL_MONITOR[] PROGMEM = "Serial Monitor";
#endif
}

#define WIFI_MANAGER_MAX_PARAMS 10

class WiFiManagerParameter {
  public:
    WiFiManagerParameter(const char *custom);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    const char *getCustomHTML();
  private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    friend class WiFiManager;
};

class WiFiManager
{
  public:
    WiFiManager();

    virtual void  bindHandler();

    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startConfigPortal();
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    // Establish both AP and network connection
    void          startConfigPortalMulti(char const *apName, char const *apPassword = NULL);
    // Always call this member function with setup()
    void          updateConfigPortalMulti();

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();

    void          resetSettings();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //useful for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void          setConfigPortalTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);


    void          setDebugOutput(boolean debug);
    //defaults to not showing anything under 8% signal quality if called
    void          setMinimumSignalQuality(int quality = 8);
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //sets config for a static IP
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(WiFiManager*) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );
    //adds a custom parameter
    void          addParameter(WiFiManagerParameter *p);
    //if this is set, it will exit after config, even if connection is unsuccessful.
    void          setBreakAfterConfig(boolean shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(boolean removeDuplicates);

    void setUseStaticIP(boolean isUse) { _is_use_static_ip = isUse; }
    boolean isUseStaticIP() { return _is_use_static_ip; }

  protected:
#ifdef ESP8266
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<ESP8266WebServer> server;
#endif

#ifdef ESP32
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<WebServer>        server;
#endif

    //const int     WM_DONE                 = 0;
    //const int     WM_WAIT                 = 10;

    //const String  HTTP_HEAD = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

    void          setupConfigPortal();
    void          startWPS();

    const char*   _apName                 = "no-net";
    const char*   _apPassword             = NULL;
    String        _ssid                   = "";
    String        _pass                   = "";
    unsigned long _configPortalTimeout    = 0;
    unsigned long _connectTimeout         = 0;
    unsigned long _configPortalStart      = 0;

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip;
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    boolean       _removeDuplicateAPs     = true;
    boolean       _shouldBreakAfterConfig = false;
    boolean       _tryWPS                 = false;

    const char*   _customHeadElement      = "";

    //String        getEEPROMString(int start, int len);
    //void          setEEPROMString(int start, int len, String string);

    int           status = WL_IDLE_STATUS;
    int           connectWifi(String ssid, String pass);
    uint8_t       waitForConnectResult();

    virtual const String get_html_head();
    virtual const String get_css_style();
    virtual const String get_script();
    virtual const String get_html_head_end();
    virtual const String get_html_root_page_header();
    virtual const String get_html_child_page_header();
    virtual const String get_html_menu_begin();
    virtual const String get_html_menu_end();
    virtual const String get_html_menu_item();
    virtual const String get_html_root_page_menu_items();
    virtual const String get_html_root_page_custom_menu_items();
    virtual const String get_html_wifi_form_start();
    virtual const String get_html_wifi_form_param();
    virtual const String get_html_wifi_form_end();
    virtual const String get_html_wifi_menu_scan();
    virtual const String get_html_end();
    virtual const String get_html_info();

    virtual void set_current_value(String& str);
    void handleCssStyle();
    void handleScript();

    virtual void  handleRoot();
    void          handleWifi(boolean scan);
    void          handleWifiSave();
    void          handleInfo();
    void          handleReset(bool is_also_setting = false);
    void          handleNotFound();
    void          handle204();
    boolean       captivePortal();
    boolean       configPortalHasTimeout();

    // DNS server
    const byte    DNS_PORT = 53;

    //helpers
    int           getRSSIasQuality(int RSSI);
    boolean       isIp(String str);
    String        toStringIp(IPAddress ip);

    boolean       connect;
    boolean       _debug = true;

    void (*_apcallback)(WiFiManager*) = NULL;
    void (*_savecallback)(void) = NULL;

    WiFiManagerParameter* _params[WIFI_MANAGER_MAX_PARAMS];

    template <typename Generic>
    static void _DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
      return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
      //DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
      return false;
    }

    boolean       _is_use_static_ip = false;
    unsigned long _last_micros = 0;

#ifdef WIFIMAN_MONITOR_DEVICE
private:
  String _mon_buf = "";
  size_t serial_on_write_hook(uint8_t ch);
public:
  virtual const String get_script_monitor();
  virtual const String get_html_serial_monitor();
  virtual void handleSerialMon();
  virtual void handleSerialMonIf();
  virtual void handleScriptMon();
#endif
};
