// ------initialize section ---- //
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <SPI.h>
#include <MFRC522.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>

// Wi-Fi & Firebase
#define WIFI_SSID "wifi_name"
#define WIFI_PASSWORD "password"
#define API_KEY "AIzaSyB0r3jMrDJB3qkqo68h1Tkwnh2Qo5TKfsk"
#define DATABASE_URL "https://smart-gps-tracker-4bef3-default-rtdb.firebaseio.com/"
#define USER_EMAIL "email"
#define USER_PASSWORD "password"

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

// Gas Sensor (Add this to your hardware)
#define GAS_SENSOR_PIN 34  // Analog pin for gas sensor (MQ-2 or similar)

// GPS
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
#define GPS_RX 16
#define GPS_TX 17

// Waste bin configuration
String binID = "BIN_001"; // Unique identifier for this waste bin
float binHeight = 30.0; // Total height of bin in cm (adjust based on your bin)
unsigned long lastDataSend = 0;
const unsigned long DATA_SEND_INTERVAL = 10000; // Send data every 10 seconds
bool signupOK = false;

struct SensorData {
  float fillLevel;
  float distanceFromSensor;
  float latitude;
  float longitude;
  int gasLevel;
  String rfidTag;
  unsigned long timestamp;
  String binStatus;
  bool gpsValid;
};

/*-------------Setup Section-------------*/
void setup() {
  Serial.begin(115200);
  
  // Init RFID
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("RFID initialized");
  
  // Init Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.println("Ultrasonic sensor initialized");
  
  // Init Gas Sensor
  pinMode(GAS_SENSOR_PIN, INPUT);
  Serial.println("Gas sensor initialized");
  
  // Init GPS
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  Serial.println("GPS initialized");
  
  // Connect Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300); 
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Init Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.token_status_callback = tokenStatusCallback;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  // Sign up or sign in
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase signup successful");
    signupOK = true;
  } else {
    Serial.printf("Firebase signup failed: %s\n", config.signer.signupError.message.c_str());
  }
  
  Serial.println("Smart Waste Bin System Started");
  Serial.println("Bin ID: " + binID);
  Serial.println("===========================");
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
  digitalWrite(TRIG_PIN, LOW); 
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); 
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;  // Distance in cm
  
  // Filter out invalid readings
  if (distance < 2 || distance > binHeight) {
    return -1; // Invalid reading
  }
  
  return distance;
}

int readGasSensor() {
  int gasLevel = analogRead(GAS_SENSOR_PIN);
  return gasLevel;
}

bool readGPS(float &lat, float &lng) {
  bool newData = false;
  
  // Read GPS data for up to 1 second
  unsigned long start = millis();
  while (millis() - start < 1000) {
    while (gpsSerial.available() > 0) {
      if (gps.encode(gpsSerial.read())) {
        newData = true;
      }
    }
  }
  
  if (gps.location.isValid() && newData) {
    lat = gps.location.lat();
    lng = gps.location.lng();
    return true;
  }
  return false;
}

float calculateFillLevel(float distance) {
  if (distance < 0) return -1; // Invalid reading
  
  // Calculate fill level percentage
  float fillLevel = ((binHeight - distance) / binHeight) * 100;
  fillLevel = constrain(fillLevel, 0, 100);
  
  return fillLevel;
}

String determineBinStatus(float fillLevel, int gasLevel) {
  if (fillLevel < 0) {
    return "SENSOR_ERROR";
  } else if (fillLevel >= 90) {
    return "FULL";
  } else if (gasLevel > 500) { 
    return "GAS_ALERT";
  } else if (fillLevel >= 70) {
    return "NEARLY_FULL";
  } else if (fillLevel >= 50) {
    return "HALF_FULL";
  } else {
    return "NORMAL";
  }
}

SensorData collectSensorData() {
  SensorData data;
  
  // Read all sensors
  data.distanceFromSensor = readUltrasonic();
  data.fillLevel = calculateFillLevel(data.distanceFromSensor);
  data.gasLevel = readGasSensor();
  data.rfidTag = readRFID();
  data.gpsValid = readGPS(data.latitude, data.longitude);
  data.timestamp = millis();
  data.binStatus = determineBinStatus(data.fillLevel, data.gasLevel);
  
  return data;
}

void sendDataToFirebase(SensorData data) {
  if (!Firebase.ready() || !signupOK) {
    Serial.println("Firebase not ready");
    return;
  }
  
  // Create JSON object for current data
  FirebaseJson json;
  json.set("binID", binID);
  json.set("fillLevel", data.fillLevel);
  json.set("distanceFromSensor", data.distanceFromSensor);
  json.set("gasLevel", data.gasLevel);
  json.set("rfidTag", data.rfidTag);
  json.set("timestamp", data.timestamp);
  json.set("binStatus", data.binStatus);
  
  if (data.gpsValid) {
    json.set("latitude", data.latitude);
    json.set("longitude", data.longitude);
  } else {
    json.set("latitude", 0);
    json.set("longitude", 0);
    json.set("gpsStatus", "GPS Not Ready");
  }
  
  json.set("lastUpdated", "timestamp");

  // Send to Firebase Realtime Database - Current Data
  String currentPath = "/smartWasteBins/" + binID + "/currentData";
  
  if (Firebase.setJSON(fbdo, currentPath.c_str(), json)) {
    Serial.println("✅ Current data sent to Firebase successfully");
    
    // Also add to historical data (only if RFID detected or significant change)
    if (data.rfidTag != "" || data.binStatus == "FULL" || data.binStatus == "GAS_ALERT") {
      String historyPath = "/smartWasteBins/" + binID + "/history/" + String(data.timestamp);
      Firebase.setJSON(fbdo, historyPath.c_str(), json);
      Serial.println("📊 Data logged to history");
    }
    
  } else {
    Serial.printf("❌ Failed to send data: %s\n", fbdo.errorReason().c_str());
  }

  // Send alert if bin is full or gas detected
  if (data.binStatus == "FULL" || data.binStatus == "GAS_ALERT") {
    sendAlert(data);
  }
}

void sendAlert(SensorData data) {
  FirebaseJson alertJson;
  alertJson.set("binID", binID);
  alertJson.set("alertType", data.binStatus);
  alertJson.set("message", getAlertMessage(data.binStatus));
  alertJson.set("timestamp", data.timestamp);
  alertJson.set("priority", data.binStatus == "GAS_ALERT" ? "HIGH" : "MEDIUM");
  alertJson.set("resolved", false);

  String alertPath = "/alerts/" + String(data.timestamp);
  if (Firebase.setJSON(fbdo, alertPath.c_str(), alertJson)) {
    Serial.println("🚨 Alert sent to Firebase");
  }
}

String getAlertMessage(String status) {
  if (status == "FULL") {
    return "Waste bin " + binID + " is full and needs collection";
  } else if (status == "GAS_ALERT") {
    return "Gas detected in waste bin " + binID + " - immediate attention required";
  }
  return "Alert for bin " + binID;
}

void printSensorData(SensorData data) {
  Serial.println("=== Sensor Reading ===");
  Serial.println("Bin ID: " + binID);
  Serial.println("Distance: " + String(data.distanceFromSensor) + " cm");
  Serial.println("Fill Level: " + String(data.fillLevel) + "%");
  Serial.println("Gas Level: " + String(data.gasLevel));
  Serial.println("RFID: " + (data.rfidTag != "" ? data.rfidTag : "No card detected"));
  if (data.gpsValid) {
    Serial.println("GPS: " + String(data.latitude, 6) + ", " + String(data.longitude, 6));
  } else {
    Serial.println("GPS: Not available");
  }
  Serial.println("Status: " + data.binStatus);
  Serial.println("Timestamp: " + String(data.timestamp));
  Serial.println("=====================");
}

void tokenStatusCallback(TokenInfo info) {
  Serial.printf("Token status: %s\n", info.status == token_status_ready ? "ready" : "not ready");
}

// --------- Main Loop ---------
void loop() {
  // Check for RFID trigger or periodic data sending
  String uid = readRFID();
  bool shouldSendData = false;
  
  // Send data if RFID detected or if it's time for periodic update
  if (uid != "") {
    Serial.println("RFID detected: " + uid);
    shouldSendData = true;
  } else if (millis() - lastDataSend > DATA_SEND_INTERVAL) {
    shouldSendData = true;
  }
  
  if (shouldSendData && Firebase.ready() && signupOK) {
    // Collect all sensor data
    SensorData data = collectSensorData();
    
    // Print data to serial monitor
    printSensorData(data);
    
    // Send data to Firebase
    sendDataToFirebase(data);
    
    lastDataSend = millis();
    
    // Brief delay before next reading
    delay(2000);
  }
  
  // Small delay to prevent overwhelming the system
  delay(100);
}

/*-------------Loop Section-------------*/
