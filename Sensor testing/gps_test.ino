#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// Use UART1 on ESP32
HardwareSerial GPS_Serial(1);
TinyGPSPlus gps;

// Set your GPS pins
#define GPS_RX 16  // GPS TX connects here
#define GPS_TX 17  // GPS RX connects here

void setup() {
  Serial.begin(115200);  // Serial monitor
  GPS_Serial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

  Serial.println("GPS module is active. Waiting for location fix...");
}

void loop() {
  while (GPS_Serial.available() > 0) {
    gps.encode(GPS_Serial.read());
  }

  if (gps.location.isUpdated()) {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);

    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);

    Serial.print("Altitude (m): ");
    Serial.println(gps.altitude.meters());

    Serial.print("Date (DD/MM/YYYY): ");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.println(gps.date.year());

    Serial.print("Time (UTC): ");
    Serial.print(gps.time.hour());
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.println(gps.time.second());

    Serial.println("----------------------------");
  }

  delay(1000);
}
