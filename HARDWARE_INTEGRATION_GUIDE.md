# 🔧 AERAS - Hardware Integration Guide

> **For: ESP32/Microcontroller Development**  
> **System: Smart Rickshaw Location Block Module**

---

## 📋 Table of Contents

1. [Overview](#overview)
2. [Firebase Connection Setup](#firebase-connection-setup)
3. [Database Structure](#database-structure)
4. [Data Flow](#data-flow)
5. [Hardware Requirements](#hardware-requirements)
6. [LED Control Logic](#led-control-logic)
7. [API Operations](#api-operations)
8. [Example Data Formats](#example-data-formats)
9. [Testing & Validation](#testing--validation)

---

## 🎯 Overview

### What the Hardware Module Does

The hardware module is a **Location Block Station** that:

- Identifies users via **Laser ID Scanner**
- Allows users to **select destination** via buttons
- Creates **ride requests** in Firebase
- Shows **LED status** feedback (Yellow/Red/Green)
- Works **offline-first** with queue system

### System Architecture

```
User → [Laser Scanner] → ESP32 → Firebase → Web Dashboard → Rickshaw Puller
                            ↓
                        [LED Strip]
                            ↓
                    Status Feedback to User
```

---

## 🔗 Firebase Connection Setup

### Firebase Configuration

**Database URL:**

```
https://rickshaw-ride-c5683-default-rtdb.asia-southeast1.firebasedatabase.app
```

**Project ID:** `rickshaw-ride-c5683`

**Authentication:**

- Use **Anonymous Authentication** OR
- Use **Service Account** (recommended for production)

### Connection Libraries

**For ESP32 (Arduino IDE):**

- Library: `Firebase-ESP-Client` by Mobizt
- Install via: Arduino Library Manager

**Required Credentials:**

- API Key: `AIzaSyCyPJA1r5HQKGPKT_xRPio1Yzafgu1pxAI`
- Database URL: (see above)

### Database Rules (Already Set)

```json
{
  "rules": {
    ".read": true,
    ".write": true
  }
}
```

⚠️ **Note:** For production, implement proper security rules

---

## 🗄️ Database Structure

### Collections You'll Interact With

#### 1. `location_blocks/{block_id}`

**Purpose:** Station status and metadata

**Structure:**

```json
{
  "id": "cuet_campus",
  "name": "CUET Campus",
  "coordinates": {
    "lat": 22.4633,
    "lng": 91.9714
  },
  "status": "active",
  "busy": false,
  "total_requests": 0
}
```

**Fields:**

- `id` (string) - Block identifier
- `name` (string) - Display name
- `coordinates` (object) - GPS location
- `status` (string) - "active" or "maintenance"
- `busy` (boolean) - Current request in progress
- `total_requests` (number) - Counter

---

#### 2. `users/{user_id}`

**Purpose:** User verification and information

**Structure:**

```json
{
  "id": "user_1",
  "name": "Ahmed Hassan",
  "age": 65,
  "user_type": "senior_citizen",
  "laser_id": "LASER_001",
  "privilege_verified": true,
  "total_rides": 8,
  "created_at": "2025-11-14T10:00:00Z"
}
```

**Fields:**

- `laser_id` (string) - **CRITICAL:** Scanned from user's card
- `privilege_verified` (boolean) - Must be `true`
- `user_type` (string) - "senior_citizen" or "special_needs"

---

#### 3. `ride_requests/{request_id}`

**Purpose:** Active ride requests from location blocks

**Structure:**

```json
{
  "id": "req_demo_001",
  "user_id": "user_1",
  "pickup_block": "cuet_campus",
  "dropoff_block": "pahartali",
  "distance_km": 2.5,
  "estimated_fare": 40,
  "estimated_points": 10,
  "privilege_verified": true,
  "status": "pending",
  "timestamp": 1731582000000,
  "rejected_by": [],
  "assigned_rickshaw": null,
  "led_status": "waiting"
}
```

**Fields Explanation:**

- `id` (string) - Unique request ID (generate: `"req_" + timestamp`)
- `user_id` (string) - From laser scan lookup
- `pickup_block` (string) - YOUR station ID
- `dropoff_block` (string) - Selected by user (button press)
- `status` (string) - Lifecycle: `"pending"` → `"accepted"` → `"picked_up"` → `"completed"`
- `timestamp` (number) - Unix timestamp in milliseconds
- `led_status` (string) - Controls LED: `"waiting"`, `"offer_incoming"`, `"pickup_confirmed"`, `"rejected"`, `"expired"`

---

#### 4. `fare_matrix/{route_id}`

**Purpose:** Lookup fare and distance for routes

**Structure:**

```json
{
  "from": "cuet_campus",
  "to": "pahartali",
  "distance_km": 2.5,
  "base_fare": 40,
  "points": 10
}
```

**Route ID Format:** `{from}_to_{to}`  
Example: `cuet_campus_to_pahartali`

---

## 🔄 Data Flow

### Complete User Journey

```
1. User scans Laser ID card
   ↓
2. ESP32 reads LASER_ID (e.g., "LASER_001")
   ↓
3. ESP32 queries: /users?orderBy="laser_id"&equalTo="LASER_001"
   ↓
4. Verify: user.privilege_verified == true
   ↓
5. User presses destination button (1-4)
   ↓
6. ESP32 looks up: /fare_matrix/{from}_to_{to}
   ↓
7. ESP32 creates: /ride_requests/{request_id}
   ↓
8. ESP32 listens: /ride_requests/{request_id}/led_status
   ↓
9. LED updates based on status changes
   ↓
10. Request expires after 60 seconds (if no acceptance)
```

---

## 🔌 Hardware Requirements

### Components Needed

1. **ESP32 Dev Board** - Main controller
2. **Laser ID Scanner** - Barcode/QR reader (UART/USB)
3. **WS2812B RGB LED Strip** - 10-20 LEDs
4. **4 Push Buttons** - Destination selection
5. **1 LCD Display (Optional)** - 16x2 I2C
6. **Power Supply** - 5V 2A minimum
7. **WiFi Connection** - 2.4GHz network

### Pin Configuration (Suggested)

```
ESP32 GPIO Pins:
- GPIO 16 (RX2) → Laser Scanner TX
- GPIO 17 (TX2) → Laser Scanner RX
- GPIO 5        → LED Strip Data
- GPIO 18-21    → Destination Buttons (with pull-up)
- GPIO 22-23    → I2C LCD (SCL, SDA)
```

---

## 💡 LED Control Logic

### LED Status Mapping

| Firebase `led_status` | LED Color | Pattern       | Meaning                            |
| --------------------- | --------- | ------------- | ---------------------------------- |
| `waiting`             | 🟡 Yellow | Solid         | Request sent, waiting for rickshaw |
| `offer_incoming`      | 🟡 Yellow | Blinking fast | Rickshaw accepted, coming to you   |
| `pickup_confirmed`    | 🟢 Green  | Solid         | Rickshaw arrived, get in!          |
| `rejected`            | 🔴 Red    | Solid 3s      | All rickshaws rejected             |
| `expired`             | 🔴 Red    | Blink 3x      | Request timed out (60s)            |

### LED Control Flow

1. **Initial State:** All LEDs OFF
2. **After Request Created:** Yellow SOLID
3. **Listen to:** `/ride_requests/{request_id}/led_status`
4. **Update LED** whenever value changes
5. **Reset to OFF** after 5 seconds on Green/Red

---

## 🔧 API Operations

### 1. User Lookup (Laser ID Scan)

**Operation:** Query by Laser ID

**Firebase Path:**

```
GET /users.json?orderBy="laser_id"&equalTo="LASER_001"
```

**Expected Response:**

```json
{
  "user_1": {
    "id": "user_1",
    "name": "Ahmed Hassan",
    "laser_id": "LASER_001",
    "privilege_verified": true,
    ...
  }
}
```

**Validation:**

- Check `privilege_verified == true`
- If false: Show error, reject request
- If not found: Invalid card

---

### 2. Fare Lookup (Destination Selection)

**Operation:** Get route information

**Firebase Path:**

```
GET /fare_matrix/{pickup_block}_to_{dropoff_block}.json
```

**Example:**

```
GET /fare_matrix/cuet_campus_to_pahartali.json
```

**Response:**

```json
{
  "from": "cuet_campus",
  "to": "pahartali",
  "distance_km": 2.5,
  "base_fare": 40,
  "points": 10
}
```

---

### 3. Create Ride Request

**Operation:** POST new request

**Firebase Path:**

```
PUT /ride_requests/{request_id}.json
```

**Request Body:**

```json
{
  "id": "req_1731582000123",
  "user_id": "user_1",
  "pickup_block": "cuet_campus",
  "dropoff_block": "pahartali",
  "distance_km": 2.5,
  "estimated_fare": 40,
  "estimated_points": 10,
  "privilege_verified": true,
  "status": "pending",
  "timestamp": 1731582000123,
  "rejected_by": [],
  "assigned_rickshaw": null,
  "led_status": "waiting"
}
```

**Generate Request ID:**

```
request_id = "req_" + String(millis())
```

---

### 4. Listen to LED Status Changes

**Operation:** Real-time listener (Stream)

**Firebase Path:**

```
GET /ride_requests/{request_id}/led_status.json (streaming)
```

**How to Listen:**

- Use Firebase stream/listener function
- Update LED immediately on value change
- Timeout after 60 seconds

**Possible Values:**

- `waiting` → Yellow solid
- `offer_incoming` → Yellow blink
- `pickup_confirmed` → Green solid
- `rejected` → Red solid
- `expired` → Red blink

---

### 5. Update Station Status

**Operation:** Mark station as busy/free

**Firebase Path:**

```
PATCH /location_blocks/{your_block_id}.json
```

**Request Body:**

```json
{
  "busy": true,
  "total_requests": 1
}
```

**When to Update:**

- Set `busy: true` when request created
- Set `busy: false` when request completes/expires
- Increment `total_requests` counter

---

## 📝 Example Data Formats

### Complete Request Creation Sequence

**Step 1: User Data (from scan)**

```json
{
  "laser_id": "LASER_001",
  "user_id": "user_1",
  "privilege_verified": true
}
```

**Step 2: Route Data (from button)**

```json
{
  "pickup_block": "cuet_campus",
  "dropoff_block": "pahartali",
  "distance_km": 2.5,
  "base_fare": 40,
  "points": 10
}
```

**Step 3: Create Request (combine + send)**

```json
{
  "id": "req_1731582000123",
  "user_id": "user_1",
  "pickup_block": "cuet_campus",
  "dropoff_block": "pahartali",
  "distance_km": 2.5,
  "estimated_fare": 40,
  "estimated_points": 10,
  "privilege_verified": true,
  "status": "pending",
  "timestamp": 1731582000123,
  "rejected_by": [],
  "assigned_rickshaw": null,
  "led_status": "waiting"
}
```

---

## 🧪 Testing & Validation

### Test Users (Use These for Testing)

| Laser ID    | User ID  | Name         | Type           | Verified |
| ----------- | -------- | ------------ | -------------- | -------- |
| `LASER_001` | `user_1` | Ahmed Hassan | senior_citizen | ✅ Yes   |
| `LASER_002` | `user_2` | Fatima Begum | senior_citizen | ✅ Yes   |
| `LASER_003` | `user_3` | Rahim Khan   | special_needs  | ✅ Yes   |
| `LASER_004` | `user_4` | Salma Khatun | senior_citizen | ✅ Yes   |
| `LASER_005` | `user_5` | Kamal Uddin  | senior_citizen | ✅ Yes   |

### Test Scenarios

#### Scenario 1: Successful Ride Request

1. Scan `LASER_001`
2. Select destination: Pahartali (Button 1)
3. Verify: Request created in Firebase
4. Check: LED shows Yellow
5. Wait: For web dashboard to accept
6. Verify: LED changes to Yellow blink → Green

#### Scenario 2: Invalid User

1. Scan unknown Laser ID: `LASER_999`
2. Verify: No user found
3. Check: Error display/sound
4. Verify: No request created

#### Scenario 3: Request Timeout

1. Create valid request
2. Wait 60 seconds (no acceptance)
3. Verify: LED changes to Red blink
4. Check: Request status = "expired"

---

## 🔍 Important Notes

### Timing Requirements

- **Request Timeout:** 60 seconds
- **LED Update Latency:** <2 seconds
- **User Scan to Request:** <3 seconds
- **WiFi Reconnect:** Auto-retry every 5 seconds

### Error Handling

**1. No WiFi Connection**

- Store requests in local queue (SPIFFS/EEPROM)
- Retry when connection restored
- Show offline indicator

**2. Firebase Write Failure**

- Retry up to 3 times
- Show error LED (Red blink)
- Log error to Serial

**3. Invalid Laser ID**

- Play error sound/beep
- Show message on LCD
- Do NOT create request

**4. Duplicate Request Prevention**

- Check if `busy == true` before creating
- Wait for previous request to complete
- Block rapid button presses (debounce)

### Security Considerations

1. **Validate all user inputs**
2. **Check privilege_verified before request**
3. **Rate limit:** Max 1 request per 60 seconds
4. **Sanitize laser_id** (alphanumeric only)
5. **Use HTTPS** (Firebase handles this)

---

## 📊 Data Field Requirements

### Mandatory Fields (MUST Include)

✅ **Required in ALL requests:**

```
id                  (string)
user_id             (string)
pickup_block        (string)
dropoff_block       (string)
distance_km         (number)
estimated_fare      (number)
estimated_points    (number)
privilege_verified  (boolean)
status              (string)
timestamp           (number)
rejected_by         (array)
assigned_rickshaw   (null or string)
led_status          (string)
```

### Optional Fields (Nice to Have)

```
user_name           (string)
station_hardware_id (string)
firmware_version    (string)
signal_strength     (number)
```

---

## 🎯 Success Checklist

### Hardware Module is Complete When:

- [ ] Can scan Laser ID cards
- [ ] Verifies users in Firebase
- [ ] Creates proper request structure
- [ ] LED responds to status changes (<2s)
- [ ] Handles 4 destination buttons
- [ ] Shows user feedback (LCD/LED)
- [ ] Auto-expires requests after 60s
- [ ] Handles WiFi disconnection gracefully
- [ ] Prevents duplicate requests
- [ ] All test scenarios pass

---

## 🚨 Critical Gotchas

### Common Mistakes to Avoid

1. **❌ Timestamp in seconds** → ✅ Use milliseconds
2. **❌ Missing led_status field** → ✅ Always include
3. **❌ String timestamp** → ✅ Use number
4. **❌ Empty rejected_by** → ✅ Use empty array `[]`
5. **❌ null in JSON** → ✅ Use `null` not string `"null"`
6. **❌ Wrong route format** → ✅ Use `{from}_to_{to}`
7. **❌ Blocking LED updates** → ✅ Use non-blocking listeners
8. **❌ No WiFi retry** → ✅ Implement auto-reconnect

---

## 📞 Support Resources

### Firebase Console

- **URL:** https://console.firebase.google.com/project/rickshaw-ride-c5683
- **Section:** Realtime Database → Data
- **Use:** View/verify data in real-time

### Database JSON Import

- **File:** `initial_database.json` (project root)
- **Contains:** Test users, routes, sample data

### Web Dashboard

- **URL:** http://localhost:5173 (after npm run dev)
- **Use:** Test ride acceptance flow
- **Tests:** Auto-test runner validates system

---

## 📋 Quick Reference Card

### Station IDs (Pickup Blocks)

```
cuet_campus → CUET Campus
pahartali   → Pahartali
noapara     → Noapara
raojan      → Raojan
```

### Button Mapping (Example)

```
Button 1 → Pahartali
Button 2 → Noapara
Button 3 → Raojan
Button 4 → CUET Campus
```

### LED Color Codes

```
🟡 Yellow Solid → Request pending
🟡 Yellow Blink → Rickshaw coming
🟢 Green Solid  → Rickshaw arrived
🔴 Red Solid    → Request rejected
🔴 Red Blink 3x → Request expired
```

### HTTP Methods

```
GET    → Read data
PUT    → Create/replace entire object
PATCH  → Update specific fields
DELETE → Remove data (rarely needed)
```

---

**Version:** 1.0  
**Last Updated:** November 14, 2025  
**System:** AERAS - IOTrix Competition  
**Status:** Ready for Hardware Development
