# Smart Waste Monitoring & Optimization System
### *A Data-Driven Approach to Waste Management Using IoT and AI*

---

### 📌 Project Overview
This project presents the design and implementation of a **Smart Waste Monitoring System** that leverages **IoT sensors**, **AI-based analytics**, and **real-time cloud connectivity** to monitor fill levels, environmental parameters, and waste classification of bins.

Using an **ESP32-based microcontroller**, the system captures and transmits multi-sensor data—including ultrasonic distance, gas concentration (e.g., CO₂), weight, GPS coordinates, and optional image classification—to **Firebase Realtime Database** for centralized storage, dashboard visualization, and route optimization.

This comprehensive solution addresses inefficiencies in traditional waste management through:

- ✅ Automated detection  
- ✅ Real-time reporting  
- ✅ Predictive analytics  

---

### 🎯 Project Objectives
- Accurately monitor bin status in real-time using multi-sensor input  
- Alert authorities on gas levels, overflow risk, and fill percentages  
- Transmit data securely to Firebase and visualize on a web dashboard  
- Enable route optimization through AI prediction of bin fill levels  
- Empower city-scale deployment with modular hardware + cloud setup  

---

### ⚙️ System Architecture & Setup

#### 🔧 Hardware
- **Microcontroller**:  
  - ESP32 Dev Board
    
- **Sensors**:  
  - HC-SR04 (Ultrasonic Distance Sensor)  
  - MQ-135 (Air Quality / Gas Sensor)    
  - NEO-6M GPS Module  
  - RC522 RFID Module  
---

### 🔌 Wiring & Testing
- Each sensor is connected to a dedicated GPIO or analog pin  
- Power is provided via USB or LiPo battery with voltage divider  
- Unit tests are conducted using Serial Monitor  
- GPIO and ADC pins are mapped per sensor role  

---

### 🧠 Firmware & Testing
- Developed using **Arduino IDE** with libraries:  
  `Firebase_ESP_Client`, `TinyGPS++`, `HX711`, `MFRC522`, etc.  
- Data is read every 10 seconds and uploaded via Wi-Fi to Firebase  
- Tested under various environmental conditions  

---

### 🔥 Firebase Integration & Data Flow
- ESP32 initializes Wi-Fi and Firebase credentials on boot  
- Each reading creates a new timestamped entry under `bin_data/` in Firebase Realtime Database  
- **Collected Data Includes**:
  - Distance (cm)  
  - Gas concentration (analog voltage)  
  - Weight (kg)  
  - GPS coordinates  
  - Temperature and humidity  
  - RFID UID  
  - Image classification result (if enabled)  
- All values are sent as float or string entries for real-time tracking
---

### 🤖 Machine Learning Model & Route Optimization  

#### 🧮 Predictive Bin Fill Classification  
A **machine learning model** was developed to predict how long it takes for a bin to become full, allowing waste services to anticipate pickups.  

- **Dataset**: [Netvox R718x Bin Sensor dataset](https://melbournetestbed.opendatasoft.com/explore/dataset/netvox-r718x-bin-sensor/table/?disjunctive.dev_id) (≈172k datapoints after preprocessing).  
- **Preprocessing**:  
  - Removed nulls/outliers  
  - Extracted time-based features (`hour`, `weekday`, `season`)  
  - Engineered a `time_to_full` column → bucketed into 4 fill-time classes  
- **Model**:  
  - Tested multiple algorithms → **Random Forest Classifier** performed best  
  - Tuned hyperparameters and exported as `bin_urgency_rf_pipeline.pkl`  
- **Integration**:  
  - Connected to **Firebase** using `firebase_admin`  
  - Live IoT data is passed to the model  
  - Predictions (bin urgency) are written back to Firebase under `/prediction/urgency`  

#### 🚚 Route Optimization  
- Combines **predicted urgency** + **GPS coordinates** to prioritize bins  
- Optimized routes generated via:  
  - Graph algorithms (e.g., Dijkstra’s, A*)  
  - Or **Google Maps Distance Matrix API**  
- Ensures only high-priority bins are serviced  

**✨ Benefits:**  
- Proactive collection before bins overflow  
- Lower fuel consumption & operational costs  
- Smarter and scalable waste management system

---

### 📊 Dashboard Visualization
- Built using **HTML/CSS/JavaScript** with **Firebase JS SDK**  
- Displays:
  - ✅ Live bin fill status (bar graph or percentage)  
  - ✅ Gas alert and CO₂ trend chart  
  - ✅ Last known location on Google Maps  
  - ✅ Recently scanned RFID cards  
  - ✅ Battery voltage with warning alerts  
- Users can view, analyze, and download bin health reports  
