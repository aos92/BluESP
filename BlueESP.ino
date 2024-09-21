#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "soc/soc.h" 
#include "soc/rtc_cntl_reg.h" 
#include <esp32-hal-ledc.h> 

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
unsigned long previousMillis = 0;
const long interval = 5000; // Interval pengiriman data 5 detik
unsigned long lastAdvertisedMillis = 0; // Untuk menyimpan waktu terakhir iklan BLE
const long restartInterval = 60000; // Interval restart 60 detik

int str1 = 0;
int str2 = 0;
int str3 = 0;
int str4 = 0;
int str5 = 0;
int str6 = 0;
int str7 = 0;
int str8 = 0;
int str9 = 0;

void generateRandomValues() {
  str1 = random(1, 100);
  str2 = random(1, 100);
  str3 = random(1, 100);
  str4 = random(1, 100);
  str5 = random(1, 100);
  str6 = random(1, 100);
  str7 = random(1, 100);
  str8 = random(1, 100);
  str9 = random(1, 100);
}

class MySecurity : public BLESecurityCallbacks {
    uint32_t onPassKeyRequest() {
        Serial.println("PassKeyRequest callback triggered");
        return 123456; 
    }

    void onPassKeyNotify(uint32_t pass_key) {
        Serial.print("Passkey Notify: ");
        Serial.println(pass_key);
    }

    bool onConfirmPIN(uint32_t pass_key) {
        Serial.print("Confirm PIN: ");
        Serial.println(pass_key);
        return true;
    }

    bool onSecurityRequest() {
        Serial.println("SecurityRequest callback triggered");
        return true;
    }

    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) {
        if (cmpl.success) {
            Serial.println("Authentication complete successfully!");
        } else {
            Serial.println("Authentication failed!");
        }
    }
};

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Device connected");
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Device disconnected");

        // Restart advertising setelah perangkat terputus
        delay(1000);  // Beri sedikit jeda sebelum mulai iklan ulang
        pServer->startAdvertising();
        Serial.println("Started advertising again");

        // Simpan waktu terakhir kali iklan dimulai
        lastAdvertisedMillis = millis();
    }
};

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable brownout detector

  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  Serial.println("Starting BLE work!");

  BLEDevice::init("ESP32");
  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
  BLEDevice::setSecurityCallbacks(new MySecurity());

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  // Mulai advertising BLE
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  Serial.println("Started advertising");

  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setKeySize(16);
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_ONLY);
  pSecurity->setCapability(ESP_IO_CAP_IO);
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

  generateRandomValues(); // Generate initial random values

  // Simpan waktu terakhir kali iklan dimulai
  lastAdvertisedMillis = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  if (deviceConnected) {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      generateRandomValues();

      String pesan = String(str1) + "," + String(str2) + "," + String(str3) + "," + String(str4) + "," + String(str5) + "," + String(str6) + "," + String(str7) + "," + String(str8) + "," + String(str9);
      pCharacteristic->setValue(pesan.c_str());
      pCharacteristic->notify();
      Serial.println("Sent message: " + pesan);
    }
  } else {
    // Restart iklan jika tidak ada perangkat terhubung dalam waktu tertentu
    if (currentMillis - lastAdvertisedMillis >= restartInterval) {
      Serial.println("No connection detected for too long, restarting BLE advertising...");
      pServer->startAdvertising(); // Restart advertising

      // Jika masalah terus berlanjut, lakukan restart ESP32
      //ESP.restart();
    }
  }
}
