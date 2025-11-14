# Firebase Setup Guide - Smart Rickshaw System

## 📋 Table of Contents

1. [Firebase Project Creation](#1-firebase-project-creation)
2. [Database Configuration](#2-database-configuration)
3. [Security Rules Setup](#3-security-rules-setup)
4. [Web App Configuration](#4-web-app-configuration)
5. [Initial Data Population](#5-initial-data-population)
6. [ESP32 Configuration](#6-esp32-configuration)

---

## 1. Firebase Project Creation

### Step 1.1: Create Firebase Account

1. Go to [Firebase Console](https://console.firebase.google.com/)
2. Sign in with your Google account
3. Click **"Add project"** or **"Create a project"**

### Step 1.2: Project Setup

1. **Project Name**: Enter `smart-rickshaw-system` (or your preferred name)
2. **Google Analytics**: Enable it (recommended for tracking)
3. Click **"Create project"**
4. Wait for project initialization (~30 seconds)

### Step 1.3: Add Web App

1. In Firebase Console, click the **Web icon** (</>)
2. **App nickname**: `rickshaw-dashboard`
3. ✅ Check **"Also set up Firebase Hosting"**
4. Click **"Register app"**
5. **IMPORTANT**: Copy the Firebase configuration object (you'll need this later)

```javascript
// Your config will look like this:
const firebaseConfig = {
  apiKey: "AIzaSyXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
  authDomain: "your-project.firebaseapp.com",
  databaseURL: "https://your-project-default-rtdb.firebaseio.com",
  projectId: "your-project-id",
  storageBucket: "your-project.appspot.com",
  messagingSenderId: "123456789012",
  appId: "1:123456789012:web:abcdef123456",
};
```

---

## 2. Database Configuration

### Step 2.1: Enable Realtime Database

1. In Firebase Console, go to **"Realtime Database"** from the left menu
2. Click **"Create Database"**
3. **Location**: Choose closest to Bangladesh (e.g., `asia-southeast1`)
4. **Security rules**: Start in **"Test mode"** (we'll secure it later)
5. Click **"Enable"**

### Step 2.2: Database Structure Setup

Your database URL will be: `https://your-project-default-rtdb.firebaseio.com/`

Copy this structure into your Firebase Realtime Database:

```json
{
  "stations": {
    "station_1": {
      "id": "station_1",
      "name": "Station A - University Gate",
      "location": {
        "lat": 22.4593,
        "lng": 91.9871
      },
      "status": "active",
      "created_at": "2025-11-14T10:00:00Z"
    },
    "station_2": {
      "id": "station_2",
      "name": "Station B - City Center",
      "location": {
        "lat": 22.4623,
        "lng": 91.9901
      },
      "status": "active",
      "created_at": "2025-11-14T10:00:00Z"
    },
    "station_3": {
      "id": "station_3",
      "name": "Station C - Market Area",
      "location": {
        "lat": 22.4653,
        "lng": 91.9931
      },
      "status": "active",
      "created_at": "2025-11-14T10:00:00Z"
    }
  },

  "users": {
    "user_1": {
      "id": "user_1",
      "name": "Ahmed Hassan",
      "privilege_type": "premium",
      "laser_id": "LASER_001",
      "total_rides": 0,
      "created_at": "2025-11-14T10:00:00Z"
    },
    "user_2": {
      "id": "user_2",
      "name": "Fatima Khan",
      "privilege_type": "standard",
      "laser_id": "LASER_002",
      "total_rides": 0,
      "created_at": "2025-11-14T10:00:00Z"
    }
  },

  "rickshaws": {
    "rickshaw_1": {
      "id": "rickshaw_1",
      "puller_name": "Karim Ahmed",
      "license_number": "DH-RICK-001",
      "current_station": "station_1",
      "status": "available",
      "total_points": 0,
      "total_rides": 0,
      "rating": 5.0,
      "created_at": "2025-11-14T10:00:00Z"
    },
    "rickshaw_2": {
      "id": "rickshaw_2",
      "puller_name": "Rahim Khan",
      "license_number": "DH-RICK-002",
      "current_station": "station_2",
      "status": "available",
      "total_points": 0,
      "total_rides": 0,
      "rating": 5.0,
      "created_at": "2025-11-14T10:00:00Z"
    }
  },

  "rides": {},

  "ride_requests": {},

  "fare_matrix": {
    "station_1_to_station_2": {
      "from": "station_1",
      "to": "station_2",
      "distance_km": 2.5,
      "base_fare": 40,
      "points": 8
    },
    "station_1_to_station_3": {
      "from": "station_1",
      "to": "station_3",
      "distance_km": 3.5,
      "base_fare": 50,
      "points": 10
    },
    "station_2_to_station_1": {
      "from": "station_2",
      "to": "station_1",
      "distance_km": 2.5,
      "base_fare": 40,
      "points": 8
    },
    "station_2_to_station_3": {
      "from": "station_2",
      "to": "station_3",
      "distance_km": 1.5,
      "base_fare": 30,
      "points": 6
    },
    "station_3_to_station_1": {
      "from": "station_3",
      "to": "station_1",
      "distance_km": 3.5,
      "base_fare": 50,
      "points": 10
    },
    "station_3_to_station_2": {
      "from": "station_3",
      "to": "station_2",
      "distance_km": 1.5,
      "base_fare": 30,
      "points": 6
    }
  },

  "system_config": {
    "points_per_km": 3,
    "base_fare_per_km": 15,
    "active_stations": 3,
    "active_rickshaws": 2,
    "version": "1.0.0"
  }
}
```

### Step 2.3: Import Data (Easy Method)

1. In Firebase Console → Realtime Database
2. Click the **⋮** (three dots) menu at the root
3. Select **"Import JSON"**
4. Upload the `initial_database.json` file (we'll create this)
5. Click **"Import"**

---

## 3. Security Rules Setup

### Step 3.1: Configure Security Rules

1. In Firebase Console → Realtime Database
2. Go to **"Rules"** tab
3. Replace existing rules with:

```json
{
  "rules": {
    ".read": true,
    ".write": true,

    "stations": {
      ".indexOn": ["status", "name"]
    },

    "users": {
      ".indexOn": ["laser_id", "privilege_type"]
    },

    "rickshaws": {
      ".indexOn": ["status", "current_station"],
      "$rickshaw_id": {
        ".validate": "newData.hasChildren(['id', 'puller_name', 'license_number', 'status'])"
      }
    },

    "rides": {
      ".indexOn": ["status", "rickshaw_id", "timestamp"],
      "$ride_id": {
        ".validate": "newData.hasChildren(['id', 'rickshaw_id', 'pickup_station', 'dropoff_station', 'status'])"
      }
    },

    "ride_requests": {
      ".indexOn": ["status", "timestamp", "pickup_station"],
      "$request_id": {
        ".validate": "newData.hasChildren(['id', 'pickup_station', 'dropoff_station', 'status'])"
      }
    },

    "fare_matrix": {
      ".read": true,
      ".write": false
    },

    "system_config": {
      ".read": true,
      ".write": false
    }
  }
}
```

4. Click **"Publish"**

⚠️ **Note**: For production, you should add authentication and restrict write access!

---

## 4. Web App Configuration

### Step 4.1: Get Your Firebase Config

1. Go to **Project Settings** (gear icon)
2. Scroll down to **"Your apps"** section
3. Select your web app
4. Copy the configuration object
5. Save it in `firebase-config.js`

### Step 4.2: Enable Required Services

1. **Realtime Database**: ✅ Already enabled
2. **Firebase Hosting**: (Optional) For deployment
   - Click **"Get started"**
   - Follow the setup instructions

---

## 5. Initial Data Population

### Method 1: Manual Entry (Good for Testing)

1. Go to Firebase Console → Realtime Database
2. Click **"+"** next to the database root
3. Add each collection manually

### Method 2: Import JSON (Recommended)

1. Create `initial_database.json` file with the structure above
2. In Firebase Console → Database → **⋮** → **Import JSON**
3. Select your file and import

### Method 3: Programmatic (Using Web App)

We'll create a setup script in the web app that populates initial data.

---

## 6. ESP32 Configuration

### Step 6.1: Install Arduino Libraries

In Arduino IDE, install:

- **Firebase ESP32 Client** by Mobizt
- **ArduinoJson** by Benoit Blanchon

### Step 6.2: ESP32 Code Configuration

```cpp
// Firebase and WiFi credentials
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourWiFiPassword"

#define FIREBASE_HOST "your-project-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "your-database-secret-or-token"

// You can get database secret from:
// Firebase Console → Project Settings → Service accounts → Database secrets
```

### Step 6.3: Get Database Secret (Legacy Token)

1. Firebase Console → **Project Settings**
2. **Service accounts** tab
3. **Database secrets** section
4. Click **"Show"** and copy the secret
5. Use this in your ESP32 code

**Alternative (Recommended)**: Use Firebase Anonymous Authentication

```cpp
// In your ESP32 code, use:
Firebase.signUp(&config, &auth, "", "");
```

---

## 🎯 Summary Checklist

### Firebase Setup ✅

- [ ] Create Firebase project
- [ ] Enable Realtime Database
- [ ] Import initial data structure
- [ ] Configure security rules
- [ ] Get Firebase config for web app
- [ ] Get database credentials for ESP32

### Web App Setup ✅

- [ ] Create HTML/CSS/JS files
- [ ] Add Firebase SDK
- [ ] Configure Firebase in `firebase-config.js`
- [ ] Test connection to database

### Hardware Setup ✅

- [ ] Install Arduino libraries
- [ ] Configure WiFi credentials
- [ ] Configure Firebase credentials
- [ ] Test ESP32 connection to Firebase

---

## 🔧 Testing Your Setup

### Test 1: Database Connection

```javascript
// In browser console:
firebase
  .database()
  .ref("system_config")
  .once("value")
  .then((snapshot) => console.log(snapshot.val()));
```

### Test 2: Write Data

```javascript
firebase.database().ref("ride_requests").push({
  id: "test_request",
  pickup_station: "station_1",
  dropoff_station: "station_2",
  status: "pending",
  timestamp: new Date().toISOString(),
});
```

### Test 3: Real-time Listener

```javascript
firebase
  .database()
  .ref("ride_requests")
  .on("value", (snapshot) => {
    console.log("Ride requests updated:", snapshot.val());
  });
```

---

## 🆘 Troubleshooting

### Issue: "Permission Denied"

**Solution**: Check security rules, ensure they allow read/write

### Issue: "Database URL not found"

**Solution**: Verify `databaseURL` in config matches your Firebase project

### Issue: "Network error"

**Solution**: Check internet connection, verify Firebase project is active

### Issue: ESP32 won't connect

**Solution**:

- Check WiFi credentials
- Verify Firebase host (don't include https://)
- Check database secret/auth token

---

## 📞 Support

- **Firebase Documentation**: https://firebase.google.com/docs/database
- **ESP32 Firebase Library**: https://github.com/mobizt/Firebase-ESP32
- **IOTrix Discussion Forum**: [Competition forum link]

---

**Last Updated**: November 14, 2025  
**Version**: 1.0
