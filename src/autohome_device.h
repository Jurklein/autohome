#ifndef AUTOHOME_DEVICE_H
#define AUTOHOME_DEVICE_H

#include <Arduino.h>
#include <Hash.h>
#include <ArduinoJson.h>
#include "auto_format.h" // "C variable manipulation" wrapper
//Begin: ESP8266 basic libraries
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
//End: ESP8266 basic libraries
//Begin: Websocket libraries
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
//End: Websocket libraries
// Begin: OTA libraries
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
// End: OTA libraries
//Begin: HTTP Client
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
//End: HTTP Client
//Begin: ESP_WiFiManager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP_WiFiManager.h>         //https://github.com/tzapu/WiFiManager
//End: ESP_WiFiManager
//Begin: File System
#define USE_LITTLEFS 1
#define USE_SPIFFS 2
#define FS_CHOICE USE_SPIFFS
#if FS_CHOICE == USE_SPIFFS
#include "FS.h"
#define FS SPIFFS
#elif FS_CHOICE == USE_LITTLEFS
#include "LittleFS.h"
#define FS LittleFS
#endif
//End: File System
//Begin: Special-PIN Labels
#define RX_PIN 3 //RX = D9 = GPIO3
#define TX_PIN 1 //TX = D10 = GPIO1
//----------------------------------// https://arduino.stackexchange.com/questions/29938/how-to-i-make-the-tx-and-rx-pins-on-an-esp-8266-01-into-gpio-pins
#define UART_PIN_AS_GPIO FUNCTION_3
#define UART_PIN_AS_UART FUNCTION_0
//----------------------------------//
#define BUILTIN_LED_PIN 16 // led_builtin = D0 = GPIO16
//End: Special-PIN Labels

//----------------------- Config Options ---------------------------//
#ifndef CONFIG_OPTIONS
#define CONFIG_OPTIONS
#define CONFIG_FILE "/config.txt"
//------------- Config Default Parameters ---------------//
#ifndef DEFAULT_SSID
#define DEFAULT_SSID "home_net"
#endif
#ifndef DEFAULT_PASSWORD
#define DEFAULT_PASSWORD "1234"
#endif
#ifndef DEFAULT_DES_HOST
#define DEFAULT_DES_HOST "192.168.0.102"
#endif
#ifndef DEFAULT_HML_HOST
#define DEFAULT_HML_HOST "192.168.0.101"
#endif
#ifndef DEFAULT_PRD_HOST
#define DEFAULT_PRD_HOST "192.168.0.100"
#endif
#ifndef DEFAULT_PORT
#define DEFAULT_PORT "3030"
#endif
#ifndef DEFAULT_DEVICE_NAME
#define DEFAULT_DEVICE_NAME "Generic_Device"
#endif
#ifndef DEFAULT_DEVICE_PWD
#define DEFAULT_DEVICE_PWD "1234"
#endif
#ifndef DEFAULT_ON_CLIENT_ERROR_ACTION
#define DEFAULT_ON_CLIENT_ERROR_ACTION "retry" //may be "retry" or "server"
#endif
#ifndef DEFAULT_RETRY_INTERVAL
#define DEFAULT_RETRY_INTERVAL "5000" //time interval in milliseconds before trying to connect again after connection failure
#endif
#ifndef DEFAULT_CONFIG_FILE
#define DEFAULT_CONFIG_FILE "{\"ssid\":\"" DEFAULT_SSID "\",\n\"pwd\":\"" DEFAULT_PASSWORD "\",\n\"des_host\":\"" DEFAULT_DES_HOST "\",\n\"hml_host\":\"" DEFAULT_HML_HOST "\",\n\"prd_host\":\"" DEFAULT_PRD_HOST "\",\n\"port\":\"" DEFAULT_PORT "\",\n\"device\":\"" DEFAULT_DEVICE_NAME "\",\n\"device_pwd\":\"" DEFAULT_DEVICE_PWD "\",\n\"on_client_error\":\"" DEFAULT_ON_CLIENT_ERROR_ACTION "\",\n\"retry_interval\":\"" DEFAULT_RETRY_INTERVAL "\"\n}"
#endif
#ifndef DEFAULT_CONFIG_FILE_SIZE
#define DEFAULT_CONFIG_FILE_SIZE 264
//Should match the number of characters of the following text (with LF (end of line) but not CR (carriage return)):
#endif
/*
{
  "ssid": "home_net",
  "pwd": "1234",
  "des_host": "192.168.0.102",
  "hml_host": "192.168.0.101",
  "prd_host": "192.168.0.100",
  "port": "3030",
  "device": "Generic_Device",
  "device_pwd": "1234",
  "on_client_error": "retry",
  "retry_interval": "5000"
}
*/
//(remember to update the above text/DEFAULT_CONFIG_FILE_SIZE if some
//of the DEFAULT_ macros are changed)
#ifndef JSON_CONFIG_DOC_SIZE
#define JSON_CONFIG_DOC_SIZE DEFAULT_CONFIG_FILE_SIZE*2+100
#endif

#endif
//-------------------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
#ifndef AP_DEFINITIONS
#define AP_DEFINITIONS
typedef enum ap_param_error_t {
    NO_AP_PARAM_ERROR    = 0,
    INVALID_EMPTY_SSID   = 1,
    INVALID_LONG_SSID    = 2,
    INVALID_LONG_PSWD    = 3,
    INVALID_LONG_TIMEOUT = 4,
    INVALID_APIP         = 5,
    INVALID_GWIP         = 6,
    INVALID_SNIP         = 7
} ap_param_error_t;

typedef enum ap_return_status_t {
    CONNECTION_FAIL      = -1,
    INVALID_PARAMS       = 0,
    WIFI_CONNECTED       = 1
} ap_return_status_t;

#endif

//------------------------------------------------------------------//

#define ESP_getChipId()   (ESP.getChipId())


//typedef std::function<void(socketIOmessageType_t type, uint8_t * payload, size_t length)> SocketIOclientEvent;
typedef std::function<void(STRING_TYPE)> TAppEventCallback;

class AutohomeDeviceClass {
    private:
    //------------------------------------------------------------------//
        ESP8266WiFiMulti _wifiMulti;
        SocketIOclient _socketIO;
        //static DynamicJsonDocument _jsonConfig(JSON_CONFIG_SIZE);
    //------------------------------------------------------------------//
    //----------------------- Config Options ---------------------------//
        static StaticJsonDocument<JSON_CONFIG_DOC_SIZE> _jsonConfig;
        JSON_VALUE_STRING_TYPE _ssid;
        JSON_VALUE_STRING_TYPE _password;
        JSON_VALUE_STRING_TYPE _des_host;
        JSON_VALUE_STRING_TYPE _hml_host;
        JSON_VALUE_STRING_TYPE _prd_host;
        //JSON_VALUE_STRING_TYPE host[2];
        JSON_VALUE_STRING_TYPE _port;
        JSON_VALUE_STRING_TYPE _device;
        JSON_VALUE_STRING_TYPE _device_pwd;
        JSON_VALUE_STRING_TYPE _on_client_error;
        JSON_VALUE_STRING_TYPE _retry_interval;
    //------------------------------------------------------------------//
    //------------------------------------------------------------------//
        STRING_TYPE getSSID();
        STRING_TYPE getPSWD();
        STRING_TYPE getSTAIP();
    //------------------------------------------------------------------//
    //------------------------------------------------------------------//
        //Setup processes: PIN, Serial, Config, WiFi, OTA, Server, IO
        boolean _pin_setup_ready = false;
        boolean _config_setup_ready = false;
        boolean _wifi_setup_ready = false; //independent - if no config credentials are given, will try to connect with memory-stored credentials
        boolean _ota_setup_ready = false;  //depends on config setup; probably independent of wifi setup
        boolean _server_setup_ready = false; //depends on wifi and config setups
        boolean _io_setup_ready = false; //depends on server and config setups
    //------------------------------------------------------------------//
    //------------------------------------------------------------------//
        boolean _server_debug_on = false;
        boolean _serial_debug_on = false;
        boolean _serial_enabled = false;
        boolean _rx_pin_is_gpio = false; // must start false - mimic hardware behaviour
        boolean _tx_pin_is_gpio = false; // must start false - mimic hardware behaviour
        boolean _begin_command_received = false;
        boolean _listen_to_commands = false;
    //------------------------------------------------------------------//
    //----------------------- Server Options ---------------------------//
        boolean _server_on_client_error = false;
        boolean _connect_des_if_reachable = true;
        boolean _connect_hml_if_reachable = false;
        STRING_TYPE _current_host;
        STRING_TYPE _current_port;
    //------------------------------------------------------------------//
    //------------------------------------------------------------------//
        boolean _io_connected = false;
        String _jwt;
    //------------------------------------------------------------------//
    protected:
    //------------------------------------------------------------------//
        boolean _rx_pin_interchangeable = true; // if pin is to be used as GPIO, must start true for the first setSerialStatus(false) call
        boolean _tx_pin_interchangeable = true; // if pin is to be used as GPIO, must start true for the first setSerialStatus(false) call
    //------------------------------------------------------------------//
    //------------------------------------------------------------------//
        boolean _overwrite_config_on_error = true;
    //------------------------------------------------------------------//
    //------------------------------------------------------------------//
        void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length);
    //------------------------------------------------------------------//
    //------------------------------------------------------------------//
        boolean setupPIN();
        boolean setupSerial();
        boolean setupConfig();
        boolean setupWiFi();
        boolean enableConfigPortal();
        void setupOTA();
        boolean setupServer();
        boolean setupIO(SocketIOclient::SocketIOclientEvent socketIOEvent);
        typedef std::vector<TAppEventCallback> VAppEventCallbacks;
        VAppEventCallbacks eventCallbacks;
        void invokeEventCallbacks(STRING_TYPE event);
    //------------------------------------------------------------------//
    //------------------------------------------------------------------//
        boolean portalParamErrorDisclaimer(ap_param_error_t paramError);
    //------------------------------------------------------------------//
    public:
    //------------------------------------------------------------------//
        void debug_print(STRING_TYPE message);
        void postIoT_log(STRING_TYPE message);
        void postMessage(STRING_TYPE message);
        void socketIOAuthenticate();
        boolean checkHost(JSON_VALUE_STRING_TYPE address, JSON_VALUE_STRING_TYPE port);
        ap_return_status_t configPortal(STRING_TYPE apSSID, STRING_TYPE apPSWD, STRING_TYPE timeout,
                                        STRING_TYPE apIP, STRING_TYPE gwIP, STRING_TYPE snIP);
        ap_return_status_t configPortal(STRING_TYPE apSSID, STRING_TYPE apPSWD, unsigned long timeout,
                                        IPAddress apIP, IPAddress gwIP, IPAddress snIP);
        boolean getSerialStatus();
        boolean setSerialStatus(boolean status, int baudRate = 115200);

        void addEventCallback(TAppEventCallback cb);
    //------------------------------------------------------------------//
};

extern AutohomeDeviceClass Device;

void debug_print(STRING_TYPE message);

#endif

