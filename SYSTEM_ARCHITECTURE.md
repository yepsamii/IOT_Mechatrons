# Smart Rickshaw Management System - Architecture Document

## 🎯 Project Overview

A smart IoT-based rickshaw management system that connects passengers with rickshaw pullers through an automated station-based network using sensors, microcontrollers (ESP32), and a real-time web application.

---

## 📋 System Components

### 1. **Hardware Side** (Brief - Focus is on Web)

- **User Station:**
  - Ultrasonic sensor (detects user presence)
  - LDR sensor (laser privilege verification)
  - Confirmation button
  - Buzzer/LED feedback
- **Rickshaw Side:**
  - ESP32/ESP8266 microcontroller
  - OLED Display (0.96" I2C) - shows ride details
  - Wi-Fi connectivity to Firebase

### 2. **Web Application** (MAIN FOCUS)

- **Rickshaw Puller Dashboard** - Web-based UI
- Real-time ride request management
- Points and earnings tracking
- Ride history and statistics
- Station and destination management

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     USER JOURNEY                             │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  1. User approaches Station → Ultrasonic detects             │
│  2. User shows laser → LDR verifies privilege                │
│  3. User presses button → Selects destination                │
│  4. Request sent to Firebase → Available rickshaws notified  │
│                                                               │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                  RICKSHAW PULLER JOURNEY                     │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  1. Views incoming request on Web Dashboard                  │
│  2. Accepts/Rejects request                                  │
│  3. Navigates to pickup → Confirms pickup                    │
│  4. Drops at destination → Confirms drop-off                 │
│  5. Points credited → Ready for next ride                    │
│                                                               │
└─────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────┐
│                    TECH STACK FLOW                           │
├──────────────────────────────────────────────────────────────┤
│                                                                │
│   Hardware (ESP32)  ←──Wi-Fi──→  Firebase Realtime DB       │
│         ↓                              ↑↓                      │
│    OLED Display                   Web Dashboard              │
│                                   (HTML/CSS/JS)              │
│                                                                │
└──────────────────────────────────────────────────────────────┘
```

---

## 🗄️ Firebase Realtime Database Structure

### Database Schema (JSON Structure)

```json
{
  "stations": {
    "station_1": {
      "id": "station_1",
      "name": "Station A",
      "location": {
        "lat": 22.4593,
        "lng": 91.9871
      },
      "status": "active",
      "created_at": "2025-11-14T10:00:00Z"
    },
    "station_2": {
      "id": "station_2",
      "name": "Station B",
      "location": {
        "lat": 22.4623,
        "lng": 91.9901
      },
      "status": "active",
      "created_at": "2025-11-14T10:00:00Z"
    }
  },

  "users": {
    "user_1": {
      "id": "user_1",
      "name": "John Doe",
      "privilege_type": "premium",
      "laser_id": "laser_001",
      "total_rides": 15,
      "created_at": "2025-11-01T08:00:00Z"
    }
  },

  "rickshaws": {
    "rickshaw_1": {
      "id": "rickshaw_1",
      "puller_name": "Karim Ahmed",
      "license_number": "DH-RICK-001",
      "current_station": "station_1",
      "status": "available",
      "total_points": 450,
      "total_rides": 28,
      "rating": 4.7,
      "created_at": "2025-11-01T06:00:00Z"
    },
    "rickshaw_2": {
      "id": "rickshaw_2",
      "puller_name": "Rahim Khan",
      "license_number": "DH-RICK-002",
      "current_station": "station_2",
      "status": "available",
      "total_points": 320,
      "total_rides": 22,
      "rating": 4.5,
      "created_at": "2025-11-01T06:30:00Z"
    }
  },

  "rides": {
    "ride_1": {
      "id": "ride_1",
      "user_id": "user_1",
      "rickshaw_id": "rickshaw_1",
      "pickup_station": "station_1",
      "dropoff_station": "station_3",
      "distance_km": 3.5,
      "fare": 50,
      "points_earned": 10,
      "status": "completed",
      "request_time": "2025-11-14T10:15:00Z",
      "accept_time": "2025-11-14T10:16:00Z",
      "pickup_time": "2025-11-14T10:20:00Z",
      "dropoff_time": "2025-11-14T10:35:00Z"
    }
  },

  "ride_requests": {
    "request_1": {
      "id": "request_1",
      "user_id": "user_1",
      "pickup_station": "station_1",
      "dropoff_station": "station_2",
      "privilege_verified": true,
      "status": "pending",
      "distance_km": 2.5,
      "estimated_fare": 40,
      "estimated_points": 8,
      "timestamp": "2025-11-14T11:00:00Z",
      "rejected_by": []
    }
  },

  "fare_matrix": {
    "station_1_to_station_2": {
      "distance_km": 2.5,
      "base_fare": 40,
      "points": 8
    },
    "station_1_to_station_3": {
      "distance_km": 3.5,
      "base_fare": 50,
      "points": 10
    },
    "station_2_to_station_3": {
      "distance_km": 1.5,
      "base_fare": 30,
      "points": 6
    }
  },

  "system_config": {
    "points_per_km": 3,
    "base_fare_per_km": 15,
    "active_stations": 3,
    "active_rickshaws": 5
  }
}
```

---

## 🎨 Web Application Design

### Pages/Views

1. **Rickshaw Puller Dashboard** (Main Page)
   - Real-time ride requests
   - Current ride status
   - Points & earnings summary
   - Ride history
2. **Ride Request Card**
   - Pickup location
   - Drop-off location
   - Distance & fare
   - Accept/Reject buttons
3. **Active Ride View**
   - Passenger details
   - Route information
   - Confirm pickup button
   - Confirm drop-off button
4. **Statistics Dashboard**
   - Daily/Weekly/Monthly earnings
   - Total rides completed
   - Average rating
   - Points accumulated

---

## 🔄 System Workflow

### Workflow 1: New Ride Request

```
┌──────────┐      ┌──────────┐      ┌──────────┐      ┌──────────┐
│  User at │      │ Hardware │      │ Firebase │      │   Web    │
│ Station  │      │ (ESP32)  │      │    DB    │      │Dashboard │
└────┬─────┘      └────┬─────┘      └────┬─────┘      └────┬─────┘
     │                 │                  │                  │
     │ 1. Detected     │                  │                  │
     ├────────────────>│                  │                  │
     │                 │                  │                  │
     │ 2. Laser verify │                  │                  │
     ├────────────────>│                  │                  │
     │                 │                  │                  │
     │ 3. Select dest  │                  │                  │
     ├────────────────>│                  │                  │
     │                 │                  │                  │
     │                 │ 4. Create request│                  │
     │                 ├─────────────────>│                  │
     │                 │                  │                  │
     │                 │                  │ 5. Notify nearby │
     │                 │                  ├─────────────────>│
     │                 │                  │                  │
```

### Workflow 2: Accepting & Completing Ride

```
┌──────────┐      ┌──────────┐      ┌──────────┐
│   Web    │      │ Firebase │      │ Hardware │
│Dashboard │      │    DB    │      │ (OLED)   │
└────┬─────┘      └────┬─────┘      └────┬─────┘
     │                  │                  │
     │ 1. Click Accept  │                  │
     ├─────────────────>│                  │
     │                  │                  │
     │                  │ 2. Update status │
     │                  ├─────────────────>│
     │                  │                  │
     │                  │ 3. Show ride info│
     │                  │                  │
     │ 4. Navigate      │                  │
     │    to pickup     │                  │
     │                  │                  │
     │ 5. Confirm pickup│                  │
     ├─────────────────>│                  │
     │                  │                  │
     │ 6. Drive to dest │                  │
     │                  │                  │
     │ 7. Confirm drop  │                  │
     ├─────────────────>│                  │
     │                  │                  │
     │                  │ 8. Credit points │
     │<─────────────────┤                  │
     │                  │                  │
```

---

## 💡 Key Features

### For Rickshaw Pullers (Web Dashboard)

- ✅ Real-time ride notifications
- ✅ Accept/Reject rides instantly
  - **Smart Rejection System**: When a rickshaw rejects a request, it only hides it from that specific rickshaw. Other rickshaws can still see and accept the same request. This ensures maximum ride fulfillment.
- ✅ View passenger details
- ✅ Track earnings and points
- ✅ View ride history
- ✅ Distance-based fare calculation
- ✅ Responsive mobile-friendly UI

### For Hardware Integration

- ✅ ESP32 sends ride requests to Firebase
- ✅ OLED displays ride details from Firebase
- ✅ Real-time status synchronization

---

## 🔐 Firebase Security Rules

```json
{
  "rules": {
    "stations": {
      ".read": true,
      ".write": "auth != null"
    },
    "users": {
      ".read": true,
      ".write": "auth != null"
    },
    "rickshaws": {
      ".read": true,
      "$rickshaw_id": {
        ".write": "auth != null && auth.uid == $rickshaw_id"
      }
    },
    "rides": {
      ".read": true,
      ".write": "auth != null"
    },
    "ride_requests": {
      ".read": true,
      ".write": true
    },
    "fare_matrix": {
      ".read": true,
      ".write": "auth != null"
    }
  }
}
```

---

## 📊 Data Flow Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                     COMPLETE DATA FLOW                       │
└─────────────────────────────────────────────────────────────┘

     HARDWARE LAYER          CLOUD LAYER           WEB LAYER
┌──────────────────┐   ┌──────────────────┐   ┌──────────────┐
│                  │   │                  │   │              │
│  Station Module  │   │    Firebase      │   │  Rickshaw    │
│  - Ultrasonic    │──>│  Realtime DB     │<──│  Dashboard   │
│  - LDR           │   │                  │   │  (Web App)   │
│  - Button        │   │  - ride_requests │   │              │
│  - ESP32         │   │  - rides         │   │ - Accept/    │
│                  │   │  - rickshaws     │   │   Reject     │
└──────────────────┘   │  - stations      │   │ - Confirm    │
                       │  - users         │   │   Pickup     │
┌──────────────────┐   │                  │   │ - Confirm    │
│                  │   └──────────────────┘   │   Drop       │
│  Rickshaw OLED   │                          │              │
│  - ESP32         │<─────────────────────────│ - Statistics │
│  - 0.96" Display │                          │              │
│                  │                          └──────────────┘
└──────────────────┘
```

---

## 🚀 Technology Stack

### Frontend

- **HTML5** - Structure
- **CSS3** - Styling (Modern, Responsive)
- **JavaScript (ES6+)** - Logic
- **Firebase SDK** - Real-time data sync

### Backend

- **Firebase Realtime Database** - NoSQL database
- **Firebase Authentication** - (Optional) User auth
- **Firebase Hosting** - Deploy web app

### Hardware Integration

- **ESP32/ESP8266** - Microcontroller
- **Arduino IDE** - Programming
- **Wi-Fi** - Firebase connectivity

---

## 📈 Judging Criteria Alignment

| Criteria                         | Our Implementation                                 | Score Focus        |
| -------------------------------- | -------------------------------------------------- | ------------------ |
| **Hardware Functionality (30%)** | ESP32 + sensors integrated with Firebase           | Real-time sync     |
| **Database Design (20%)**        | Well-structured Firebase schema with relationships | Normalization      |
| **Web Dashboard (20%)**          | Modern, responsive, real-time UI                   | UX & visualization |
| **Documentation (15%)**          | Comprehensive docs + video                         | Clear workflow     |
| **Innovation (15%)**             | Laser privilege system, points-based gamification  | Unique features    |

---

## 📝 Next Steps

1. ✅ Set up Firebase project
2. ✅ Create database structure
3. ✅ Develop web dashboard
4. ✅ Integrate hardware with Firebase
5. ✅ Test end-to-end flow
6. ✅ Create documentation & video

---

## 🔗 Useful Resources

- Firebase Documentation: https://firebase.google.com/docs
- ESP32 Firebase Library: https://github.com/mobizt/Firebase-ESP32
- Realtime Database Guide: https://firebase.google.com/docs/database

---

**Document Version:** 1.0  
**Last Updated:** November 14, 2025  
**Competition:** IOTrix - Televerse 1.0
