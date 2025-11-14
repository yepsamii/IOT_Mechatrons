# ✅ Manual Tasks Checklist

## Step-by-Step Guide for Setting Up the Smart Rickshaw System

> **⏱️ Estimated Time**: 30-45 minutes  
> **📋 Prerequisites**: Computer with internet, web browser, text editor

---

## 🔥 Part 1: Firebase Setup (20 minutes)

### Task 1.1: Create Firebase Account & Project

- [ ] Go to https://console.firebase.google.com/
- [ ] Sign in with Google account
- [ ] Click **"Add project"** or **"Create a project"**
- [ ] Enter project name: `smart-rickshaw-system` (or your choice)
- [ ] Enable Google Analytics: **Yes** (recommended)
- [ ] Select Analytics location: **Bangladesh** or closest
- [ ] Click **"Create project"**
- [ ] Wait for project creation (~30 seconds)
- [ ] Click **"Continue"** when ready

**✅ Checkpoint**: You should see the Firebase project dashboard

---

### Task 1.2: Enable Realtime Database

- [ ] In left sidebar, click **"Build"** → **"Realtime Database"**
- [ ] Click **"Create Database"** button
- [ ] Choose location: **Singapore (asia-southeast1)** or closest to Bangladesh
- [ ] Security rules: Select **"Start in test mode"**
- [ ] Click **"Enable"**
- [ ] Wait for database creation (~15 seconds)

**✅ Checkpoint**: You should see an empty database with a URL like:  
`https://your-project-name-default-rtdb.firebaseio.com/`

**📝 Note**: Copy this database URL - you'll need it later!

---

### Task 1.3: Configure Security Rules

- [ ] In Realtime Database page, click **"Rules"** tab
- [ ] Replace existing rules with:

```json
{
  "rules": {
    ".read": true,
    ".write": true,
    "stations": {
      ".indexOn": ["status", "name"]
    },
    "rickshaws": {
      ".indexOn": ["status", "current_station"]
    },
    "rides": {
      ".indexOn": ["status", "rickshaw_id", "timestamp"]
    },
    "ride_requests": {
      ".indexOn": ["status", "timestamp", "pickup_station"]
    }
  }
}
```

- [ ] Click **"Publish"**
- [ ] Confirm when prompted

**✅ Checkpoint**: Rules should show as "Published" with timestamp

**⚠️ Important**: These rules allow public access for testing. For production, add authentication!

---

### Task 1.4: Import Initial Database

- [ ] In Realtime Database page, click **"Data"** tab
- [ ] At the database root, click the **⋮** (three dots) menu
- [ ] Select **"Import JSON"**
- [ ] Click **"Browse"** and select `initial_database.json` from your project folder
- [ ] Click **"Import"**
- [ ] Wait for import to complete (~5 seconds)

**✅ Checkpoint**: You should now see these nodes in your database:

- `stations` (3 entries)
- `users` (2 entries)
- `rickshaws` (2 entries)
- `fare_matrix` (6 entries)
- `system_config` (1 entry)
- `rides` (empty)
- `ride_requests` (empty)

---

### Task 1.5: Get Firebase Configuration

- [ ] Click the **⚙️ Settings** (gear icon) in top left
- [ ] Select **"Project settings"**
- [ ] Scroll down to **"Your apps"** section
- [ ] If no web app exists, click the **Web icon** (</>)
  - [ ] App nickname: `rickshaw-dashboard`
  - [ ] Check **"Also set up Firebase Hosting"** (optional)
  - [ ] Click **"Register app"**
- [ ] Copy the entire `firebaseConfig` object
- [ ] **IMPORTANT**: Save this configuration in a safe place!

**Your config should look like this:**

```javascript
const firebaseConfig = {
  apiKey: "AIzaSy...",
  authDomain: "your-project.firebaseapp.com",
  databaseURL: "https://your-project-default-rtdb.firebaseio.com",
  projectId: "your-project-id",
  storageBucket: "your-project.appspot.com",
  messagingSenderId: "123456789",
  appId: "1:123456789:web:abc123",
};
```

**✅ Checkpoint**: You have copied your Firebase configuration

---

## 💻 Part 2: Web App Configuration (10 minutes)

### Task 2.1: Update Firebase Configuration

- [ ] Open your project folder in a text editor (VS Code recommended)
- [ ] Navigate to `web-app/firebase-config.js`
- [ ] Replace the placeholder configuration with your actual Firebase config:

```javascript
const firebaseConfig = {
  apiKey: "PASTE_YOUR_ACTUAL_API_KEY_HERE",
  authDomain: "PASTE_YOUR_ACTUAL_AUTH_DOMAIN_HERE",
  databaseURL: "PASTE_YOUR_ACTUAL_DATABASE_URL_HERE",
  projectId: "PASTE_YOUR_ACTUAL_PROJECT_ID_HERE",
  storageBucket: "PASTE_YOUR_ACTUAL_STORAGE_BUCKET_HERE",
  messagingSenderId: "PASTE_YOUR_ACTUAL_SENDER_ID_HERE",
  appId: "PASTE_YOUR_ACTUAL_APP_ID_HERE",
};
```

- [ ] Save the file (Ctrl+S or Cmd+S)

**✅ Checkpoint**: Your `firebase-config.js` has real Firebase credentials (no placeholders)

---

### Task 2.2: Test Web App

- [ ] Open `web-app/index.html` in your web browser
  - **Windows**: Double-click the file or right-click → Open with → Chrome/Firefox
  - **Mac**: Double-click or right-click → Open With → Chrome/Safari
- [ ] Check the footer for connection status
- [ ] It should show: **"🟢 Connected"**

**✅ Checkpoint**: Dashboard loads successfully and shows "Connected" status

**If it shows "Disconnected":**

- Check `firebase-config.js` has correct credentials
- Open browser console (F12) and check for errors
- Verify Realtime Database is enabled in Firebase

---

### Task 2.3: Select Rickshaw

- [ ] At the top of the dashboard, click the rickshaw dropdown
- [ ] Select **"Karim Ahmed (DH-RICK-001)"**
- [ ] Statistics should update:
  - Total Points: 0
  - Total Rides: 0
  - Total Earnings: ৳0
  - Rating: 5.0
- [ ] Status indicator should show **"Available"** with green dot

**✅ Checkpoint**: Dashboard shows rickshaw data and status is "Available"

---

## 🧪 Part 3: Testing the System (15 minutes)

### Task 3.1: Create Test Ride Request (Manual)

- [ ] Go back to Firebase Console
- [ ] Open **Realtime Database** → **Data** tab
- [ ] Click on **`ride_requests`** node
- [ ] Click the **+** button to add new child
- [ ] Name: `test_request_1`
- [ ] Add this JSON data:

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

- [ ] Click **"Add"**
- [ ] Go back to your web dashboard (should auto-update)
- [ ] The request should appear in "Incoming Ride Requests" section

**✅ Checkpoint**: Ride request appears on dashboard within 1-2 seconds

---

### Task 3.2: Accept Ride Request

- [ ] On the web dashboard, find the pending request
- [ ] Click **"Accept Ride"** button (green button)
- [ ] The request should move to **"Active Ride"** section
- [ ] Check Firebase Database:
  - [ ] `ride_requests/test_request_1/status` should be `"accepted"`
  - [ ] A new ride should appear in `rides` node
  - [ ] `rickshaws/rickshaw_1/status` should be `"busy"`

**✅ Checkpoint**: Active ride appears on dashboard, status changed to "On Ride"

---

### Task 3.3: Confirm Pickup

- [ ] In the "Active Ride" section, click **"Confirm Pickup"** button
- [ ] Pickup should be confirmed
- [ ] Check Firebase Database:
  - [ ] Ride status should change to `"in_progress"`
  - [ ] `pickup_time` should be set

**✅ Checkpoint**: Button changes to "Confirm Drop-off"

---

### Task 3.4: Confirm Drop-off

- [ ] Click **"Confirm Drop-off"** button
- [ ] A completion summary should appear
- [ ] Check statistics:
  - [ ] Total Points: 8 (increased)
  - [ ] Total Rides: 1 (increased)
  - [ ] Total Earnings: ৳40 (increased)
- [ ] Status should return to **"Available"**
- [ ] Check Firebase Database:
  - [ ] Ride status should be `"completed"`
  - [ ] `rickshaws/rickshaw_1/total_points` should be 8
  - [ ] `rickshaws/rickshaw_1/total_rides` should be 1

**✅ Checkpoint**: Ride completed, statistics updated, status back to "Available"

---

### Task 3.5: View Ride History

- [ ] Scroll down to **"Ride History"** section
- [ ] Click **"Refresh"** button
- [ ] Your completed ride should appear
- [ ] Verify details:
  - From: Station A
  - To: Station B
  - Distance: 2.5 km
  - Fare: ৳40
  - Points: +8 pts

**✅ Checkpoint**: Ride history shows completed ride with correct details

---

## 🔧 Part 4: Additional Testing (Optional)

### Task 4.1: Test with Multiple Requests

- [ ] Create 2-3 more ride requests using different stations
- [ ] Try accepting and rejecting different requests
- [ ] Complete multiple rides and check statistics accumulation

### Task 4.2: Test with Second Rickshaw

- [ ] Select **"Rahim Khan (DH-RICK-002)"** from dropdown
- [ ] Create a new ride request
- [ ] Accept and complete a ride
- [ ] Verify separate statistics for each rickshaw

### Task 4.3: Stress Test Real-time Updates

- [ ] Open dashboard in two browser tabs
- [ ] Select the same rickshaw in both
- [ ] Make changes in Firebase Database directly
- [ ] Verify both tabs update in real-time

---

## 📝 Part 5: Documentation Checklist

### Task 5.1: Take Screenshots

- [ ] Dashboard with statistics
- [ ] Incoming ride requests
- [ ] Active ride screen
- [ ] Ride history
- [ ] Firebase database structure
- [ ] Firebase security rules

### Task 5.2: Prepare Video Demo (5 minutes)

- [ ] Open screen recording software
- [ ] Show Firebase database structure
- [ ] Demonstrate web dashboard features
- [ ] Create ride request and complete full cycle
- [ ] Show statistics update
- [ ] Record voice explanation

### Task 5.3: Write Documentation PDF

- [ ] System architecture diagram
- [ ] Database schema explanation
- [ ] Web dashboard features
- [ ] Testing results
- [ ] Challenges faced and solutions

---

## 🎯 Final Checklist Before Submission

### Firebase

- [ ] Realtime Database is enabled and populated
- [ ] Security rules are configured
- [ ] Database has sample data (stations, rickshaws, users)

### Web Application

- [ ] Firebase credentials are correctly configured
- [ ] Dashboard connects successfully
- [ ] All features work (accept, reject, confirm)
- [ ] Statistics update correctly
- [ ] Responsive design works on mobile

### Testing

- [ ] Created test ride requests
- [ ] Accepted and completed rides
- [ ] Verified real-time synchronization
- [ ] Tested with multiple rickshaws
- [ ] All test cases pass

### Documentation

- [ ] Screenshots taken
- [ ] Video demo recorded (max 5 minutes)
- [ ] PDF report prepared
- [ ] GitHub repository ready
- [ ] README is complete

---

## ⏱️ Time Tracking

| Task           | Estimated Time | Your Time    |
| -------------- | -------------- | ------------ |
| Firebase Setup | 20 min         | \_\_\_\_ min |
| Web App Config | 10 min         | \_\_\_\_ min |
| System Testing | 15 min         | \_\_\_\_ min |
| Documentation  | 30 min         | \_\_\_\_ min |
| **Total**      | **75 min**     | \_\_\_\_ min |

---

## 🆘 Quick Troubleshooting

### Problem: Dashboard shows "Disconnected"

**Solution**: Check firebase-config.js has correct credentials, verify database is enabled

### Problem: No rickshaws in dropdown

**Solution**: Import initial_database.json to Firebase, refresh page

### Problem: Requests don't appear

**Solution**: Ensure status is "pending", select a rickshaw first

### Problem: Statistics don't update

**Solution**: Check Firebase rules allow write access, verify rickshaw is selected

### Problem: "Permission Denied" error

**Solution**: Set Firebase rules to test mode (read/write: true)

---

## 📞 Need Help?

- **Firebase Issues**: Check [Firebase Documentation](https://firebase.google.com/docs)
- **Competition Support**: Email eteteleverse@gmail.com
- **Technical Questions**: Post in IOTrix Discussion Forum

---

## ✅ Completion Status

- [ ] All Firebase tasks completed
- [ ] Web app configured and tested
- [ ] All test cases passed
- [ ] Documentation prepared
- [ ] Ready for submission

**Date Completed**: ******\_\_\_******

**Tested By**: ******\_\_\_******

**Submission Status**: [ ] Not Started [ ] In Progress [ ] Ready [ ] Submitted

---

**🎉 Congratulations! You're ready for Phase 1 submission!**

_Good luck with the competition!_ 🚀
