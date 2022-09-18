// Code generated by Arduino IoT Cloud, DO NOT EDIT.

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char DEVICE_LOGIN_NAME[]  = "96fbf52e-5ea5-4d1b-bb55-62c9ea7183d8";
const char DEVICE_KEY[]         = "BXZ9WV2LAETUFPZBUF9P";    // Secret device password

void onTargetTemperature1Change();
void onTargetTemperature2Change();
void onAntifreezeModeChange();

float actualTemperature1;
float actualTemperature2;
float targetTemperature1;
float targetTemperature2;
bool antifreezeMode;

void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(actualTemperature1, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(actualTemperature2, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(targetTemperature1, READWRITE, ON_CHANGE, onTargetTemperature1Change);
  ArduinoCloud.addProperty(targetTemperature2, READWRITE, ON_CHANGE, onTargetTemperature2Change);
  ArduinoCloud.addProperty(antifreezeMode, READWRITE, ON_CHANGE, onAntifreezeModeChange);

}