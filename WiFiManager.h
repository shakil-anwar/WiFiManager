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

#ifndef WiFiManager_h
#define WiFiManager_h
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>

extern "C" {
  #include "user_interface.h"
}

const char HTTP_HEADER[] PROGMEM          = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .newbutton{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2rem;font-size:1.125rem;width:25%; float:right;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
const char STYLE_FORM_POP_UP[] PROGMEM    = "<style>.form-popup {display: none;position: center;bottom: 0;right: 15px;border: 3px solid #f1f1f1;z-index: 9;} .form-container {max-width: 200px;padding: 10px;background-color: white;} .form-container input[type=text], .form-container input[type=password] {width: 100%;padding: 15px;margin: 5px 0 22px 0;border: none;background: #f1f1f1;} .form-container input[type=text]:focus, .form-container input[type=password]:focus {background-color: #ddd;outline: none;} .form-container .btn {background-color: #4CAF50;color: white;padding: 16px 20px;border: none;cursor: pointer;width: 100%;margin-bottom:10px;opacity: 0.8;} .form-container .cancel {background-color: red;} .form-container .btn:hover, .open-button:hover {opacity: 1;}</style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();document.getElementById('myForm').style.display = 'block';} function openForm() {document.getElementById(\"myForm\").style.display = \"block\";} function closeForm() {document.getElementById(\"myForm\").style.display = \"none\";}</script>";
const char HTTP_HEADER_END[] PROGMEM      = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char PRANISHEBA_LOGO[] PROGMEM      = "<div style=\"text-align: center;\"><img src='data:image/png;base64,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' alt='Adorsho Pranisheba'></div>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/wifi\" method=\"get\"><button>Configure WiFi</button></form><br/><form action=\"/0wifi\" method=\"get\"><button>Configure WiFi (No Scan)</button></form><br/><form action=\"/mqttconf\" method=\"get\"><button>Configure MQTT</button></form><br/><form action=\"/debugconf\" method=\"get\"><button>Configure Debug</button></form><br/><form action=\"/i\" method=\"get\"><button>Info</button></form><br/><form action=\"/r\" method=\"post\"><button>Reset</button></form>";
const char HTTP_LOGIN_PAGE[] PROGMEM      = "<form action=\"/wifilogin\" method=\"get\"><button class= \"newbutton\">Login</button></form><br/></form>";
const char HTTP_ITEM[] PROGMEM            = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
const char HTTP_FORM_START[] PROGMEM      = "<div class='form-popup' id='myForm'> <form method='get' action='wifisave' class='form-container'><input id='s' name='s' length=32 placeholder='SSID' readonly><br/><input id='p' name='p' length=32 type='password' placeholder='password'><br/>";
const char HTTP_LOGIN_FORM_START[] PROGMEM= "<form method='post' action='wifiloginsubmit'><input id='u' name='u' length=32 placeholder='Username'><br/><input id='h' name='h' length=64 type='password' placeholder='Password'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM      = "<br/><input id='{i}' name='{n}' maxlength={l} placeholder='{p}' value='{v}' {c} readonly>";
const char HTTP_FORM_END[] PROGMEM        = "<br/><button type='submit'>save</button></form> </div>";
const char HTTP_SCAN_LINK[] PROGMEM       = "<br/><div class=\"c\"><a href=\"/wifi\">Scan</a></div>";
const char HTTP_SAVED[] PROGMEM           = "<div>Credentials Saved<br />Trying to connect Your device to network.<br />If it fails reconnect to this wifi and try again</div>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";

#ifndef WIFI_MANAGER_MAX_PARAMS
#define WIFI_MANAGER_MAX_PARAMS 10
#endif

class WiFiManagerParameter {
  public:
    /** 
        Create custom parameters that can be added to the WiFiManager setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
    WiFiManagerParameter(const char *custom);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
    ~WiFiManagerParameter();

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
    ~WiFiManager();

    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startConfigPortal();
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

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
    //adds a custom parameter, returns false on failure
    bool          addParameter(WiFiManagerParameter *p);
    //if this is set, it will exit after config, even if connection is unsuccessful.
    void          setBreakAfterConfig(boolean shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(boolean removeDuplicates);

  private:
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<ESP8266WebServer> server;

    //const int     WM_DONE                 = 0;
    //const int     WM_WAIT                 = 10;

    //const String  HTTP_HEADER = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

    void          setupConfigPortal();
    void          startWPS();

    const char*   _apName                 = "no-net";
    const char*   _apPassword             = NULL;
    String        _ssid                   = "";
    String        _pass                   = "";
    String        _httpUserName           = "admin";
    String        _httpPassWord           = "admin";

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

    void          handleRoot();
    void          handleWifilogin(void);
    void          handleWifi(boolean scan);

    void          handleWifiloginSubmit(void);
    void          handleWifiSave();
    
    void          handleInfo();
    void          handleReset();
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

    int                    _max_params;
    WiFiManagerParameter** _params;

    template <typename Generic>
    void          DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
      return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
      DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
      return false;
    }
};

#endif
