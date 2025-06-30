// ------initialize section ---- //
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <SPI.h>
#include <MFRC522.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>


// Wi-Fi & Firebase
#define WIFI_SSID "16Dubem"
#define WIFI_PASSWORD "DubemIsBrokeAsf"




#define API_KEY "AIzaSyB0r3jMrDJB3qkqo68h1Tkwnh2Qo5TKfsk"
#define DATABASE_URL "https://smart-gps-tracker-4bef3-default-rtdb.firebaseio.com/"
#define USER_EMAIL "yuribenjamins24@gmail.com"
#define USER_PASSWORD "2Burner2024"


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


// RFID
#define RST_PIN 22
#define SS_PIN 21
MFRC522 mfrc522(SS_PIN, RST_PIN);


// Ultrasonic
#define TRIG_PIN 12
#define ECHO_PIN 13


// GPS
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
#define GPS_RX 16
#define GPS_TX 17

/*-------------Setup Section-------------*/
void setup() {
  Serial.begin(115200);


  // Init RFID
  SPI.begin();
  mfrc522.PCD_Init();


  // Init Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);


  // Init GPS
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);


  // Connect Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300); Serial.print(".");
  }
  Serial.println("\nWiFi connected");


  // Init Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}
/*-------------Setup Section-------------*/

// --------- Sensor Functions --------- //


String readRFID() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return "";


  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading
  return uid;
}


float readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;  // Distance in cm (Check tinkercad to see if you want to add anything to this code for ultrasonic)
}


bool readGPS(float &lat, float &lng) {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
  if (gps.location.isUpdated()) {
    lat = gps.location.lat();
    lng = gps.location.lng();
    return true;
  }
  return false;
}


// --------- Main Loop ---------


void loop() {
  String uid = readRFID();
  if (uid != "") {
    Serial.print("User: "); Serial.println(uid);


    float fill = readUltrasonic();
    Serial.print("Fill level (cm): "); Serial.println(fill);


    float lat = 0.0, lng = 0.0;
    bool gpsOK = readGPS(lat, lng);
   
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);


    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);


    String timestamp = "Time_" + String(millis());
    String path = "/smart_bins/" + timestamp;


    Firebase.setString(fbdo, path + "/user", uid);
    Firebase.setFloat(fbdo, path + "/fillLevel_cm", fill);
    Firebase.setString(fbdo, path + "/timestamp", timestamp);


    if (gpsOK) {
      Firebase.setFloat(fbdo, path + "/lat", lat);
      Firebase.setFloat(fbdo, path + "/lng", lng);
    } else {
      Firebase.setString(fbdo, path + "/location", "GPS Not Ready");
    }


    Serial.println("Logged to Firebase.\n");
    delay(3000);
  }
}
/*-------------Loop Section-------------*/