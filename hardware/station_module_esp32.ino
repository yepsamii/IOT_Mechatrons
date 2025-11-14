/*
 * ========================================
 * Smart Rickshaw - Station Module (ESP32)
 * ========================================
 * 
 * Description: Station hardware that detects users, verifies privilege,
 *              and creates ride requests in Firebase
 * 
 * Components:
 * - ESP32 DevKit
 * - HC-SR04 Ultrasonic Sensor (User Detection)
 * - LDR (Light Dependent Resistor) - Laser Detection
 * - Push Buttons (Destination Selection)
 * - LEDs (Status Indicators)
 * - Buzzer (Confirmation Feedback)
 * 
 * Competition: IOTrix - Televerse 1.0
 * ========================================
 */

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ArduinoJson.h>

// ========================================
// CONFIGURATION - UPDATE THESE VALUES
// ========================================

// WiFi Credentials
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Firebase Configuration
#define FIREBASE_HOST "your-project-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "your-database-secret-or-leave-empty"

// Station Configuration
#define STATION_ID "station_1"
#define STATION_NAME "Station A - University Gate"

// ========================================
// PIN DEFINITIONS
// ========================================

// Ultrasonic Sensor (HC-SR04)
#define TRIG_PIN 5
#define ECHO_PIN 18

// LDR (Laser Detection)
#define LDR_PIN 34  // Analog input

// Buttons (Destination Selection)
#define BTN_STATION_2 19
#define BTN_STATION_3 21

// LEDs (Status Indicators)
#define LED_READY 25     // Green - System ready
#define LED_DETECTED 26  // Blue - User detected
#define LED_VERIFIED 27  // Yellow - Privilege verified

// Buzzer
#define BUZZER_PIN 32

// ========================================
// CONSTANTS
// ========================================

#define DETECTION_DISTANCE 50  // cm - ultrasonic trigger distance
#define LDR_THRESHOLD 500      // Laser detection threshold
#define DEBOUNCE_DELAY 200     // ms - button debounce

// ========================================
// GLOBAL VARIABLES
// ========================================

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

bool userDetected = false;
bool privilegeVerified = false;
String selectedDestination = "";

unsigned long lastButtonPress = 0;

// ========================================
// SETUP
// ========================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n========================================");
  Serial.println("Smart Rickshaw - Station Module");
  Serial.println("========================================\n");
  
  // Initialize pins
  setupPins();
  
  // Connect to WiFi
  connectWiFi();
  
  // Initialize Firebase
  setupFirebase();
  
  // System ready
  digitalWrite(LED_READY, HIGH);
  playTone(1000, 200);  // Ready beep
  Serial.println("✅ Station module ready!");
  Serial.println("Waiting for users...\n");
}

// ========================================
// MAIN LOOP
// ========================================

void loop() {
  // Step 1: Detect user presence
  if (!userDetected) {
    float distance = getDistance();
    
    if (distance > 0 && distance < DETECTION_DISTANCE) {
      userDetected = true;
      digitalWrite(LED_DETECTED, HIGH);
      playTone(800, 100);
      Serial.println("👤 User detected!");
      Serial.println("→ Please verify privilege with laser...");
    }
  }
  
  // Step 2: Verify privilege (laser)
  else if (userDetected && !privilegeVerified) {
    int ldrValue = analogRead(LDR_PIN);
    
    if (ldrValue > LDR_THRESHOLD) {
      privilegeVerified = true;
      digitalWrite(LED_VERIFIED, HIGH);
      playTone(1200, 200);
      Serial.println("✅ Privilege verified!");
      Serial.println("→ Please select destination...");
    }
  }
  
  // Step 3: Select destination
  else if (userDetected && privilegeVerified && selectedDestination == "") {
    checkDestinationButtons();
  }
  
  // Step 4: Create ride request
  else if (userDetected && privilegeVerified && selectedDestination != "") {
    createRideRequest();
    resetStation();
  }
  
  delay(50);  // Small delay to prevent overwhelming the loop
}

// ========================================
// PIN SETUP
// ========================================

void setupPins() {
  // Ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // LDR (analog input - no pinMode needed)
  
  // Buttons
  pinMode(BTN_STATION_2, INPUT_PULLUP);
  pinMode(BTN_STATION_3, INPUT_PULLUP);
  
  // LEDs
  pinMode(LED_READY, OUTPUT);
  pinMode(LED_DETECTED, OUTPUT);
  pinMode(LED_VERIFIED, OUTPUT);
  
  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize all LEDs off
  digitalWrite(LED_READY, LOW);
  digitalWrite(LED_DETECTED, LOW);
  digitalWrite(LED_VERIFIED, LOW);
}

// ========================================
// WIFI CONNECTION
// ========================================

void connectWiFi() {
  Serial.print("🌐 Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi connected!");
    Serial.print("📡 IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ WiFi connection failed!");
    playTone(400, 1000);  // Error beep
  }
}

// ========================================
// FIREBASE SETUP
// ========================================

void setupFirebase() {
  Serial.println("\n🔥 Initializing Firebase...");
  
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  
  // For anonymous authentication (recommended)
  // Firebase.signUp(&config, &auth, "", "");
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  if (Firebase.ready()) {
    Serial.println("✅ Firebase connected!");
  } else {
    Serial.println("❌ Firebase connection failed!");
    playTone(400, 1000);
  }
}

// ========================================
// ULTRASONIC DISTANCE MEASUREMENT
// ========================================

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30ms timeout
  
  if (duration == 0) {
    return -1;  // No echo received
  }
  
  float distance = duration * 0.034 / 2;  // Convert to cm
  return distance;
}

// ========================================
// DESTINATION BUTTON CHECK
// ========================================

void checkDestinationButtons() {
  unsigned long currentTime = millis();
  
  // Debounce check
  if (currentTime - lastButtonPress < DEBOUNCE_DELAY) {
    return;
  }
  
  // Check Station 2 button
  if (digitalRead(BTN_STATION_2) == LOW) {
    selectedDestination = "station_2";
    lastButtonPress = currentTime;
    playTone(1000, 100);
    Serial.println("📍 Destination selected: Station B");
  }
  
  // Check Station 3 button
  else if (digitalRead(BTN_STATION_3) == LOW) {
    selectedDestination = "station_3";
    lastButtonPress = currentTime;
    playTone(1000, 100);
    Serial.println("📍 Destination selected: Station C");
  }
}

// ========================================
// CREATE RIDE REQUEST IN FIREBASE
// ========================================

void createRideRequest() {
  Serial.println("\n📤 Creating ride request...");
  
  // Get fare information from fare matrix
  String fareKey = String(STATION_ID) + "_to_" + selectedDestination;
  String farePath = "/fare_matrix/" + fareKey;
  
  float distance = 0;
  int fare = 0;
  int points = 0;
  
  // Fetch fare data
  if (Firebase.getFloat(firebaseData, farePath + "/distance_km")) {
    distance = firebaseData.floatData();
  }
  if (Firebase.getInt(firebaseData, farePath + "/base_fare")) {
    fare = firebaseData.intData();
  }
  if (Firebase.getInt(firebaseData, farePath + "/points")) {
    points = firebaseData.intData();
  }
  
  // Generate unique request ID
  String requestId = "req_" + String(millis());
  String requestPath = "/ride_requests/" + requestId;
  
  // Create JSON object
  FirebaseJson json;
  json.set("id", requestId);
  json.set("user_id", "user_1");  // In real system, get from user database
  json.set("pickup_station", STATION_ID);
  json.set("dropoff_station", selectedDestination);
  json.set("distance_km", distance);
  json.set("estimated_fare", fare);
  json.set("estimated_points", points);
  json.set("privilege_verified", true);
  json.set("status", "pending");
  json.set("timestamp", getTimestamp());
  
  // Send to Firebase
  if (Firebase.setJSON(firebaseData, requestPath, json)) {
    Serial.println("✅ Ride request created successfully!");
    Serial.println("📊 Details:");
    Serial.println("   From: " + String(STATION_NAME));
    Serial.println("   To: " + selectedDestination);
    Serial.println("   Distance: " + String(distance) + " km");
    Serial.println("   Fare: ৳" + String(fare));
    Serial.println("   Points: +" + String(points));
    
    // Success feedback
    playSuccessSound();
    flashLEDs();
    
  } else {
    Serial.println("❌ Failed to create request!");
    Serial.println("Error: " + firebaseData.errorReason());
    playTone(400, 500);  // Error beep
  }
}

// ========================================
// UTILITY FUNCTIONS
// ========================================

String getTimestamp() {
  // In real implementation, use NTP time
  // For now, return current millis
  return String(millis());
}

void resetStation() {
  Serial.println("\n🔄 Resetting station...\n");
  delay(3000);  // Wait 3 seconds before accepting next user
  
  userDetected = false;
  privilegeVerified = false;
  selectedDestination = "";
  
  digitalWrite(LED_DETECTED, LOW);
  digitalWrite(LED_VERIFIED, LOW);
  
  Serial.println("Waiting for next user...\n");
}

void playTone(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
  delay(duration);
  noTone(BUZZER_PIN);
}

void playSuccessSound() {
  playTone(1000, 100);
  delay(50);
  playTone(1200, 100);
  delay(50);
  playTone(1400, 200);
}

void flashLEDs() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_READY, HIGH);
    digitalWrite(LED_DETECTED, HIGH);
    digitalWrite(LED_VERIFIED, HIGH);
    delay(200);
    digitalWrite(LED_READY, LOW);
    digitalWrite(LED_DETECTED, LOW);
    digitalWrite(LED_VERIFIED, LOW);
    delay(200);
  }
  digitalWrite(LED_READY, HIGH);  // Ready for next user
}

// ========================================
// DEBUG FUNCTIONS
// ========================================

void printStatus() {
  Serial.println("\n=== Station Status ===");
  Serial.println("User Detected: " + String(userDetected ? "YES" : "NO"));
  Serial.println("Privilege Verified: " + String(privilegeVerified ? "YES" : "NO"));
  Serial.println("Destination: " + (selectedDestination != "" ? selectedDestination : "NONE"));
  Serial.println("LDR Value: " + String(analogRead(LDR_PIN)));
  Serial.println("Distance: " + String(getDistance()) + " cm");
  Serial.println("=====================\n");
}

/*
 * ========================================
 * CIRCUIT CONNECTIONS
 * ========================================
 * 
 * HC-SR04 Ultrasonic:
 *   VCC → 5V
 *   GND → GND
 *   TRIG → GPIO 5
 *   ECHO → GPIO 18
 * 
 * LDR (Laser Sensor):
 *   One leg → 3.3V
 *   Other leg → GPIO 34 (ADC) + 10kΩ resistor to GND
 * 
 * Buttons:
 *   Station 2 Button → GPIO 19 + GND (internal pullup used)
 *   Station 3 Button → GPIO 21 + GND (internal pullup used)
 * 
 * LEDs (with 220Ω resistors):
 *   Ready LED → GPIO 25
 *   Detected LED → GPIO 26
 *   Verified LED → GPIO 27
 * 
 * Buzzer:
 *   + → GPIO 32
 *   - → GND
 * 
 * ========================================
 * REQUIRED LIBRARIES (Arduino IDE)
 * ========================================
 * 
 * 1. Firebase ESP32 Client by Mobizt
 *    Sketch → Include Library → Manage Libraries
 *    Search: "Firebase ESP32"
 *    Install: "Firebase Arduino Client Library for ESP32"
 * 
 * 2. ArduinoJson by Benoit Blanchon
 *    Search: "ArduinoJson"
 *    Install version 6.x
 * 
 * ========================================
 * TESTING
 * ========================================
 * 
 * 1. Upload code to ESP32
 * 2. Open Serial Monitor (115200 baud)
 * 3. Verify WiFi and Firebase connection
 * 4. Approach ultrasonic sensor (< 50cm)
 * 5. Point laser at LDR
 * 6. Press destination button
 * 7. Check Firebase for new ride request
 * 8. Verify web dashboard shows request
 * 
 * ========================================
 */

