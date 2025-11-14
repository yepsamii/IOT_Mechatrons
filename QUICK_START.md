# ⚡ Quick Start Guide - Smart Rickshaw System

> **🎯 Goal**: Get the system running in 30 minutes!

---

## 📦 What You'll Need

- ✅ Computer with web browser
- ✅ Internet connection
- ✅ Node.js installed (v16 or higher)
- ✅ Google account (for Firebase)
- ✅ Text editor (VS Code, Sublime, etc.)
- ✅ Project files (this folder)

---

## 🚀 3-Step Setup

### Step 1: Firebase (15 minutes)

#### 1.1 Create Project

```
1. Go to: https://console.firebase.google.com/
2. Click "Add project"
3. Name: smart-rickshaw-system
4. Enable Analytics: Yes
5. Click "Create project"
```

#### 1.2 Enable Database

```
1. Left menu → "Realtime Database"
2. Click "Create Database"
3. Location: Singapore (asia-southeast1)
4. Rules: "Test mode"
5. Click "Enable"
```

#### 1.3 Import Data

```
1. In Database, click ⋮ menu
2. Select "Import JSON"
3. Choose: initial_database.json
4. Click "Import"
```

#### 1.4 Get Config

```
1. Click ⚙️ Settings → "Project settings"
2. Scroll to "Your apps"
3. Click Web icon (</>)
4. Nickname: rickshaw-dashboard
5. Copy the firebaseConfig object
```

---

### Step 2: Setup React App (10 minutes)

#### 2.1 Install Dependencies

```bash
1. Open terminal
2. Navigate to: cd web-app
3. Run: npm install
4. Wait for installation to complete (2-3 minutes)
```

#### 2.2 Update Firebase Config

```
1. Open: web-app/src/firebase.js
2. Replace placeholders with your Firebase config
3. Save file
```

**Your config looks like:**

```javascript
const firebaseConfig = {
  apiKey: "AIza...", // ← Paste yours
  authDomain: "your-project.firebaseapp.com",
  databaseURL: "https://your-project-default-rtdb.firebaseio.com",
  projectId: "your-project-id",
  storageBucket: "your-project.appspot.com",
  messagingSenderId: "123456789",
  appId: "1:123456789:web:abc123",
};
```

---

### Step 3: Run & Test (10 minutes)

#### 3.1 Start Development Server

```bash
1. In terminal (inside web-app folder): npm run dev
2. App opens at: http://localhost:3000
3. Check footer: Should show "🟢 Connected"
4. Select rickshaw: "Karim Ahmed (DH-RICK-001)"
```

#### 3.2 Create Test Request

```
1. Go to Firebase Console → Database
2. Click "ride_requests" node
3. Click "+" button
4. Name: test_request_1
5. Paste this JSON:
```

```json
{
  "id": "test_request_1",
  "user_id": "user_1",
  "pickup_station": "station_1",
  "dropoff_station": "station_2",
  "distance_km": 2.5,
  "estimated_fare": 40,
  "estimated_points": 8,
  "privilege_verified": true,
  "status": "pending",
  "timestamp": "2025-11-14T12:00:00Z"
}
```

```
6. Click "Add"
7. Check dashboard → Request should appear!
```

#### 3.3 Complete a Ride

```
1. Click "Accept Ride" (green button)
2. Click "Confirm Pickup"
3. Click "Confirm Drop-off"
4. Check statistics updated:
   - Points: 8
   - Rides: 1
   - Earnings: ৳40
```

---

## ✅ Success Checklist

- [ ] Node.js installed
- [ ] Firebase project created
- [ ] Database enabled and populated
- [ ] Dependencies installed (npm install)
- [ ] Development server running (npm run dev)
- [ ] Web app shows "Connected"
- [ ] Rickshaw selected from dropdown
- [ ] Test request appears on dashboard
- [ ] Can accept and complete ride
- [ ] Statistics update correctly

---

## 🎯 What's Next?

### For Web-Only Demo:

1. ✅ Create multiple test requests
2. ✅ Test accept/reject flow
3. ✅ Complete 3-5 rides
4. ✅ Take screenshots
5. ✅ Record demo video

### For Full Hardware Demo:

1. ⏳ Build circuit on breadboard
2. ⏳ Upload ESP32 code (see hardware/station_module_esp32.ino)
3. ⏳ Test sensors (ultrasonic, LDR)
4. ⏳ Program rickshaw OLED display
5. ⏳ Test end-to-end integration

---

## 📸 Screenshot Locations

**Take these for documentation:**

1. **Firebase Database** - Show full structure
2. **Dashboard - Empty** - Before selecting rickshaw
3. **Dashboard - Selected** - With statistics
4. **Incoming Request** - Request card visible
5. **Active Ride** - Ride in progress
6. **Completed** - Statistics after ride
7. **Ride History** - History section populated

---

## 🎥 Video Demo Structure (5 minutes)

```
[0:00-0:30] Introduction
  - Project name and team
  - Problem statement

[0:30-1:30] Firebase Database
  - Show database structure
  - Explain collections
  - Show fare matrix

[1:30-3:30] Web Dashboard Demo
  - Select rickshaw
  - Show statistics
  - Create ride request
  - Accept ride
  - Confirm pickup
  - Confirm dropoff
  - Show history

[3:30-4:30] Hardware (if available)
  - Circuit diagram
  - Sensor demo
  - OLED display
  - Integration test

[4:30-5:00] Conclusion
  - Key innovations
  - Scalability
  - Thank you
```

---

## 🐛 Common Issues & Fixes

### Issue: "Disconnected" Status

**Fix**:

```
1. Check src/firebase.js has correct values
2. Open browser console (F12)
3. Look for errors
4. Verify databaseURL is correct
5. Ensure dev server is running (npm run dev)
```

### Issue: No Rickshaws in Dropdown

**Fix**:

```
1. Go to Firebase → Database
2. Check "rickshaws" node exists
3. Re-import initial_database.json
4. Refresh page
```

### Issue: Request Doesn't Appear

**Fix**:

```
1. Verify request status is "pending"
2. Check Firebase rules allow read
3. Ensure rickshaw is selected
4. Refresh page
```

---

## 📞 Need Help?

**Documentation:**

- 📖 README.md - Complete guide
- 🏗️ SYSTEM_ARCHITECTURE.md - Technical details
- 🔥 FIREBASE_SETUP_GUIDE.md - Detailed Firebase setup
- ✅ MANUAL_TASKS_CHECKLIST.md - Step-by-step checklist
- 🧪 TEST_CASES.md - All test cases

**Competition:**

- 📧 Email: eteteleverse@gmail.com
- 🌐 Website: https://eteteleverse.com

---

## 🏆 Competition Submission Checklist

### Phase 1 Requirements:

- [ ] **Code** - GitHub repository

  - [ ] Web app files
  - [ ] Hardware code (if implemented)
  - [ ] README with setup instructions

- [ ] **Documentation** - PDF Report

  - [ ] System architecture
  - [ ] Database schema
  - [ ] Circuit diagram
  - [ ] Test results
  - [ ] Screenshots

- [ ] **Video** - 5-minute demo

  - [ ] Screen recording with voice
  - [ ] Show Firebase database
  - [ ] Demonstrate web dashboard
  - [ ] Show hardware (if available)
  - [ ] Explain integration

- [ ] **Submission** - Before deadline
  - [ ] GitHub repo link
  - [ ] Google Drive link (PDF + video)
  - [ ] Submit through competition portal

---

## 🎉 You're Ready!

**Timeline:**

- ✅ Setup complete: 30 minutes
- ⏳ Testing & refinement: 2-3 hours
- ⏳ Documentation: 2-3 hours
- ⏳ Video recording: 1-2 hours
- ⏳ Buffer time: 2 hours

**Total estimated time: 8-10 hours**

---

## 💡 Pro Tips

1. **Test Early, Test Often**

   - Don't wait until last minute
   - Test each feature thoroughly

2. **Document as You Go**

   - Take screenshots during development
   - Note challenges and solutions

3. **Practice Your Demo**

   - Record video multiple times
   - Keep it under 5 minutes
   - Speak clearly and confidently

4. **Backup Everything**

   - Export Firebase database
   - Commit code to GitHub
   - Save multiple copies of video

5. **Read the Rulebook**
   - Follow all requirements
   - Meet submission format
   - Submit before deadline

---

**Good luck! 🚀**

_"Decode the Matrix, Recode the World"_

---

## 📈 Scoring Breakdown

Based on competition judging criteria:

| Criteria                | Weight | Your Strategy                    |
| ----------------------- | ------ | -------------------------------- |
| **Hardware (30%)**      | 30%    | ESP32 + sensors + real-time sync |
| **Database (20%)**      | 20%    | Well-structured Firebase schema  |
| **Web Dashboard (20%)** | 20%    | Modern UI + real-time features   |
| **Documentation (15%)** | 15%    | Comprehensive docs + video       |
| **Innovation (15%)**    | 15%    | Laser privilege + gamification   |

**Target**: 85%+ for qualification to Phase 2!

---

**Version:** 1.0  
**Last Updated:** November 14, 2025  
**Competition:** IOTrix - Televerse 1.0
