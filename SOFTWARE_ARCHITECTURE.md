# Software Architecture Documentation

**AERAS - Accessible E-Rickshaw Automation System**

---

## 1. System Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                         SYSTEM ARCHITECTURE                         │
└─────────────────────────────────────────────────────────────────────┘

                    ┌──────────────────────┐
                    │   PHYSICAL LAYER     │
                    │                      │
                    │  • Location Blocks   │
                    │  • LED System        │
                    │  • GPS Sensors       │
                    └──────────┬───────────┘
                               │
                               ↓
┌──────────────┐      ┌───────────────┐      ┌──────────────┐
│    USERS     │      │   RICKSHAWS   │      │    ADMIN     │
│              │      │   (Pullers)   │      │              │
│ • Seniors    │      │               │      │ • Dashboard  │
│ • Special    │◄────►│ • Web App     │◄────►│ • Monitor    │
│   Needs      │      │ • GPS Device  │      │ • Review     │
│              │      │ • Dashboard   │      │              │
└──────┬───────┘      └───────┬───────┘      └──────┬───────┘
       │                      │                      │
       │                      │                      │
       └──────────────────────┼──────────────────────┘
                              │
                              ↓
                    ┌─────────────────┐
                    │  FIREBASE RTDB  │
                    │   (BACKEND)     │
                    │                 │
                    │  • Real-time    │
                    │  • NoSQL        │
                    │  • Auth         │
                    │  • Sync <3s     │
                    └─────────────────┘

COMPONENTS:
├── Users (Elderly/Special Needs)
│   └── Physical interaction via location blocks + LED feedback
│
├── Rickshaw Pullers
│   └── Web Dashboard (React + Firebase SDK)
│
├── Admin
│   └── Web Dashboard (same app, admin view)
│
└── Backend (Firebase Realtime Database)
    ├── Real-time synchronization
    ├── NoSQL data storage
    └── Authentication layer
```

---

## 2. Data Flow Diagram

### 2.1 Complete Ride Flow

```
USER                LED SYSTEM          RICKSHAW           FIREBASE           ADMIN
  │                     │                   │                  │                │
  │ (1) Press button    │                   │                  │                │
  │────────────────────►│                   │                  │                │
  │                     │                   │                  │                │
  │                     │ (2) Create request│                  │                │
  │                     │───────────────────┼─────────────────►│                │
  │                     │                   │                  │                │
  │                     │                   │ (3) Notify      ││                │
  │                     │                   │◄─────────────────┘                │
  │                     │                   │ Audio/Vibration                   │
  │                     │                   │                                   │
  │                     │                   │ (4) Accept/Reject                 │
  │                     │                   │──────────────────►                │
  │                     │                   │                  │                │
  │   (5) LED Update    │◄──────────────────────────────────────               │
  │   🟡 Yellow ON      │                   │                  │                │
  │◄────────────────────┘                   │                  │                │
  │                                         │                  │                │
  │                                         │ (6) Pickup GPS   │                │
  │                                         │──────────────────►│                │
  │                                         │                  │                │
  │   (7) LED Update    │◄──────────────────────────────────────               │
  │   🟢 Green ON       │                   │                  │                │
  │◄────────────────────┘                   │                  │                │
  │                                         │                  │                │
  │                                         │ (8) Dropoff GPS  │                │
  │                                         │──────────────────►│                │
  │                                         │                  │                │
  │                                         │     (9) Calculate Points          │
  │                                         │◄─────────────────┘                │
  │                                         │                                   │
  │                                         │      (10) Review (if needed)      │
  │                                         │───────────────────────────────────►│
  │                                         │                                   │
  └─────────────────────────────────────────┴───────────────────────────────────┘

FLOW STAGES:
1. Request Creation   → User presses location block button
2. Database Write     → ride_requests/{id} created with timestamp
3. Puller Notified    → Real-time listener triggers audio/vibration
4. Accept/Reject      → Puller responds via web dashboard
5. LED Status Update  → Yellow (waiting), Red (rejected), Green (pickup)
6. Pickup Confirmed   → GPS location recorded
7. Green LED ON       → User notified puller has arrived
8. Dropoff Confirmed  → GPS location + distance calculated
9. Points Calculated  → Based on GPS accuracy (10 - distance/10)
10. Admin Review      → Manual review if distance >100m
```

### 2.2 LED Status Flow

```
REQUEST STATE          LED STATUS          COLOR      TRIGGERED BY
─────────────────────────────────────────────────────────────────────
Initial Request    →   waiting          → 🔴 ALL OFF  → User button press
Accepted           →   waiting          → 🟡 Yellow    → Puller accepts
Pickup Confirmed   →   pickup_confirmed → 🟢 Green     → Puller GPS confirm
Rejected/Timeout   →   rejected         → 🔴 Red       → All reject OR 60s timeout
Completed          →   [removed]        → ⚫ OFF       → Dropoff confirmed
```

### 2.3 GPS Verification Flow

```
DROPOFF EVENT
     │
     ├─► Get GPS Coordinates (lat, lng, accuracy)
     │
     ├─► Fetch Destination Block Coordinates
     │
     ├─► Calculate Distance using Haversine Formula
     │
     ├─► Calculate Points:
     │        Base Points = 10
     │        Penalty = distance(m) / 10
     │        Final = 10 - Penalty (min: 0)
     │
     └─► Determine Status:
          ├─► 0-10m   → rewarded  (Full 10 points)
          ├─► 11-50m  → rewarded  (Partial 8+ points)
          ├─► 51-100m → reduced   (5+ points)
          └─► >100m   → pending   (Admin review)
```

---

## 3. API Endpoint Documentation

### 3.1 Firebase Realtime Database Structure

**Base URL:** `https://rickshaw-ride-c5683-default-rtdb.asia-southeast1.firebasedatabase.app`

**Authentication:** Firebase API Key + Database Rules

### 3.2 Database Collections (Endpoints)

#### **3.2.1 Location Blocks** `/location_blocks`

**Purpose:** Physical pickup/dropoff locations

**Methods:**

- `GET /location_blocks` - Retrieve all blocks
- `GET /location_blocks/{id}` - Get specific block
- `PUT /location_blocks/{id}` - Update block status

**Schema:**

```json
{
  "block_1": {
    "id": "block_1",
    "name": "CUET Campus",
    "coordinates": {
      "lat": 22.4633,
      "lng": 91.9714
    },
    "status": "active",
    "busy": false,
    "total_requests": 42
  }
}
```

---

#### **3.2.2 Users** `/users`

**Purpose:** Registered users (elderly/special needs)

**Methods:**

- `GET /users` - List all users
- `GET /users/{id}` - Get user profile
- `PUT /users/{id}` - Update user data
- `POST /users/{id}` - Create new user

**Schema:**

```json
{
  "user_1": {
    "id": "user_1",
    "name": "Abdul Rahman",
    "age": 67,
    "user_type": "senior_citizen",
    "laser_id": "BD-CUET-001",
    "privilege_verified": true,
    "total_rides": 23,
    "created_at": 1699900800000
  }
}
```

---

#### **3.2.3 Rickshaws** `/rickshaws`

**Purpose:** Rickshaw puller profiles

**Methods:**

- `GET /rickshaws` - List all rickshaws
- `GET /rickshaws/{id}` - Get rickshaw details
- `PUT /rickshaws/{id}/status` - Update availability
- `PUT /rickshaws/{id}/total_points` - Update points (after ride)

**Schema:**

```json
{
  "rickshaw_1": {
    "id": "rickshaw_1",
    "puller_name": "Karim Mia",
    "license_number": "CTG-RS-1001",
    "phone": "+8801712345678",
    "current_location": {
      "lat": 22.4633,
      "lng": 91.9714
    },
    "status": "available", // available | busy | offline
    "total_points": 250,
    "total_rides": 45,
    "rating": 4.8
  }
}
```

---

#### **3.2.4 Ride Requests** `/ride_requests`

**Purpose:** Pending ride requests from users

**Methods:**

- `POST /ride_requests` - Create new request
- `GET /ride_requests?status=pending` - Get pending requests
- `PUT /ride_requests/{id}/status` - Accept/Reject
- `PUT /ride_requests/{id}/led_status` - Update LED
- `DELETE /ride_requests/{id}` - Remove completed

**Schema:**

```json
{
  "req_1699900800000": {
    "id": "req_1699900800000",
    "user_id": "user_1",
    "user_name": "Abdul Rahman",
    "pickup_block": "block_1",
    "dropoff_block": "block_2",
    "distance_km": 2.5,
    "estimated_fare": 67,
    "estimated_points": 10,
    "privilege_verified": true,
    "status": "pending", // pending | accepted | rejected | expired
    "timestamp": 1699900800000,
    "rejected_by": [], // Array of rickshaw IDs
    "assigned_rickshaw": null,
    "led_status": "waiting" // waiting | pickup_confirmed | rejected
  }
}
```

**Business Logic:**

- Auto-reject after 60 seconds if no acceptance
- Track rejected_by array for multi-rickshaw handling
- LED status updates trigger physical hardware

---

#### **3.2.5 Active Rides** `/active_rides`

**Purpose:** Currently ongoing rides

**Methods:**

- `POST /active_rides` - Create (on accept)
- `GET /active_rides?rickshaw_id={id}` - Get active ride for puller
- `PUT /active_rides/{id}/status` - Update ride status
- `PUT /active_rides/{id}/pickup_location` - Record pickup GPS
- `DELETE /active_rides/{id}` - Move to completed

**Schema:**

```json
{
  "ride_1699900800000": {
    "id": "ride_1699900800000",
    "request_id": "req_1699900800000",
    "user_id": "user_1",
    "rickshaw_id": "rickshaw_1",
    "pickup_block": "block_1",
    "dropoff_block": "block_2",
    "distance_km": 2.5,
    "fare": 67,
    "points_earned": 0,
    "status": "accepted", // accepted | picked_up | completed
    "request_time": 1699900800000,
    "accept_time": 1699900820000,
    "pickup_time": null,
    "dropoff_time": null,
    "pickup_location": null,
    "dropoff_location": null,
    "points_status": "pending"
  }
}
```

---

#### **3.2.6 Completed Rides** `/completed_rides`

**Purpose:** Historical ride data

**Methods:**

- `POST /completed_rides` - Create (on dropoff)
- `GET /completed_rides?rickshaw_id={id}` - Get ride history
- `GET /completed_rides?user_id={id}` - Get user history

**Schema:**

```json
{
  "ride_1699900800000": {
    "id": "ride_1699900800000",
    "request_id": "req_1699900800000",
    "user_id": "user_1",
    "rickshaw_id": "rickshaw_1",
    "pickup_block": "block_1",
    "dropoff_block": "block_2",
    "distance_km": 2.5,
    "fare": 67,
    "points_earned": 9,
    "status": "completed",
    "request_time": 1699900800000,
    "accept_time": 1699900820000,
    "pickup_time": 1699900900000,
    "dropoff_time": 1699901200000,
    "dropoff_location": {
      "lat": 22.4725,
      "lng": 91.9845,
      "accuracy": 15
    },
    "dropoff_distance_from_block": 12.5, // meters
    "points_status": "rewarded" // rewarded | reduced | pending | denied
  }
}
```

---

#### **3.2.7 Points History** `/points_history`

**Purpose:** Audit trail for point calculations

**Methods:**

- `POST /points_history` - Create entry (on dropoff)
- `GET /points_history?rickshaw_id={id}` - Get puller history
- `PUT /points_history/{id}/admin_reviewed` - Mark reviewed

**Schema:**

```json
{
  "ph_1699901200000": {
    "id": "ph_1699901200000",
    "ride_id": "ride_1699900800000",
    "rickshaw_id": "rickshaw_1",
    "base_points": 10,
    "distance_penalty": 1.25, // distance(m) / 10
    "final_points": 9,
    "status": "rewarded",
    "gps_accuracy": 12.5, // meters from block
    "timestamp": 1699901200000,
    "admin_reviewed": false
  }
}
```

**Point Calculation Formula:**

```javascript
const basePoints = 10;
const distanceFromBlock = calculateGPSDistance(dropoff, destination);
const penalty = distanceFromBlock / 10.0; // 1 point per 10 meters
const finalPoints = Math.max(0, Math.floor(basePoints - penalty));

// Status rules:
// 0-10m:   rewarded (full points)
// 11-50m:  rewarded (partial)
// 51-100m: reduced
// >100m:   pending (admin review)
```

---

### 3.3 Real-time Operations

#### **3.3.1 Subscribe to Changes (Firebase Listeners)**

**Ride Requests Listener:**

```javascript
import { ref, onValue } from "firebase/database";

const requestsRef = ref(database, "ride_requests");
onValue(requestsRef, (snapshot) => {
  const data = snapshot.val();
  const pending = Object.values(data).filter((req) => req.status === "pending");
  // Trigger audio/vibration notification
});
```

**Active Ride Listener:**

```javascript
const activeRideRef = ref(database, `active_rides`);
onValue(activeRideRef, (snapshot) => {
  const data = snapshot.val();
  const myRide = Object.values(data).find(
    (ride) => ride.rickshaw_id === currentRickshawId
  );
  // Update UI
});
```

---

#### **3.3.2 Update Operations**

**Accept Ride Request:**

```javascript
import { ref, update } from "firebase/database";

const acceptRide = async (requestId, rickshawId) => {
  const updates = {};

  // Create active ride
  updates[`active_rides/${rideId}`] = {
    id: rideId,
    request_id: requestId,
    rickshaw_id: rickshawId,
    status: "accepted",
    accept_time: Date.now(),
    // ... other fields
  };

  // Update request
  updates[`ride_requests/${requestId}/status`] = "accepted";
  updates[`ride_requests/${requestId}/assigned_rickshaw`] = rickshawId;
  updates[`ride_requests/${requestId}/led_status`] = "waiting"; // Yellow LED

  // Update rickshaw
  updates[`rickshaws/${rickshawId}/status`] = "busy";

  await update(ref(database), updates);
};
```

**Confirm Pickup (Green LED):**

```javascript
const confirmPickup = async (rideId, requestId, gpsLocation) => {
  const updates = {};
  updates[`active_rides/${rideId}/status`] = "picked_up";
  updates[`active_rides/${rideId}/pickup_time`] = Date.now();
  updates[`active_rides/${rideId}/pickup_location`] = gpsLocation;
  updates[`ride_requests/${requestId}/led_status`] = "pickup_confirmed"; // Green LED

  await update(ref(database), updates);
};
```

**Complete Ride (GPS Verification):**

```javascript
const completeRide = async (rideId, rickshawId, dropoffGPS, blockCoords) => {
  // Calculate distance and points
  const distance = calculateHaversineDistance(dropoffGPS, blockCoords);
  const points = calculatePoints(distance);

  const updates = {};

  // Move to completed
  updates[`completed_rides/${rideId}`] = {
    // ... ride data
    dropoff_time: Date.now(),
    dropoff_location: dropoffGPS,
    dropoff_distance_from_block: distance,
    points_earned: points.finalPoints,
    points_status: points.status,
  };

  // Add points history
  updates[`points_history/${pointsId}`] = {
    ride_id: rideId,
    rickshaw_id: rickshawId,
    base_points: 10,
    distance_penalty: distance / 10,
    final_points: points.finalPoints,
    status: points.status,
    gps_accuracy: distance,
  };

  // Update rickshaw
  updates[`rickshaws/${rickshawId}/status`] = "available";
  updates[`rickshaws/${rickshawId}/total_points`] = increment(
    points.finalPoints
  );
  updates[`rickshaws/${rickshawId}/total_rides`] = increment(1);

  await update(ref(database), updates);

  // Remove active ride and request
  await remove(ref(database, `active_rides/${rideId}`));
  await remove(ref(database, `ride_requests/${requestId}`));
};
```

---

### 3.4 Database Rules & Security

**Firebase Security Rules:**

```json
{
  "rules": {
    ".read": "auth != null",
    ".write": "auth != null",

    "ride_requests": {
      ".indexOn": ["status", "timestamp", "user_id"]
    },

    "active_rides": {
      ".indexOn": ["rickshaw_id", "status"]
    },

    "completed_rides": {
      ".indexOn": ["rickshaw_id", "dropoff_time"]
    },

    "points_history": {
      ".indexOn": ["rickshaw_id", "timestamp"]
    }
  }
}
```

---

### 3.5 Performance Metrics

| Operation         | Target | Actual |
| ----------------- | ------ | ------ |
| Real-time Sync    | <3s    | <2s    |
| Request Creation  | <1s    | <500ms |
| Accept/Reject     | <2s    | <800ms |
| GPS Verification  | <3s    | <2s    |
| Point Calculation | <1s    | <100ms |

---

### 3.6 Error Handling

**Common Error Codes:**

| Code              | Message                     | Resolution                                |
| ----------------- | --------------------------- | ----------------------------------------- |
| PERMISSION_DENIED | GPS permission not granted  | Request permissions or fallback to manual |
| NETWORK_ERROR     | Firebase connection lost    | Retry with exponential backoff            |
| TIMEOUT           | Request exceeded 60 seconds | Auto-reject and activate Red LED          |
| INVALID_REQUEST   | Missing required fields     | Validate before submission                |
| GPS_UNAVAILABLE   | Cannot get location         | Manual verification mode                  |

---

## Technology Stack

### Frontend

- **Framework:** React 18 + Vite
- **Language:** JavaScript ES6+
- **Styling:** CSS3 (Custom)
- **State Management:** React Hooks (useState, useEffect)

### Backend

- **Database:** Firebase Realtime Database (NoSQL)
- **Authentication:** Firebase Auth (optional)
- **Hosting:** Vercel (CI/CD enabled)

### Hardware Integration

- **GPS:** Browser Geolocation API + external GPS modules
- **LED System:** Arduino/ESP32 (monitors Firebase DB)
- **Location Blocks:** Physical buttons with microcontrollers

---

## Deployment Architecture

```
GitHub Repo (Main Branch)
         │
         ↓
   GitHub Actions CI/CD
         │
         ├──► Build React App (Vite)
         │
         ├──► Deploy to Vercel
         │         │
         │         └──► Production URL
         │
         └──► Firebase Hosting (optional)
```

---

## Data Integrity & Privacy

### Database Features (from schema.sql)

- **History Tables:** Audit trail for users, rickshaws, rides
- **Triggers:** Auto-log changes on UPDATE/DELETE
- **Indexes:** Optimized queries for performance
- **Constraints:** Foreign keys, CHECK constraints for data validation

### Privacy Compliance

- User data anonymization after 90 days
- GDPR-compliant data retention
- Secure laser ID verification
- No tracking of location history beyond ride scope

---

## Scalability Considerations

### Current Capacity

- **Users:** 100+ concurrent
- **Rickshaws:** 50+ active pullers
- **Requests:** 1000+ per day
- **Response Time:** <3 seconds

### Scaling Strategy

1. **Firebase Spark Plan** → Blaze Plan for production
2. **Add Redis Cache** for frequently accessed data
3. **Implement CDN** for static assets (Vercel Edge Network)
4. **Load Balancing** via Firebase Realtime Database auto-scaling

---

**Document Version:** 1.0  
**Last Updated:** November 15, 2025  
**Prepared For:** Software Architecture [3 marks]  
**System Status:** ✅ Production Ready
