#define USE_CLI
//#define USE_DPS

#if defined(USE_CLI)
#define USE_LOGO
#define PNP_DEMO
// Wi-Fi
#define IOT_CONFIG_WIFI_SSID				Storage::WiFiSSID.c_str()
#define IOT_CONFIG_WIFI_PASSWORD			Storage::WiFiPassword.c_str()

// Azure IoT Hub DPS
#define IOT_CONFIG_GLOBAL_DEVICE_ENDPOINT	"global.azure-devices-provisioning.net"
#define IOT_CONFIG_ID_SCOPE					Storage::IdScope
#define IOT_CONFIG_REGISTRATION_ID			Storage::RegistrationId
#define IOT_CONFIG_SYMMETRIC_KEY			Storage::SymmetricKey
#define IOT_CONFIG_MODEL_ID					"dtmi:seeedkk:wioterminal:wioterminal_aziot_example;1"

#else // USE_CLI

// Wi-Fi
#define IOT_CONFIG_WIFI_SSID				"[wifi ssid]"
#define IOT_CONFIG_WIFI_PASSWORD			"[wifi password]"

#if !defined(USE_DPS)
// Azure IoT Hub
#define IOT_CONFIG_IOTHUB					"[Azure IoT Hub host name].azure-devices.net"
#define IOT_CONFIG_DEVICE_ID				"[device id]"
#define IOT_CONFIG_SYMMETRIC_KEY			"[symmetric key]"
#define IOT_CONFIG_MODEL_ID					"dtmi:seeedkk:wioterminal:wioterminal_aziot_example;1"
#else // USE_DPS
// Azure IoT Hub DPS
#define IOT_CONFIG_GLOBAL_DEVICE_ENDPOINT	"global.azure-devices-provisioning.net"
#define IOT_CONFIG_ID_SCOPE					"[id scope]"
#define IOT_CONFIG_REGISTRATION_ID			"[registration id]"
#define IOT_CONFIG_SYMMETRIC_KEY			"[symmetric key]"
#define IOT_CONFIG_MODEL_ID					"dtmi:seeedkk:wioterminal:wioterminal_aziot_example;1"
#endif // USE_DPS

#endif // USE_CLI

#define TOKEN_LIFESPAN                      3600

#define TELEMETRY_FREQUENCY_MILLISECS		2000
#define TELEMETRY_ACCEL_X					"accelX"
#define TELEMETRY_ACCEL_Y					"accelY"
#define TELEMETRY_ACCEL_Z					"accelZ"
#define COMMAND_RING_BUZZER					"ringBuzzer"
