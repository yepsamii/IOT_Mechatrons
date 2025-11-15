/*
 * ESP32 GPS Tracker with Firebase Integration
 * Displays rickshaw location and customer pickup location
 * 
 * Components:
 * - GPS Module (NEO-6M): RX-16, TX-17 (optional - currently using hardcoded location)
 * - OLED Display (SSD1306): I2C (SDA-21, SCL-22)
 * 
 * Libraries Required:
 * - TinyGPSPlus
 * - Adafruit_SSD1306
 * - Adafruit_GFX
 * - Firebase ESP Client
 */

 #include <TinyGPSPlus.h>
 #include <WiFi.h>
 #include <Wire.h>
 #include <Adafruit_SSD1306.h>
 #include <Firebase_ESP_Client.h>
 #include "addons/TokenHelper.h"
 #include "addons/RTDBHelper.h"
 
 // Firebase Configuration
 #define API_KEY "AIzaSyCyPJA1r5HQKGPKT_xRPio1Yzafgu1pxAI" 
 #define DATABASE_URL "https://rickshaw-ride-c5683-default-rtdb.asia-southeast1.firebasedatabase.app"
 
 // WiFi Credentials
 const char* ssid = "realme 8";
 const char* password = "11223344";
 
 // OLED Display Settings
 #define SCREEN_WIDTH 128
 #define SCREEN_HEIGHT 64
 #define OLED_RESET -1
 Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
 // GPS Settings
 #define GPS_RX 16  // Connect to GPS TX
 #define GPS_TX 17  // Connect to GPS RX
 #define GPS_BAUD 9600
 HardwareSerial gpsSerial(2);
 TinyGPSPlus gps;
 
// Firebase objects (using multiple objects to prevent buffer corruption)
FirebaseData fbdo;
FirebaseData fbdoBackup;  // Backup connection for parallel requests
FirebaseAuth auth;
FirebaseConfig config;
 
// SSL Buffer size (increase to handle Firebase SSL handshake)
#define SSL_BUFFER_SIZE 8192
#define MAX_RESPONSE_SIZE 8192

// Rickshaw puller's current location (hardcoded for now)
 double currentLat = 22.4580;
 double currentLon = 91.9714;
 String currentLocationName = "CUET Campus";
 
 // Customer's pickup location (destination for rickshaw)
 double customerLat = 0.0;
 double customerLon = 0.0;
 String customerLocationName = "";
 String pickupBlock = "";
 String dropoffBlock = "";
 
// Ride details
double distance = 0.0;
int estimatedFare = 0;
int estimatedPoints = 0;

// Firebase tracking
String activeRequestId = "";
String assignedRickshawId = "rickshaw_2"; // Unique ID for this rickshaw (must match database)
unsigned long lastFirebaseCheck = 0;
const unsigned long FIREBASE_CHECK_INTERVAL = 2000; // Check every 2 seconds for faster response

// Status notification tracking
String lastStatus = "";
bool showNotification = false;
String notificationMessage = "";
unsigned long notificationStartTime = 0;
const unsigned long NOTIFICATION_DURATION = 5000; // Show notification for 5 seconds
 
 // Function to calculate distance between two GPS coordinates (Haversine formula)
 double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
   double R = 6371000; // Earth radius in meters
   double phi1 = lat1 * PI / 180.0;
   double phi2 = lat2 * PI / 180.0;
   double deltaPhi = (lat2 - lat1) * PI / 180.0;
   double deltaLambda = (lon2 - lon1) * PI / 180.0;
 
   double a = sin(deltaPhi / 2) * sin(deltaPhi / 2) +
              cos(phi1) * cos(phi2) *
              sin(deltaLambda / 2) * sin(deltaLambda / 2);
   double c = 2 * atan2(sqrt(a), sqrt(1 - a));
 
   return R * c; // Distance in meters
 }
 
// Convert block name to readable location name
String blockToLocationName(String block) {
  if (block == "cuet_campus") return "CUET Campus";
  else if (block == "pahartali") return "Pahartali";
  else if (block == "noapara") return "Noapara";
  else if (block == "raojan") return "Raojan";
  else return block;
}

// Helper function to strip HTTP headers if they leak through
String stripHttpHeaders(String response) {
  // If response starts with HTTP headers, find the JSON body
  if (response.startsWith("HTTP/") || response.indexOf("HTTP/") == 0) {
    // Look for double newline that separates headers from body
    int bodyStart = response.indexOf("\r\n\r\n");
    if (bodyStart >= 0) {
      return response.substring(bodyStart + 4);
    }
    bodyStart = response.indexOf("\n\n");
    if (bodyStart >= 0) {
      return response.substring(bodyStart + 2);
    }
    // If we can't find body separator, the response is corrupted
    return "";
  }
  return response;
}

// Show notification on OLED
void showNotificationScreen(String title, String message, String details = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Title bar with inverse colors
  display.fillRect(0, 0, SCREEN_WIDTH, 12, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(2, 2);
  display.println(title);
  
  // Message content
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 18);
  display.setTextSize(2);
  display.println(message);
  
  // Additional details
  if (details != "") {
    display.setTextSize(1);
    display.println("");
    display.println(details);
  }
  
  display.display();
}
 
// Function to update OLED display
void updateDisplay() {
  // Check if we should show a notification
  if (showNotification) {
    unsigned long currentTime = millis();
    if (currentTime - notificationStartTime < NOTIFICATION_DURATION) {
      // Still showing notification
      return;
    } else {
      // Notification time expired
      showNotification = false;
    }
  }
  
  // Page 1: Rickshaw's Current Location
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println("MY LOCATION:");
  display.setTextSize(2);
  display.println(currentLocationName);
  
  display.setTextSize(1);
  display.println("");
  display.print("Lat: ");
  display.println(currentLat, 4);
  display.print("Lon: ");
  display.println(currentLon, 4);
  
  display.display();
  delay(1000); // Reduced delay for faster updates
   
   // Page 2: Customer's Pickup Location
   display.clearDisplay();
   display.setCursor(0, 0);
   display.setTextSize(1);
   
   if (customerLat != 0.0 && customerLon != 0.0) {
     display.println("CUSTOMER PICKUP:");
     display.setTextSize(2);
     display.println(customerLocationName);
     
     display.setTextSize(1);
     display.println("");
     display.print("Lat: ");
     display.println(customerLat, 4);
     display.print("Lon: ");
     display.println(customerLon, 4);
   } else {
     display.setTextSize(2);
     display.println("No Active");
     display.println("Ride");
     display.setTextSize(1);
     display.println("");
     display.println("Waiting for ride");
     display.println("requests...");
   }
   
  display.display();
  delay(1000); // Reduced delay for faster updates
  
  // Page 3: Distance and Fare (only if there's an active ride)
  if (customerLat != 0.0 && customerLon != 0.0) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    
    display.println("RIDE DETAILS:");
    display.println("");
    
    display.print("Distance: ");
    if (distance < 1000) {
      display.print((int)distance);
      display.println(" m");
    } else {
      display.print(distance / 1000, 2);
      display.println(" km");
    }
    
    display.println("");
    display.print("Fare: ");
    display.print(estimatedFare);
    display.println(" Tk");
    
    display.println("");
    display.print("Points: ");
    display.println(estimatedPoints);
    
    display.display();
    delay(1000); // Reduced delay for faster updates
  }
}
 
// Check Firebase for accepted ride requests assigned to this rickshaw
void checkFirebaseForRides() {
  unsigned long currentTime = millis();
   
  if (currentTime - lastFirebaseCheck >= FIREBASE_CHECK_INTERVAL) {
    lastFirebaseCheck = currentTime;
    
    // Check WiFi connection first
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("\n[WiFi not connected! Skipping Firebase check...]");
      return;
    }
    
    // Check if Firebase is ready before making requests
    if (!Firebase.ready()) {
      Serial.println("\n[Firebase not ready yet, skipping check...]");
      return;
    }
    
    Serial.println("\n[Checking Firebase for rides...]");
    Serial.print("  WiFi Signal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    
    bool foundActiveRide = false;
    
    // Clear any previous data to prevent buffer corruption
    fbdo.clear();
    fbdoBackup.clear();
    delay(200);
    
    // Reinitialize the connection with fresh buffers
    fbdoBackup.setBSSLBufferSize(SSL_BUFFER_SIZE, SSL_BUFFER_SIZE);
    fbdoBackup.setResponseSize(MAX_RESPONSE_SIZE);
    
    // Query Firebase for rides assigned to THIS rickshaw only
    // Use orderBy and equalTo to filter on server-side
    String queryPath = "/ride_requests";
    
    // Build a Firebase query to get only rides assigned to this rickshaw
    FirebaseJson query;
    QueryFilter queryFilter;
    queryFilter.orderBy("assigned_rickshaw");
    queryFilter.equalTo(assignedRickshawId);
    
    if (Firebase.RTDB.getJSON(&fbdoBackup, queryPath.c_str(), &queryFilter)) {
      String jsonStr = fbdoBackup.stringData();
      
      // Try to strip HTTP headers if they leaked through
      jsonStr = stripHttpHeaders(jsonStr);
      
      // Validate that we got actual JSON
      if (jsonStr == "" || jsonStr.startsWith("HTTP/") || jsonStr.startsWith("<!DOCTYPE") || jsonStr.length() < 2) {
        Serial.println("  -> ERROR: Received malformed response (HTTP headers in body)");
        Serial.print("  -> Raw Response (first 200 chars): ");
        Serial.println(fbdoBackup.stringData().substring(0, 200));
        
        // Force reconnection on next cycle
        fbdoBackup.clear();
        fbdo.clear();
        delay(1000);
        
        // Reset Firebase connection
        Firebase.reconnectWiFi(true);
        return;
      }
      
      Serial.print("  Response received: ");
      Serial.print(jsonStr.length());
      Serial.println(" bytes");
      Serial.print("  First 100 chars: ");
      Serial.println(jsonStr.substring(0, min(100, (int)jsonStr.length())));
      
      // Check if response is empty or null
      if (jsonStr == "" || jsonStr == "null" || jsonStr.length() < 5 || jsonStr == "{}") {
        Serial.println("  -> No ride requests in database.");
        
        // Clear data if no active ride found
        if (activeRequestId != "") {
          Serial.println("  -> Clearing previous ride data...");
          activeRequestId = "";
          lastStatus = "";
          customerLat = 0.0;
          customerLon = 0.0;
          customerLocationName = "";
          pickupBlock = "";
          dropoffBlock = "";
          estimatedFare = 0;
          estimatedPoints = 0;
        }
        return;
      }
      
      Serial.println("  -> Parsing ride requests...");
      
      FirebaseJson json;
      json.setJsonData(jsonStr);
      
      size_t len = json.iteratorBegin();
      String key, value;
      int type = 0;
      
      if (len == 0) {
        Serial.println("  -> No ride request keys found (len=0).");
        json.iteratorEnd();
        return;
      }
      
      Serial.print("  -> Found ");
      Serial.print(len);
      Serial.println(" ride(s) assigned to this rickshaw.");
      
      // Iterate through ride requests (filtered by assigned_rickshaw)
      for (size_t i = 0; i < len; i++) {
        json.iteratorGet(i, type, key, value);
        
        // Skip non-object entries (like metadata fields)
        if (type != FirebaseJson::JSON_OBJECT) {
          continue;
        }
        
        Serial.print("\n  [");
        Serial.print(i + 1);
        Serial.print("/");
        Serial.print(len);
        Serial.print("] Processing ride: ");
        Serial.println(key);
        
        // Parse the ride object from the value
        FirebaseJson rideJson;
        rideJson.setJsonData(value);
        
        // Get status from the ride object first (most important field)
        FirebaseJsonData statusData;
        rideJson.get(statusData, "status");
        
        if (statusData.success) {
          String status = statusData.stringValue;
          Serial.print("      Status: ");
          Serial.println(status);
          
          // Skip if not in a relevant status
          if (status != "accepted" && status != "picked_up" && status != "rejected" && status != "completed") {
            Serial.println("      -> Not in active status, skipping.");
            continue;
          }
          
          // Double-check assignment (should already be filtered, but just in case)
          FirebaseJsonData assignedData;
          rideJson.get(assignedData, "assigned_rickshaw");
          
          if (assignedData.success) {
            String assignedTo = assignedData.stringValue;
            
            if (assignedTo != assignedRickshawId) {
              Serial.println("      -> Not for this rickshaw, skipping.");
              continue;
            }
          } else {
            Serial.println("      -> No assignment info, skipping.");
            continue;
          }
          
          Serial.println("      -> ACTIVE RIDE for this rickshaw!");
          
          // Detect status changes for this request
          bool isStatusChange = false;
          if (activeRequestId == key && lastStatus != "" && lastStatus != status) {
            isStatusChange = true;
          }
          
          // Look for accepted rides
          if (status == "accepted") {
            foundActiveRide = true;
            
            // Check if this is a newly accepted ride
            if (activeRequestId != key) {
              isStatusChange = true;
            }
            
            activeRequestId = key;
            lastStatus = status;
            
            // Get pickup block (where customer is waiting)
            FirebaseJsonData pickupData;
            rideJson.get(pickupData, "pickup_block");
            if (pickupData.success) {
              pickupBlock = pickupData.stringValue;
              customerLocationName = blockToLocationName(pickupBlock);
            }
            
            // Get dropoff block
            FirebaseJsonData dropoffData;
            rideJson.get(dropoffData, "dropoff_block");
            if (dropoffData.success) {
              dropoffBlock = dropoffData.stringValue;
            }
            
            // Get fare and points
            FirebaseJsonData fareData;
            rideJson.get(fareData, "estimated_fare");
            if (fareData.success) {
              estimatedFare = fareData.intValue;
            }
            
            FirebaseJsonData pointsData;
            rideJson.get(pointsData, "estimated_points");
            if (pointsData.success) {
              estimatedPoints = pointsData.intValue;
            }
            
            // Get customer's coordinates from /location_blocks (need separate call)
            fbdo.clear();
            delay(50);
            String coordPath = "/location_blocks/" + pickupBlock + "/coordinates";
            if (Firebase.RTDB.getJSON(&fbdo, coordPath.c_str())) {
              String coordStr = fbdo.stringData();
              coordStr = stripHttpHeaders(coordStr);
              
              if (coordStr != "" && !coordStr.startsWith("HTTP/")) {
                FirebaseJson coordJson;
                coordJson.setJsonData(coordStr);
                FirebaseJsonData latData, lngData;
                
                coordJson.get(latData, "lat");
                coordJson.get(lngData, "lng");
                
                if (latData.success && lngData.success) {
                  customerLat = latData.doubleValue;
                  customerLon = lngData.doubleValue;
                }
              }
            }
            
            Serial.println("  ========================================");
            Serial.println("  ACTIVE RIDE FOUND!");
            Serial.println("  Request ID: " + key);
            Serial.println("  Customer Location: " + customerLocationName);
            Serial.print("  Coordinates: ");
            Serial.print(customerLat, 6);
            Serial.print(", ");
            Serial.println(customerLon, 6);
            Serial.println("  Dropoff: " + blockToLocationName(dropoffBlock));
            Serial.print("  Fare: ");
            Serial.print(estimatedFare);
            Serial.println(" Tk");
            Serial.print("  Points: ");
            Serial.println(estimatedPoints);
            Serial.println("  ========================================");
            
            // Show notification for newly accepted ride
            if (isStatusChange) {
              showNotification = true;
              notificationStartTime = millis();
              String details = "Pickup: " + customerLocationName + "\nDropoff: " + blockToLocationName(dropoffBlock);
              showNotificationScreen("RIDE ACCEPTED!", "New Ride", details);
              Serial.println(">>> NOTIFICATION: New ride accepted!");
            }
            
            break; // Only handle one ride at a time
          }
          // Handle rejected rides
          else if (status == "rejected") {
            if (activeRequestId == key) {
              Serial.println("  Ride REJECTED. Clearing data...");
              
              // Show rejection notification
              showNotification = true;
              notificationStartTime = millis();
              showNotificationScreen("RIDE REJECTED", "Rejected", "Looking for new rides...");
              Serial.println(">>> NOTIFICATION: Ride rejected!");
              
              activeRequestId = "";
              lastStatus = "";
              customerLat = 0.0;
              customerLon = 0.0;
              customerLocationName = "";
              pickupBlock = "";
              dropoffBlock = "";
              estimatedFare = 0;
              estimatedPoints = 0;
              foundActiveRide = false;
            }
          }
          // Clear data if ride is picked up or completed
          else if (status == "picked_up" || status == "completed") {
            if (activeRequestId == key) {
              Serial.println("  Ride " + status + ". Clearing data...");
              
              // Show status notification
              if (isStatusChange) {
                showNotification = true;
                notificationStartTime = millis();
                String statusMsg = (status == "picked_up") ? "Picked Up!" : "Completed!";
                showNotificationScreen("RIDE UPDATE", statusMsg, "");
                Serial.println(">>> NOTIFICATION: Ride " + status);
              }
              
              activeRequestId = "";
              lastStatus = "";
              customerLat = 0.0;
              customerLon = 0.0;
              customerLocationName = "";
              pickupBlock = "";
              dropoffBlock = "";
              estimatedFare = 0;
              estimatedPoints = 0;
              foundActiveRide = false;
            }
          }
        }
        
        delay(100); // Small delay between operations
      }
      
      json.iteratorEnd();
      
      // Clear data if no active ride found
      if (!foundActiveRide && activeRequestId != "") {
        Serial.println("No active rides. Clearing data...");
        activeRequestId = "";
        lastStatus = "";
        customerLat = 0.0;
        customerLon = 0.0;
        customerLocationName = "";
        pickupBlock = "";
        dropoffBlock = "";
        estimatedFare = 0;
        estimatedPoints = 0;
      }
      
    } else {
      String errorMsg = fbdoBackup.errorReason();
      int errorCode = fbdoBackup.httpCode();
      
      Serial.print("Firebase Query Failed - HTTP Code: ");
      Serial.print(errorCode);
      Serial.print(", Error: ");
      Serial.println(errorMsg.length() > 0 ? errorMsg : "Unknown error");
      
      // Handle SSL/connection errors
      if (errorMsg.indexOf("SSL") >= 0 || errorMsg.indexOf("buffer") >= 0 || 
          errorMsg.indexOf("connection") >= 0 || errorMsg.indexOf("closed") >= 0 ||
          errorCode == -1 || errorCode == 0 || errorCode == 401 || errorCode == 403) {
        Serial.println("Connection/SSL Error - Attempting to reconnect...");
        
        // Close any existing connections and clear buffer
        fbdo.clear();
        delay(500);
        
        // Reconnect WiFi
        if (WiFi.status() != WL_CONNECTED) {
          Serial.println("WiFi disconnected. Reconnecting...");
          WiFi.disconnect();
          delay(1000);
          WiFi.begin(ssid, password);
          
          int attempts = 0;
          while (WiFi.status() != WL_CONNECTED && attempts < 10) {
            delay(500);
            Serial.print(".");
            attempts++;
          }
          
          if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nWiFi reconnected!");
            Firebase.reconnectWiFi(true);
          } else {
            Serial.println("\nWiFi reconnection failed!");
            return;
          }
        }
        
        // Reset Firebase connection
        fbdo.setBSSLBufferSize(SSL_BUFFER_SIZE, SSL_BUFFER_SIZE);
        fbdo.setResponseSize(MAX_RESPONSE_SIZE);
        delay(1000);
        
      } else if (errorMsg.indexOf("path not exist") >= 0 || errorMsg.indexOf("not found") >= 0 || errorCode == 404) {
        Serial.println("No ride requests path found in database.");
      } else {
        Serial.println("Unexpected Firebase error - will retry next cycle.");
      }
    }
  }
}
 
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n========================================");
  Serial.println("ESP32 Rickshaw GPS Tracker");
  Serial.println("========================================");
  Serial.println("Rickshaw ID: " + assignedRickshawId);
  Serial.println("Database URL: " + String(DATABASE_URL));
  Serial.println("========================================\n");
   
   // Initialize GPS (commented out - no GPS sensor connected)
   // gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);
   // Serial.println("GPS Initialized");
   
   // Initialize OLED
   if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
     Serial.println(F("SSD1306 allocation failed"));
     for (;;);
   }
   
   display.clearDisplay();
   display.setTextSize(1);
   display.setTextColor(SSD1306_WHITE);
   display.setCursor(0, 0);
   display.println("Rickshaw Tracker");
   display.println("Initializing...");
   display.display();
   delay(2000);
   
   // Connect to WiFi
   WiFi.begin(ssid, password);
   Serial.print("Connecting to WiFi");
   
   display.clearDisplay();
   display.setCursor(0, 0);
   display.println("Connecting WiFi...");
   display.display();
   
   int attempts = 0;
   while (WiFi.status() != WL_CONNECTED && attempts < 20) {
     delay(500);
     Serial.print(".");
     attempts++;
   }
   
   if (WiFi.status() == WL_CONNECTED) {
     Serial.println("\nWiFi Connected!");
     Serial.print("IP Address: ");
     Serial.println(WiFi.localIP());
     
     display.clearDisplay();
     display.setCursor(0, 0);
     display.println("WiFi Connected!");
     display.print("IP: ");
     display.println(WiFi.localIP());
     display.display();
     delay(2000);
   } else {
     Serial.println("\nWiFi Failed!");
     display.clearDisplay();
     display.setCursor(0, 0);
     display.println("WiFi Failed!");
     display.display();
     delay(5000);
     ESP.restart();
   }
   
  // Configure Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  
  // Increase timeout for slower connections
  config.timeout.serverResponse = 10 * 1000; // 10 seconds
  config.timeout.socketConnection = 30 * 1000; // 30 seconds
  
  // Set SSL buffer size (larger to handle SSL handshake)
  fbdo.setBSSLBufferSize(SSL_BUFFER_SIZE, SSL_BUFFER_SIZE);
  fbdo.setResponseSize(MAX_RESPONSE_SIZE);
  
  // Also initialize backup object
  fbdoBackup.setBSSLBufferSize(SSL_BUFFER_SIZE, SSL_BUFFER_SIZE);
  fbdoBackup.setResponseSize(MAX_RESPONSE_SIZE);
  
  // Initialize Firebase
  Serial.println("Initializing Firebase...");
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  // Sign in anonymously
  Serial.println("Signing in anonymously...");
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connecting Firebase...");
  display.display();
  
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Anonymous sign-in successful");
    Serial.print("User UID: ");
    Serial.println(auth.token.uid.c_str());
    
    // Wait for token to be generated
    Serial.println("Waiting for auth token...");
    int tokenWait = 0;
    while ((auth.token.uid == "" || !Firebase.ready()) && tokenWait < 30) {
      delay(1000);
      Serial.print(".");
      tokenWait++;
    }
    
    if (Firebase.ready()) {
      Serial.println("\nFirebase is ready!");
    } else {
      Serial.println("\nWarning: Firebase may not be fully ready");
    }
    
  } else {
    Serial.println("Anonymous sign-in failed");
    Serial.println("Error: " + String(config.signer.signupError.message.c_str()));
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Firebase Auth");
    display.println("Failed!");
    display.println("");
    display.println("Check Firebase");
    display.println("settings");
    display.display();
    delay(5000);
  }
  
  Serial.println("Firebase initialized successfully");
   
   display.clearDisplay();
   display.setCursor(0, 0);
   display.setTextSize(1);
   display.println("System Ready!");
   display.println("");
   display.println("Waiting for");
   display.println("ride requests...");
   display.display();
   delay(2000);
   
   Serial.println("System ready. Listening for rides...\n");
 }
 
 void loop() {
   // Read GPS data (commented out - no GPS sensor connected)
   
   while (gpsSerial.available() > 0) {
     if (gps.encode(gpsSerial.read())) {
       if (gps.location.isValid()) {
         currentLat = gps.location.lat();
         currentLon = gps.location.lng();
         currentLocationName = "GPS Location";
         
         static unsigned long lastGPSPrint = 0;
         if (millis() - lastGPSPrint > 5000) {
           Serial.print("GPS: ");
           Serial.print(currentLat, 6);
           Serial.print(", ");
           Serial.println(currentLon, 6);
           lastGPSPrint = millis();
         }
       }
     }
   }
 
   
   // Check Firebase for ride assignments
   checkFirebaseForRides();
   
   // Calculate distance if both locations are valid
   if (currentLat != 0.0 && currentLon != 0.0 && customerLat != 0.0 && customerLon != 0.0) {
     distance = calculateDistance(currentLat, currentLon, customerLat, customerLon);
   } else {
     distance = 0.0;
   }
   
   // Update display
   updateDisplay();
   
   delay(100);
 }