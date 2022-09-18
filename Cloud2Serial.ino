#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "thingProperties.h"


//WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
struct keyvalue_pair_t {
  char    key[5];
  int     value;
};

const byte numChars = 32; // buffer size for serial reader
char receivedChars[numChars]; // input buffer for receiving from serial input
char tempChars[numChars];  

void setup() { 
  const char * SSID = "AIThermometer";
  const char * PASS = "password";
  
  // Setup Serial comm
  Serial.begin(9600);
  // FIRST Setup WIFI Connection
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  //wm.resetSettings();
  wm.setHostname(SSID);
  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name,
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  res = wm.autoConnect(SSID,PASS); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      ESP.restart();
  } 
  else { 
      Serial.println("connected...yeey :)");
  }
  SSID = wm.getWiFiSSID().c_str();
  PASS = wm.getWiFiPass().c_str();
  
  Serial.printf("SSID: %s \n", SSID); // debug

  // SECOND, setup Arduino Cloud connection
  WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
  initProperties();
  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
    boolean newData = false; 

    newData = recvWithStartEndMarkers();
    if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseKeyValuePair() replaces the commas with \0
       
        keyvalue_pair_t kv; 

        kv = parseKeyValuePair();
        processParsedData(kv);
        newData = false;
    }
    
    ArduinoCloud.update();
}

void onAntifreezeModeChange()  {
  Serial.printf("<AF, %d>", antifreezeMode);
}

void onTargetTemperature1Change()  { 
  Serial.printf("<TT1, %d>", targetTemperature1 / 100);
}

void onTargetTemperature2Change()  {
  Serial.printf("<TT2, %d>", targetTemperature2 / 100);
}

bool recvWithStartEndMarkers() {
    bool newData = false;
    static boolean recvInProgress = false;
    static byte indx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[indx] = rc;
                indx++;
                if (indx >= numChars) {
                    indx = numChars - 1;
                }
            }
            else {
                receivedChars[indx] = '\0'; // terminate the string
                recvInProgress = false;
                indx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
    return newData;
}

keyvalue_pair_t parseKeyValuePair() {      // split the data into its parts
    char *strtokIndx; // this is used by strtok() as an index
    keyvalue_pair_t kv;

    strtokIndx = strtok(tempChars,",");      // get the first part - the tag
    strcpy(kv.key, strtokIndx); // copy it to KEY 
 
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    kv.value = atoi(strtokIndx);     // convert this part to an integer

    return kv;
}

void processParsedData(keyvalue_pair_t &kv) {
  // take appropriate actions according to the tag
  if (strcmp(kv.key, "T1") == 0){
    actualTemperature1 = kv.value / 100.0;
  }
  else if (strcmp(kv.key, "T2") == 0) {
    actualTemperature2 = kv.value / 100.0;
  }
  else if (strcmp(kv.key, "TT1") == 0){
    targetTemperature1 = kv.value / 100.0;
  }
  else if (strcmp(kv.key, "TT2") == 0) {
    targetTemperature2 = kv.value / 100.0;
  }
  else if (strcmp(kv.key, "AF") == 0) {
    antifreezeMode = (bool)kv.value;
  }
  else {
    // do some error handling here. Ingoring it now.
  }

}
