/*#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
*/


#include <Arduino.h>
#include <ESP8266WiFi.h>
/*
// Begin: OTA libraries
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
// End: OTA libraries
#include <ESP8266WiFiMulti.h>

#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <Hash.h>

//Begin: HTTP Client
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
//End: HTTP Client

//Begin: ESP_WiFiManager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP_WiFiManager.h>         //https://github.com/tzapu/WiFiManager
//End: ESP_WiFiManager
*/


void setup() {
  //---------------PIN Setup----------------//
  //PIN setup parameters: none (for finalization, depends on serial_debug_on flag)
  /*pinMode(PORTAL_ON_PIN, INPUT);
  pinMode(PORTAL_PIN, OUTPUT);
  pinMode(HORTA_PIN, OUTPUT);
  digitalWrite(PORTAL_PIN, !RELAY_ON);
  digitalWrite(HORTA_PIN, !RELAY_ON);
  pinMode(PORTAL_ABERTO_PIN, INPUT);
  pinMode(PORTAL_FECHADO_PIN, INPUT);*/
  //-----------Safe Serial setup------------//  <-- REMEMBER to disconnect RX / TX pins if using Serial!!
  //Serial setup parameters: none
  /*if(serial_debug_on) {
    USE_SERIAL.begin(115200);
    serial_enabled = true;
    USE_SERIAL.setDebugOutput(true);
    for(int t = 4; t > 0; t--) {
      debug_print(str_num("[SETUP] BOOT WAIT ", t));
      USE_SERIAL.flush();
      delay(1000);
    }
  } else {
    pinMode(PORTAL_FA_PIN, INPUT);
    debug_print("[SETUP] Serial disabled.");
  }*/
  //serial_setup_ready = true;
  //----------------------------------------//
  //pin_setup_ready = true; //Only set this after RX pin usage has been decided
  //----------------------------------------//

  //------Read the Config file params-------//
  //Config setup parameters: none
  /*boolean mount_FS_fail = false;
  boolean open_config_fail = false;
  boolean deserializeJson_fail = false;
  boolean default_config_created = false;
  boolean mount_result = FS.begin(); // always use this to "mount" the filesystem
  if(!mount_result) {
    mount_FS_fail = true;
  }
  JSON_VALUE_STRING_TYPE ssid;
  JSON_VALUE_STRING_TYPE password;
  JSON_VALUE_STRING_TYPE des_host;
  JSON_VALUE_STRING_TYPE hml_host;
  JSON_VALUE_STRING_TYPE prd_host;
  //JSON_VALUE_STRING_TYPE host[2];
  JSON_VALUE_STRING_TYPE port;
  JSON_VALUE_STRING_TYPE device;
  JSON_VALUE_STRING_TYPE device_pwd;
  JSON_VALUE_STRING_TYPE on_client_error;
  JSON_VALUE_STRING_TYPE retry_interval;
  DynamicJsonDocument doc(DEFAULT_CONFIG_FILE_SIZE*2+100);
  if(!mount_FS_fail) {
    File conf = FS.open("/config.txt", "r");
    if (!conf) {
      if(FS.exists("/config.txt"))
        debug_print("[CONF] Config file exists but couldn't be opened; overwriting with default config file.");
      else debug_print("[CONF] Config file not found; creating default config file.");
      conf.close();
      conf = FS.open("/config.txt", "w");
      if (!conf) {
        debug_print("[SETUP] file creation failed");
        open_config_fail = true;
      } else {
        const uint8_t* default_config_file = (const uint8_t*)DEFAULT_CONFIG_FILE;
        size_t file_size = DEFAULT_CONFIG_FILE_SIZE;
        conf.write(default_config_file,file_size);
        conf.close();
        default_config_created = true;
      }
    } else conf.close();
    if(!open_config_fail) {
      File config_file = FS.open("/config.txt", "r");
      if(config_file) {
        debug_print(str_num("[CONF] config file size: ", (int)config_file.size()));
        String config_data;
        while(config_file.available()) {
          config_data += config_file.readStringUntil('\n');
        }
        DeserializationError error = deserializeJson(doc, config_data);
        if (error) {
          debug_print(str_str("[ERROR][CONF] deserializeJson() failed. Error: ", error.c_str()));
          deserializeJson_fail = true;
        } else {
          ssid = doc["ssid"];//get_json_value(doc, ssid, "ssid");//doc["ssid"];
          password = doc["pwd"];//get_json_value(doc, password, "pwd");
          des_host = doc["des_host"];//get_json_value(doc,host_0,"host",0); // "192.168.0.100"
          hml_host = doc["hml_host"];//get_json_value(doc,host_0,"host",0); // "192.168.0.100"
          prd_host = doc["prd_host"];//get_json_value(doc,host_1,"host",1); // "192.168.0.101"
          port = doc["port"];//get_json_value(doc,port, "port");
          device = doc["device"];//get_json_value(doc,device, "device");
          device_pwd = doc["device_pwd"];//get_json_value(doc,device_pwd,"device_pwd");
          on_client_error = doc["on_client_error"];
          retry_interval = doc["retry_interval"];
        }
      } else {
        open_config_fail = true;
        debug_print("[ERROR][CONF] Config file couldn't be opened on the second moment.");
      }
      config_file.close();
    }
  } else debug_print("[ERROR][SETUP] Filesystem couldn't be mounted.");
  if(mount_FS_fail || open_config_fail || deserializeJson_fail) {
    ssid     = (JSON_VALUE_STRING_TYPE)DEFAULT_SSID;
    password = (JSON_VALUE_STRING_TYPE)DEFAULT_PASSWORD;
    des_host = (JSON_VALUE_STRING_TYPE)DEFAULT_DES_HOST;
    hml_host = (JSON_VALUE_STRING_TYPE)DEFAULT_HML_HOST;
    prd_host = (JSON_VALUE_STRING_TYPE)DEFAULT_PRD_HOST;
    port     = (JSON_VALUE_STRING_TYPE)DEFAULT_PORT;
    device   = (JSON_VALUE_STRING_TYPE)DEFAULT_DEVICE_NAME;
    device_pwd = (JSON_VALUE_STRING_TYPE)DEFAULT_DEVICE_PWD;
    on_client_error = (JSON_VALUE_STRING_TYPE)DEFAULT_ON_CLIENT_ERROR_ACTION;
    retry_interval  = (JSON_VALUE_STRING_TYPE)DEFAULT_RETRY_INTERVAL;
  }
  if(!str_empty(ssid)) debug_print(str_str("[CONF] SSID: ",ssid));
  else {
    debug_print(str_str("[CONF] SSID not defined; setting to \"",str_str(DEFAULT_SSID,"\".")));
    ssid = DEFAULT_SSID;//"home_net";
  }
  if(!str_empty(password)) debug_print(str_str("[CONF] Password: ",password));
  else {
    debug_print(str_str("[CONF] Password not defined; setting to \"",str_str(DEFAULT_PASSWORD,"\".")));
    password = DEFAULT_PASSWORD;//"1234";
  }
  if(!str_empty(des_host)) debug_print(str_str("[CONF] DES Host: ", des_host)); else debug_print("[CONF] DES Host not defined.");
  if(!str_empty(hml_host)) debug_print(str_str("[CONF] HML Host: ", hml_host)); else debug_print("[CONF] HML Host not defined.");
  if(!str_empty(prd_host)) debug_print(str_str("[CONF] PRD Host: ",prd_host));
  else {
    if(str_empty(des_host) && str_empty(hml_host)) {
      debug_print(str_str("[CONF] PRD Host not defined; setting to \"",str_str(DEFAULT_PRD_HOST,"\".")));
      prd_host = DEFAULT_PRD_HOST;//"192.168.0.100";
    }
    else debug_print("[CONF] PRD Host not defined.");
  }
  if(!str_empty(port)) debug_print(str_str("[CONF] Port: ",port));
  else {
    debug_print(str_str("[CONF] Port not defined; setting to \"",str_str(DEFAULT_PORT,"\".")));
    port = DEFAULT_PORT;//"3030";
  }
  if(!str_empty(device)) debug_print(str_str("[CONF] Device Name: ",device));
  else {
    debug_print(str_str("[CONF] Device Name empty; setting to \"",str_str(DEFAULT_DEVICE_NAME,"\".")));
    device = DEFAULT_DEVICE_NAME;//"Generic_Device";
  }
  if(!str_empty(device_pwd)) debug_print(str_str("[CONF] Device Password: ",device_pwd)); else debug_print("[CONF] Device Password empty.");
  if(!str_empty(on_client_error)) debug_print(str_str("[CONF] Action on server connection error: ",on_client_error));
  else {
    debug_print(str_str("[CONF] Action on server connection error not defined; setting to \"",str_str(DEFAULT_ON_CLIENT_ERROR_ACTION,"\".")));
    on_client_error = DEFAULT_ON_CLIENT_ERROR_ACTION;//"retry";
  }
  if(!str_empty(retry_interval)) debug_print(str_str("[CONF] Retry interval if action is \"retry\": ",retry_interval));
  else {
    debug_print(str_str("[CONF] Retry interval if action is \"retry\" not defined; setting to \"",str_str(DEFAULT_RETRY_INTERVAL,"\" milliseconds.")));
    retry_interval = DEFAULT_RETRY_INTERVAL;//"5000";
  }
  config_setup_ready = true;
  */
  //----------------------------------------//

  //------Setting Up Wi-Fi connection-------//
  //WiFi setup parameters: ssid, password
    //Config Portal setup parameters: apIP_str, apGatewayIP_str, apSubNetIP_str, ap_ssid, ap_password
  /*
  if(WiFi.getMode() & WIFI_AP) {
      WiFi.softAPdisconnect(true);
  }
  if(!WiFiMulti.existsAP(ssid,password)) {
    if(WiFiMulti.addAP(ssid, password)) {
      debug_print("[SETUP] WiFi credentials stored.");
    } else {
      debug_print("[ERROR][SETUP] WiFi credentials could not be stored.");
    }
  }
  //WiFi.disconnect();
  unsigned long wifi_timeout_counter = millis();
  unsigned long wifi_timeout = 10000; // If connection couldn't be established after 10 seconds, log the misconnection.
  boolean wifi_setup_fail = false;    // This flag isn't being used right now; but it may be useful for:
                                      // ESP8266 continues to manage peripherals even if connection couldn't be established.
  int attempt_count = 1;
  int max_attempts_before_config_portal = 3;
  int config_portal_attempt_count = 1;
  int max_config_portal_attempts = 2;
  String router_SSID;
  String router_Pass;
  debug_print(str_num(str_str(str_num("[SETUP] Trying to connect; attempt ", attempt_count), " of "), max_attempts_before_config_portal));
  while(WiFiMulti.run() != WL_CONNECTED) {
      delay(100);
      if(millis() - wifi_timeout_counter > wifi_timeout) {
        debug_print(str_str(str_num("[ERROR][SETUP] WiFi automatic connection failed after trying for ", (float)wifi_timeout * (float)0.001), " seconds; parameters:"));
        ssid = (STRING_TYPE)WiFi.SSID().c_str();
        password = (STRING_TYPE)WiFi.psk().c_str();
        debug_print(str_str("ssid: ", ssid));
        debug_print(str_str("password: ", password));
        attempt_count++;
        if(attempt_count > max_attempts_before_config_portal) {
          attempt_count = 1;
          debug_print("[SETUP] Maximum attempts for automatic connection reached; setting up Access Point and Config Portal.");
          //---------------- WiFi Manager Instance ----------------//
          ESP_WiFiManager ESP_wifiManager(device);
          ESP_wifiManager.setDebugOutput(true);
          //-------------------------------------------------------//
          //-------- "ESP as STA" Parameters Configuration --------//
          // asterisko //
          int staErro = 0;
          String staIP_str = "192.168.2.114";
          IPAddress staIP;
          if(!staIP.fromString(staIP_str)) staErro++;
          String staGatewayIP_str = "192.168.2.1";
          IPAddress staGatewayIP;
          if(!staGatewayIP.fromString(staGatewayIP_str)) staErro++;
          String staNetMask_str = "255.255.255.0";
          IPAddress staNetMask;
          if(!staNetMask.fromString(staNetMask_str)) staErro++;
          String staDNS1IP_str = staGatewayIP_str;
          IPAddress staDNS1IP;
          if(!staDNS1IP.fromString(staDNS1IP_str)) staErro++;
          String staDNS2IP_str = "8.8.8.8";
          IPAddress staDNS2IP;
          if(!staDNS2IP.fromString(staDNS2IP_str)) staErro++;
          if(staErro == 0) {
            debug_print("[SETUP] Setting up Station");
            ESP_wifiManager.setMinimumSignalQuality(-1);
            ESP_wifiManager.setSTAStaticIPConfig(staIP, staGatewayIP, staNetMask, staDNS1IP, staDNS2IP);
          } else {
            debug_print("[ERROR][SETUP] Error configuring Station parametgers.");
          }
          // asterisko //
          //-------------------------------------------------------//
          //-------- "ESP as AP" Parameters Configuration ---------//
          int apErro = 0;
          String apIP_str = "192.168.123.1";
          IPAddress apIP;
          if(!apIP.fromString(apIP_str)) apErro++;
          String apGatewayIP_str = "192.168.123.1";
          IPAddress apGatewayIP;
          if(!apGatewayIP.fromString(apGatewayIP_str)) apErro++;
          String apSubnetIP_str = "255.255.255.0";
          IPAddress apSubnetIP;
          if(!apSubnetIP.fromString(apSubnetIP_str)) apErro++;
          if(apErro == 0) {
            debug_print("[SETUP] Setting up Access Point");
            //ESP_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));
            ESP_wifiManager.setAPStaticIPConfig(apIP, apGatewayIP, apSubnetIP);
          } else {
            debug_print("[ERROR][SETUP] Error setting up Access Point; exiting WiFi setup.");
            wifi_setup_fail = true;
            break;
          }
          //-------------------------------------------------------//
          //------------- "ESP as AP" Config Portal ---------------//
          String ap_ssid = "GenDev_" + String(ESP_getChipId(), HEX);
          const char* ap_password = "1234";//"MyESP_" + chipID;
          //ESP_wifiManager.setConfigPortalTimeout(60); //If no access point name has been previously entered disable timeout.
          debug_print(str_num(str_str(str_num("[SETUP] Opening Config Portal to retrieve new WiFi credentials; attempt ", config_portal_attempt_count), " of "), max_config_portal_attempts));
          if(ESP_wifiManager.startConfigPortal((const char *) ap_ssid.c_str(), ap_password)) {
            continue;
          } else {
            debug_print("[ERROR][SETUP] Config Portal couldn't establish a WiFi connection; parameters:");
            router_SSID = ESP_wifiManager.WiFi_SSID();
            router_Pass = ESP_wifiManager.WiFi_Pass();
            debug_print(str_str("ssid: ", router_SSID));
            debug_print(str_str("password: ", router_Pass));
            if(config_portal_attempt_count >= max_config_portal_attempts) {
              debug_print("[SETUP] Maximum number of WiFi connection attempts via Config Portal reached; exiting WiFi setup.");
              wifi_setup_fail = true; //flag not being used
              break;
            } else {
              debug_print("[SETUP] Trying to connect again; if still no success, Config Portal will be reopened.");
              config_portal_attempt_count++;
            }
          }
          //-------------------------------------------------------//
        }
        debug_print(str_num(str_str(str_num("[SETUP] Trying to connect; attempt ", attempt_count), " of "), max_attempts_before_config_portal));
        wifi_timeout_counter = millis();
      }
  }
  //wifi_setup_fail = false; // flag not being used
  if(!wifi_setup_fail) {
    debug_print(str_str("[SETUP] WiFi Connected; IP: ", WiFi.localIP().toString()));
    wifi_setup_ready = true;
  }
  */
  //----------------------------------------//

  //--------------Arduino OTA---------------//  <-- OTA code must come after the Wi-Fi connection setup
  //OTA setup parameters: device, device_pwd
  /*
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(device);
  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  //ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  if(!str_empty(device_pwd)) ArduinoOTA.setPassword(device_pwd);
  ArduinoOTA.onStart([]() {
    if(serial_enabled) { serial_enabled = false; Serial.end(); } // If Serial began, this safely ends it
    server_debug_on = false; //If server_debug_on, ESP won't respond after OTA process is completed.
    debug_print("[OTA] Starting OTA update");
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
      FS.end();
    }
    debug_print(str_str("[OTA] Start updating ",type));
  });
  ArduinoOTA.onEnd([]() {
    //int i = 2;
    debug_print("[OTA] update ended");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //int j = 2;
    debug_print(str_num("[OTA] progress: ", (float)(100.0 * (float) progress / total)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    debug_print(str_str(str_num("[OTA] Error[", (float)error),"]:"));
    if (error == OTA_AUTH_ERROR) {
      debug_print("[OTA] Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      debug_print("[OTA] Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      debug_print("[OTA] Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      debug_print("[OTA] Receive Failed");
    } else if (error == OTA_END_ERROR) {
      debug_print("[OTA] End Failed");
    }
  });
  ArduinoOTA.begin();
  debug_print("[SETUP] OTA Ready.");
  ota_setup_ready = true;
  //Serial.println("8");
  */
  //----------------------------------------//

/*
while(WiFiMulti.run() != WL_CONNECTED) { //here just to temporarily prevent crash, while ESP_Class hasn't been created
  delay(100);
}
*/

  //--------Check Server Connection---------//
  //server setup parameters: des_host, hml_host, prd_host, port, on_client_error, retry_interval
  // server address, port and URL
  /*
  STRING_TYPE current_host;
  //int n_hosts = sizeof(host)/sizeof(prd_host);
  boolean host_reached = false;
  if(wifi_setup_ready) {
    while(!host_reached) {
      if(connect_des_if_reachable && !str_empty(des_host)) {
        if(checkHost(des_host,port)) {
          current_host = des_host;
          host_reached = true;
        }
      } 
      if(!host_reached && connect_hml_if_reachable && !str_empty(hml_host)) {
        if(checkHost(hml_host,port)) {
          current_host = hml_host;
          host_reached = true;
        }
      }
      if(!host_reached) {
        if(checkHost(prd_host,port)) {
          current_host = prd_host;
          host_reached = true;
        }
      }
      if(!host_reached) {
        debug_print("[SETUP] Couldn't connect to any of the hosts listed.");
        if(str_eq(on_client_error,"retry")) {
          delay(str_to_i(retry_interval));
        }
      }
    }
    debug_print(str_str("[SETUP] Connected to Server: ", str_str(current_host, str_str(":",port))));
  } else {
    debug_print("[SETUP] Server check setup postponed, as a WiFi connection couldn't be made.");
  }
  if(host_reached) 
    server_setup_ready = true;
  */
  //----------------------------------------//
  
  //---------Setup SocketIO Client----------//
  //socketio parameters: host, port; socketIOEvent callback function
  /*
  if(server_setup_ready) {
    socketIO.begin(current_host, str_to_i(port));
    //socketIO.begin("192.168.0.100", 3030);
    // event handler
    //Serial.println("13");
    socketIO.onEvent(socketIOEvent);
    //socketIO.setAuthorization("2@2.2", "123"); //n funciona usando socketIO, só funciona usando WebSocket
    //socketIO.setExtraHeaders();
    debug_print("[SETUP] SocketIO client enabled.");
    io_setup_ready = true;
  } else {
    debug_print("[SETUP] SocketIO setup ignored, as no server could be reached.");
  }
  */
  //----------------------------------------//

  //--------------Safe Start----------------//   <-- In order to prevent the ESP from being unreachable if an immediate loop crash happens. If, in the future, the offline peripheral management capability is added, this must be removed.
    /*while(!begin_command_received) {
      ArduinoOTA.handle();
      socketIO.loop();
    }
    debug_print("starting to listen to commands.");*/
  //----------------------------------------//
}

//unsigned long messageTimestamp = 0;
void loop() {
  
    //if(ota_setup_ready) ArduinoOTA.handle();
    //if(io_setup_ready) socketIO.loop();
    /*if(begin_command_received) { // <-- In order to prevent the ESP from being unreachable if an immediate crash related to the peripherals happens. If, in the future, the offline peripheral management capability is added, this must be removed.
      hortaLoop();
      portalLoop();
    }*/
    /*
    //if(io_connected) {
      //if (millis() - messageTimestamp > 2000) {
        //portalRead();
        //messageTimestamp = millis();
        //debug_print(str_num("alive",messageTimestamp));
      //}
    //}
    */
   //check_status();
}


