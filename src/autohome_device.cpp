#include <autohome_device.h>
#include <auto_format.h>


void debug_print(STRING_TYPE message) {
    Device.debug_print(message);
}

void AutohomeDeviceClass::debug_print(STRING_TYPE message) {
//void debug_print(STRING_TYPE message) {
  if(_server_debug_on && _io_connected) postIoT_log(message);
  if(_serial_debug_on && _serial_enabled) USE_SERIAL.println(message);
}

void AutohomeDeviceClass::postIoT_log(STRING_TYPE message) {
  if(Format.sNul(message)) { debug_print("Message NULL in postIoT_log"); return; }
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  array.add("create");
  array.add("iot-logs");
  JsonObject param1 = array.createNestedObject();
  param1["text"] = message;
  String output;
  serializeJson(doc, output);
  _socketIO.sendEVENT(output);
}

void AutohomeDeviceClass::postMessage(STRING_TYPE message) {
    if(Format.sNul(message)) { debug_print("Message NULL"); return; }
    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();
    array.add("create");
    array.add("messages");
    JsonObject param1 = array.createNestedObject();
    param1["text"] = message;
    String output;
    serializeJson(doc, output);
    _socketIO.sendEVENT(output);
}

void AutohomeDeviceClass::socketIOAuthenticate() {
    debug_print("begin socketIOAuthenticate");
    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();
    array.add("create");
    array.add("authentication");
    JsonObject param1 = array.createNestedObject();
    param1["strategy"] = "local";
    param1["email"] = "esp_garagem@iot";
    param1["password"] = "123";
    String output;
    serializeJson(doc, output);
    _socketIO.sendEVENT(output);
    debug_print("end socketIOAuthenticate");
}

void AutohomeDeviceClass::addEventCallback(TAppEventCallback cb) {
    eventCallbacks.push_back(cb);
}

void AutohomeDeviceClass::invokeEventCallbacks(STRING_TYPE event) {
    //std::for_each(eventCallbacks.begin(), eventCallbacks.end(), std::ref(*this));
    std::for_each(eventCallbacks.begin(), eventCallbacks.end(), [event](TAppEventCallback cb) {
        cb(event);
    });
}

void AutohomeDeviceClass::socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
//void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
  debug_print("[IoC] socketIO msg received");
    switch(type) {
        case sIOtype_DISCONNECT:
            {
            debug_print("[IOc] Disconnected.");
            _io_connected = false;
            _server_debug_on = false;
            //_begin_command_received = false;
            }
            break;
        case sIOtype_CONNECT:
            {
            _io_connected = true;
            _server_debug_on = true;
            if(Format.sLen(payload) > 0)
              debug_print(Format.ss("[IOc] Connected to url: ", payload));
            }
            break;
        case sIOtype_EVENT:
            {
            //char * sptr = NULL;
            //int id = strtol((char *)payload, &sptr, 10);
            debug_print(Format.ss("[IOc] get event: ", (STRING_TYPE)payload));
            
            //if(id) {
            //    payload = (uint8_t *)sptr;
            //}
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, payload, length);
            if(error) {
                debug_print(Format.ss("deserializeJson() failed: ",error.c_str()));
                return;
            }
            STRING_TYPE eventName = doc[0];
            debug_print(Format.ss("[IOc] event name: ", eventName));
            // Message Includes a ID for a ACK (callback)
            /*
            if(id) {
                // creat JSON message for Socket.IO (ack)
                DynamicJsonDocument docOut(1024);
                JsonArray array = docOut.to<JsonArray>();
                // add payload (parameters) for the ack (callback function)
                JsonObject param1 = array.createNestedObject();
                param1["now"] = millis();
                // JSON to String (serializion)
                String output;
                output += id;
                serializeJson(docOut, output);
                // Send event
                socketIO.send(sIOtype_ACK, output);
            }*/
            if(Format.sEqu(eventName,"authentication expected") || Format.sEqu(eventName,"authentication expiring") || Format.sEqu(eventName,"authentication expired")) {
              socketIOAuthenticate();
              //postMessage("soy trochita");
            }
            else if(Format.sEqu(eventName,"commands begin")) {
              debug_print("Event is of type \"commands begin\"");
              _begin_command_received = true;
            }
            else if(Format.sEqu(eventName,"commands listen")) {
              debug_print("Event is of type \"commands listen\"");
              _listen_to_commands = true;
            }
            else if(Format.sEqu(eventName,"commands created")) {
              debug_print("Event is of type \"commands created\"");
              if(!_listen_to_commands) {
                postMessage("Dispositivo ainda não liberado para receber comandos");
                break;
              }
              JsonObject data = doc[1];
              //int user_id = data["user_id"];
              //int target_user_id = data["target_user_id"];
              JsonObject entities = data["entities"];
              JsonObject horta;
              JsonObject portal;
              if(entities["horta"]) { horta = entities["horta"]; debug_print("horta entity exists in request"); }
              if(entities["portal"]) { portal = entities["portal"]; debug_print("portal entity exists in request"); }
              STRING_TYPE portal_request;
              STRING_TYPE horta_request;
              if(portal["request"]) portal_request = portal["request"]; else portal_request = "invalid";
              if(horta["request"]) horta_request = horta["request"]; else horta_request = "invalid";
              JsonObject portal_request_options;
              JsonObject horta_request_options;
              if(portal["options"]) portal_request_options = portal["options"]; /*else portal_action_options = "";*/
              if(horta["options"]) horta_request_options = horta["options"]; /*else horta_action_options = "";*/
              //if(!Format.sEqu(horta_request,"invalid")) { postMessage(hortaInteract(horta_request, horta_request_options)); torneira_just_closed = false; }
              //if(!Format.sEqu(portal_request,"invalid")) { postMessage(portalInteract(portal_request, portal_request_options)); portal_changed = false; }
            }
        }
            break;
        case sIOtype_ACK:
            debug_print(Format.ss("[IOc] get ack: ", (int)length));
            hexdump(payload, length);
            break;
        case sIOtype_ERROR:
            debug_print(Format.ss("[IOc] get error: ", (int)length));
            hexdump(payload, length);
            break;
        case sIOtype_BINARY_EVENT:
            debug_print(Format.ss("[IOc] get binary: ", (int)length));
            hexdump(payload, length);
            break;
        case sIOtype_BINARY_ACK:
            debug_print(Format.ss("[IOc] get binary ack: ", (int)length));
            hexdump(payload, length);
            break;
    }
}

//-----------Safe Serial setup------------//
//REMEMBER to disconnect RX / TX pins if using Serial!!
boolean AutohomeDeviceClass::setupSerial() {
    if(_serial_debug_on) {
        setSerialStatus(true, 115200);
        USE_SERIAL.setDebugOutput(true);
        for(int t = 4; t > 0; t--) {
            debug_print(Format.ss("[SETUP] BOOT WAIT ", t));
            USE_SERIAL.flush();
            delay(1000);
        }
        return true;
    } else {
        setSerialStatus(false);
        //pinMode(RX_PIN, INPUT);
        debug_print("[SETUP] Serial disabled.");
        if(_rx_pin_is_gpio) {
            debug_print("[SETUP] RX pin changed to GPIO input.");
        }
        if(_tx_pin_is_gpio) {
            debug_print("[SETUP] TX pin changed to GPIO output.");
        }
        return false;
    }
}

boolean AutohomeDeviceClass::setupPIN() { //returns false if RX / TX pins don't change to GPIO mode
    //PinModes defined here...
    boolean serialResult = setupSerial();
    _pin_setup_ready = true; //Only set this after RX pin usage has been decided
    return !serialResult;
}

boolean AutohomeDeviceClass::setupConfig() {
    boolean mount_FS_fail = false;
    boolean open_original_config_fail = false;
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
    JSON_VALUE_STRING_TYPE port;
    JSON_VALUE_STRING_TYPE device;
    JSON_VALUE_STRING_TYPE device_pwd;
    JSON_VALUE_STRING_TYPE on_client_error;
    JSON_VALUE_STRING_TYPE retry_interval;
    DynamicJsonDocument doc(DEFAULT_CONFIG_FILE_SIZE*2+100);
    if(!mount_FS_fail) {
        File conf = FS.open(CONFIG_FILE, "r");
        if (!conf) {
            open_original_config_fail = true;
            if(FS.exists(CONFIG_FILE)) {
                if(_overwrite_config_on_error) {
                debug_print("[CONF] Config file exists but couldn't be opened; overwriting with default config file.");
                } else {
                    debug_print("[CONF] Config file exists but couldn't be opened; config parameters will be set with default values.");
                    open_config_fail = true;
                }
            } else {
                debug_print("[CONF] Config file not found; creating default config file.");
            }
            conf.close();
            if(!open_config_fail) {
                conf = FS.open(CONFIG_FILE, "w");
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
            }
        } else conf.close();
        if(!open_config_fail) {
            File config_file = FS.open(CONFIG_FILE, "r");
            if(config_file) {
                debug_print(Format.ss("[CONF] config file size: ", (int)config_file.size()));
                String config_data;
                while(config_file.available()) {
                    config_data += config_file.readStringUntil('\n');
                }
                DeserializationError error = deserializeJson(doc, config_data);
                if (error) {
                    debug_print(Format.ss("[ERROR][CONF] deserializeJson() failed. Error: ", error.c_str()));
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
    if(!Format.sNul(ssid)) debug_print(Format.ss("[CONF] SSID: ",ssid));
    else {
        debug_print(Format.ss("[CONF] SSID not defined; setting to \"",Format.ss(DEFAULT_SSID,"\".")));
        ssid = DEFAULT_SSID;//"home_net";
    }
    if(!Format.sNul(password)) debug_print(Format.ss("[CONF] Password: ",password));
    else {
        debug_print(Format.ss("[CONF] Password not defined; setting to \"",Format.ss(DEFAULT_PASSWORD,"\".")));
        password = DEFAULT_PASSWORD;//"1234";
    }
    if(!Format.sNul(des_host)) debug_print(Format.ss("[CONF] DES Host: ", des_host)); else debug_print("[CONF] DES Host not defined.");
    if(!Format.sNul(hml_host)) debug_print(Format.ss("[CONF] HML Host: ", hml_host)); else debug_print("[CONF] HML Host not defined.");
    if(!Format.sNul(prd_host)) debug_print(Format.ss("[CONF] PRD Host: ",prd_host));
    else {
        if(Format.sNul(des_host) && Format.sNul(hml_host)) {
            debug_print(Format.ss("[CONF] PRD Host not defined; setting to \"",Format.ss(DEFAULT_PRD_HOST,"\".")));
            prd_host = DEFAULT_PRD_HOST;//"192.168.0.100";
        }
        else debug_print("[CONF] PRD Host not defined.");
    }
    if(!Format.sNul(port)) debug_print(Format.ss("[CONF] Port: ",port));
    else {
        debug_print(Format.ss("[CONF] Port not defined; setting to \"",Format.ss(DEFAULT_PORT,"\".")));
        port = DEFAULT_PORT;//"3030";
    }
    if(!Format.sNul(device)) debug_print(Format.ss("[CONF] Device Name: ",device));
    else {
        debug_print(Format.ss("[CONF] Device Name empty; setting to \"",Format.ss(DEFAULT_DEVICE_NAME,"\".")));
        device = DEFAULT_DEVICE_NAME;//"Generic_Device";
    }
    if(!Format.sNul(device_pwd)) debug_print(Format.ss("[CONF] Device Password: ",device_pwd)); else debug_print("[CONF] Device Password empty.");
    if(!Format.sNul(on_client_error)) debug_print(Format.ss("[CONF] Action on server connection error: ",on_client_error));
    else {
        debug_print(Format.ss("[CONF] Action on server connection error not defined; setting to \"",Format.ss(DEFAULT_ON_CLIENT_ERROR_ACTION,"\".")));
        on_client_error = DEFAULT_ON_CLIENT_ERROR_ACTION;//"retry";
    }
    if(!Format.sNul(retry_interval)) debug_print(Format.ss("[CONF] Retry interval if action is \"retry\": ",retry_interval));
    else {
        debug_print(Format.ss("[CONF] Retry interval if action is \"retry\" not defined; setting to \"",Format.ss(DEFAULT_RETRY_INTERVAL,"\" milliseconds.")));
        retry_interval = DEFAULT_RETRY_INTERVAL;//"5000";
    }
    _config_setup_ready = true;
    return !open_original_config_fail;
}


//STRING_TYPE getSSID();
STRING_TYPE AutohomeDeviceClass::getSSID() {
    return (STRING_TYPE)WiFi.SSID().c_str();
}
//STRING_TYPE getPSWD();
STRING_TYPE AutohomeDeviceClass::getPSWD() {
    return (STRING_TYPE)WiFi.psk().c_str();
}
//STRING_TYPE getSTAIP();
STRING_TYPE AutohomeDeviceClass::getSTAIP() {
    return (STRING_TYPE)WiFi.localIP().toString().c_str();
}

boolean AutohomeDeviceClass::portalParamErrorDisclaimer(ap_param_error_t paramError) {
    //-------- "ESP as AP" Parameters Configuration ---------//
    switch(paramError) {
        case NO_AP_PARAM_ERROR:
            return false;
        case INVALID_EMPTY_SSID:
            debug_print("[ERROR][WIFI] SSID cannot be null");
            return true;
        case INVALID_LONG_SSID:
            debug_print("[ERROR][WIFI] SSID cannot have more than 32 characters");
            return true;
        case INVALID_LONG_PSWD:
            debug_print("[ERROR][WIFI] Password cannot have more than 32 characters");
            return true;
        case INVALID_LONG_TIMEOUT:
            debug_print("[ERROR][WIFI] Timeout cannot be more than 10 minutes");
            return true;
        case INVALID_APIP:
            debug_print("[ERROR][WIFI] Invalid Access Point IP");
            return true;
        case INVALID_GWIP:
            debug_print("[ERROR][WIFI] Invalid Gateway IP");
            return true;
        case INVALID_SNIP:
            debug_print("[ERROR][WIFI] Invalid SubNet IP");
            return true;
    }
}

ap_return_status_t AutohomeDeviceClass::configPortal(STRING_TYPE apSSID, STRING_TYPE apPSWD, STRING_TYPE timeout, STRING_TYPE apIP, STRING_TYPE gwIP, STRING_TYPE snIP) {
    ap_param_error_t paramError;
    if(Format.sNul(apSSID)) paramError = INVALID_EMPTY_SSID;
    if(Format.sLen(apSSID) > 32) paramError = INVALID_LONG_SSID;
    if(Format.sLen(apPSWD) > 32) paramError = INVALID_LONG_PSWD;
    unsigned long timeout_ = Format.s2ul(timeout);
    if(timeout_ > 10 * 60 * 1000) paramError = INVALID_LONG_TIMEOUT;
    IPAddress apIPA;
    if(!apIPA.fromString(Format.cc2s(apIP))) paramError = INVALID_APIP;
    IPAddress gwIPA;
    if(!gwIPA.fromString(Format.cc2s(gwIP))) paramError = INVALID_GWIP;
    IPAddress snIPA;
    if(!snIPA.fromString(Format.cc2s(snIP))) paramError = INVALID_SNIP;
    paramError = NO_AP_PARAM_ERROR;
    if(portalParamErrorDisclaimer(paramError)) {
        return INVALID_PARAMS;
    }
    return configPortal(apSSID, apPSWD, timeout_, apIPA, gwIPA, snIPA);
}

ap_return_status_t AutohomeDeviceClass::configPortal(STRING_TYPE apSSID, STRING_TYPE apPSWD, unsigned long timeout, IPAddress apIP, IPAddress gwIP, IPAddress snIP) {
    ESP_WiFiManager ESP_wifiManager(_device);
    ESP_wifiManager.setDebugOutput(false);
    if(timeout)
        ESP_wifiManager.setConfigPortalTimeout(timeout);
    ESP_wifiManager.setAPStaticIPConfig(apIP, gwIP, snIP);
    if(ESP_wifiManager.startConfigPortal(apSSID, apPSWD)) {
        return WIFI_CONNECTED;
    } else {
        return CONNECTION_FAIL;
    }
}

boolean AutohomeDeviceClass::setupWiFi() {
    if(WiFi.getMode() & WIFI_AP) {
        WiFi.softAPdisconnect(true);
    }
    if(!_wifiMulti.existsAP(_ssid,_password)) {
        if(_wifiMulti.addAP(_ssid,_password)) {
        debug_print("[SETUP] WiFi credentials stored.");
        } else {
        debug_print("[ERROR][SETUP] WiFi credentials could not be stored.");
        }
    }
    //WiFi.disconnect();
    unsigned long wifi_timeout_counter = millis();
    unsigned long wifi_timeout = 10000; // If connection couldn't be established after 10 seconds, log the misconnection.
    boolean wifi_setup_fail = false;
    int attempt_count = 1;
    int max_attempts_before_config_portal = 2;
    int config_portal_attempt_count = 1;
    int max_config_portal_attempts = 2;
    String router_SSID;
    String router_Pass;
    debug_print(Format.ss(Format.ss(Format.ss("[SETUP] Trying to connect; attempt ", attempt_count), " of "), max_attempts_before_config_portal));
    while(_wifiMulti.run() != WL_CONNECTED) {
        delay(100);
        if(millis() - wifi_timeout_counter > wifi_timeout) {
            debug_print(Format.ss(Format.ss("[ERROR][SETUP] WiFi automatic connection failed after trying for ", (float)wifi_timeout * (float)0.001), " seconds; parameters:"));
            debug_print(Format.ss("ssid: ", getSSID()));
            debug_print(Format.ss("password: ", getPSWD()));
            attempt_count++;
            if(attempt_count > max_attempts_before_config_portal) {
                attempt_count = 1;
                debug_print("[SETUP] Maximum attempts for automatic connection reached; setting up Access Point and Config Portal.");
                //---------------- WiFi Manager Instance ----------------//
                ap_return_status_t ap_return_ = configPortal(_device, _device_pwd, "0", "192.168.100.1", "192.168.100.1", "255.255.255.0");
                //-------------------------------------------------------//
                if(ap_return_ == WIFI_CONNECTED) {
                    continue;
                } else if (ap_return_ == INVALID_PARAMS) {
                    wifi_setup_fail = true;
                    break;
                } else if (ap_return_ == CONNECTION_FAIL) {
                    debug_print("[ERROR][SETUP] Config Portal couldn't establish a WiFi connection; parameters:");
                    debug_print(Format.ss("ssid: ", getSSID()));
                    debug_print(Format.ss("password: ", getPSWD()));
                    if(config_portal_attempt_count >= max_config_portal_attempts) {
                        debug_print("[SETUP] Maximum number of WiFi connection attempts via Config Portal reached; exiting WiFi setup.");
                        wifi_setup_fail = true; //flag not being used
                        break;
                    } else {
                        debug_print("[SETUP] Trying to connect again; if still no success, Config Portal will be reopened.");
                        config_portal_attempt_count++;
                    }
                }
            }
            debug_print(Format.ss(Format.ss(Format.ss("[SETUP] Trying to connect; attempt ", attempt_count), " of "), max_attempts_before_config_portal));
            wifi_timeout_counter = millis();
        }
    }
    if(!wifi_setup_fail) {
        debug_print(Format.ss("[SETUP] WiFi Connected; IP: ", getSTAIP()));
        _wifi_setup_ready = true;
        return true;
    }
    return false;
}

void AutohomeDeviceClass::setupOTA() {
    //OTA setup parameters: device, device_pwd, ota_port
    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);
    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname(_device);
    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    //ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
    if(!Format.sNul(_device_pwd)) ArduinoOTA.setPassword(_device_pwd);
    ArduinoOTA.onStart([]() {
        if(Device.getSerialStatus()) {
            Device.setSerialStatus(false); // If Serial is on, this safely ends it
        }
        Device.debug_print("[OTA] Starting OTA update");
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
        } else { // U_FS
        type = "filesystem";
        FS.end();
        }
        Device.debug_print(Format.ss("[OTA] Start updating ",type));
    });
    ArduinoOTA.onEnd([]() {
        //int i = 2;
        Device.debug_print("[OTA] update ended");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        //int j = 2;
        Device.debug_print(Format.ss("[OTA] progress: ", (float)(100.0 * (float) progress / total)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Device.debug_print(Format.ss(Format.ss("[OTA] Error[", (float)error),"]:"));
        if (error == OTA_AUTH_ERROR) {
        Device.debug_print("[OTA] Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
        Device.debug_print("[OTA] Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
        Device.debug_print("[OTA] Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
        Device.debug_print("[OTA] Receive Failed");
        } else if (error == OTA_END_ERROR) {
        Device.debug_print("[OTA] End Failed");
        }
    });
    ArduinoOTA.begin();
    debug_print("[SETUP] OTA Ready.");
    _ota_setup_ready = true;
}

boolean AutohomeDeviceClass::checkHost(JSON_VALUE_STRING_TYPE address, JSON_VALUE_STRING_TYPE port) {
    WiFiClient client;
    HTTPClient http;
    debug_print("[HTTP] begin...\n");
    debug_print(address);
    debug_print(port);
    STRING_TYPE host = Format.ss("http://",Format.ss(address,Format.ss(":",port)));
    debug_print(host);
    if (http.begin(client, host)) {  // HTTP
        int httpCode = http.GET();   // start connection and send HTTP header
        if (httpCode > 0) {          // httpCode will be negative on error
            debug_print(Format.ss("[HTTP] GET... code: ", httpCode));
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                String payload = http.getString();
                debug_print(Format.ss("Server answered: ",host));
                http.end();
                return true;
            }
        } else {
            debug_print(Format.ss("[HTTP] GET... failed, error: ", http.errorToString(httpCode).c_str()));
        }
    } else {
        debug_print(Format.ss("[HTTP] Couldn't connect to server ",host));
    }
    http.end();
    return false;
}

boolean AutohomeDeviceClass::setupServer() {
    boolean host_reached = false;
    if(_wifi_setup_ready) {
        while(!host_reached) {
            if(_connect_des_if_reachable && !Format.sNul(_des_host)) {
                if(checkHost(_des_host,_port)) {
                _current_host = _des_host;
                _current_port = _port;
                host_reached = true;
                }
            } 
            if(!host_reached && _connect_hml_if_reachable && !Format.sNul(_hml_host)) {
                if(checkHost(_hml_host,_port)) {
                _current_host = _hml_host;
                _current_port = _port;
                host_reached = true;
                }
            }
            if(!host_reached) {
                if(checkHost(_prd_host,_port)) {
                _current_host = _prd_host;
                _current_port = _port;
                host_reached = true;
                }
            }
            if(!host_reached) {
                debug_print("[SETUP] Couldn't connect to any of the hosts listed.");
                if(Format.sEqu(_on_client_error,"retry")) {
                delay(Format.s2i(_retry_interval));
                }
            }
        }
        debug_print(Format.ss("[SETUP] Connected to Server: ", Format.ss(_current_host, Format.ss(":",_port))));
    } else {
        debug_print("[SETUP] Server check setup postponed, as a WiFi connection couldn't be made.");
    }
    if(host_reached) {
        _server_setup_ready = true;
        return true;
    } else if(!_io_setup_ready) {
        _server_setup_ready = false;
    }
    return false;
}

boolean AutohomeDeviceClass::setupIO(SocketIOclient::SocketIOclientEvent socketIOEvent) {
    //socketio parameters: host, port; socketIOEvent callback function
    if(_server_setup_ready) {
        _socketIO.begin(_current_host, Format.s2i(_current_port));
        //socketIO.begin("192.168.0.100", 3030);
        // event handler
        //Serial.println("13");
        _socketIO.onEvent(socketIOEvent);
        //socketIO.setAuthorization("2@2.2", "123"); //n funciona usando socketIO, só funciona usando WebSocket
        //socketIO.setExtraHeaders();
        debug_print("[SETUP] SocketIO client enabled.");
        _io_setup_ready = true;
        return true;
    } else {
        debug_print("[SETUP] SocketIO setup ignored, as no server could be reached.");
        return false;
    }
}

//boolean getSerialStatus();
boolean AutohomeDeviceClass::getSerialStatus() {
    return _serial_enabled;
}
//boolean setSerialStatus();
boolean AutohomeDeviceClass::setSerialStatus(boolean status, int baudRate) {
    if(status) {
        if(!Serial) {
            if((_rx_pin_is_gpio && !_rx_pin_interchangeable) || (_tx_pin_is_gpio && !_tx_pin_interchangeable)) {
                return false;
            }
            if(_rx_pin_is_gpio) {
                pinMode(RX_PIN, UART_PIN_AS_UART);
                _rx_pin_is_gpio = false;
            }
            if(_tx_pin_is_gpio) {
                pinMode(TX_PIN, UART_PIN_AS_UART);
                _tx_pin_is_gpio = false;
            }
            Serial.begin(baudRate);
            _serial_debug_on = true;
            _serial_enabled = true;
            _rx_pin_interchangeable = false; // may be removed if connected sensor permits
            _tx_pin_interchangeable = false; // may be removed if connected actuator permits
        }
    } else {
        if(Serial) {
            Serial.end();
            _serial_debug_on = false;
            _serial_enabled = false;
        }
        if(_rx_pin_interchangeable) {
            pinMode(RX_PIN, UART_PIN_AS_GPIO);
            _rx_pin_is_gpio = true;
            pinMode(RX_PIN, INPUT);
        }
        if(_tx_pin_interchangeable) {
            pinMode(TX_PIN, UART_PIN_AS_GPIO);
            _tx_pin_is_gpio = true;
            pinMode(TX_PIN, OUTPUT);
        }
        _rx_pin_interchangeable = false; // may be removed if connected sensor permits
        _tx_pin_interchangeable = false; // may be removed if connected actuator permits
    }
    return true;
}