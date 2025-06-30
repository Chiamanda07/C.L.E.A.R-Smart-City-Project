#include <SPI.h>
#include <MFRC522.h>

#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "Elora"
#define WIFI_PASSWORD "mandy2007"


#define API_KEY "AIzaSyB0r3jMrDJB3qkqo68h1Tkwnh2Qo5TKfsk"
#define DATABASE_URL "https://smart-gps-tracker-4bef3-default-rtdb.firebaseio.com/"
#define USER_EMAIL "yuribenjamins24@gmail.com"
#define USER_PASSWORD "2Burner2024"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

#define RST_PIN 22
#define SS_PIN 21
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("RC522 ready.");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300); Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
}


void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  Serial.print("Card UID: "); Serial.println(uid);

  String timestamp = "Time_" + String(millis());
  String path = "/rfid_logs/" + timestamp;


  Firebase.setString(fbdo, path + "/uid", uid);
  Firebase.setString(fbdo, path + "/timestamp", timestamp);

  Serial.println("Logged to Firebase.");

  delay(2000);
}


