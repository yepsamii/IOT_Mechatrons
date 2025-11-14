# 🧪 Test Cases - Smart Rickshaw System

## Test Case Documentation for IOTrix Competition

---

## 📋 Test Case Summary

| Category             | Total Tests | Priority |
| -------------------- | ----------- | -------- |
| Database Operations  | 8           | High     |
| Web Dashboard        | 12          | High     |
| Real-time Sync       | 6           | High     |
| Ride Workflow        | 10          | Critical |
| Hardware Integration | 8           | Medium   |
| Edge Cases           | 6           | Medium   |
| **Total**            | **50**      | -        |

---

## 1️⃣ Database Operations (Firebase)

### TC-DB-01: Create Stations

**Description**: Verify stations can be created in database  
**Priority**: High  
**Preconditions**: Firebase Realtime Database enabled

**Steps**:

1. Open Firebase Console → Realtime Database
2. Navigate to `/stations` node
3. Add new station with complete data structure
4. Verify station is saved

**Expected Result**:

- Station created successfully
- All fields (id, name, location, status) present
- Timestamp generated correctly

**Status**: [ ] Pass [ ] Fail

---

### TC-DB-02: Create Rickshaw

**Description**: Verify rickshaw entry creation  
**Priority**: High

**Steps**:

1. Navigate to `/rickshaws` node
2. Add new rickshaw with required fields
3. Verify default values (total_points=0, status=available)

**Expected Result**:

- Rickshaw created with correct structure
- Default values applied
- Ready to receive rides

**Status**: [ ] Pass [ ] Fail

---

### TC-DB-03: Fare Matrix Lookup

**Description**: Verify distance-based fare calculation  
**Priority**: High

**Steps**:

1. Query `/fare_matrix/station_1_to_station_2`
2. Verify distance, fare, and points data
3. Test reverse route lookup

**Expected Result**:

- Correct fare returned for route
- Distance accurate
- Points calculated correctly (distance × points_per_km)

**Status**: [ ] Pass [ ] Fail

---

### TC-DB-04: Security Rules

**Description**: Verify database read/write permissions  
**Priority**: High

**Steps**:

1. Attempt read from unauthenticated client
2. Attempt write from unauthenticated client
3. Verify test mode allows both

**Expected Result**:

- Read operations succeed
- Write operations succeed (test mode)
- Indexed fields optimize queries

**Status**: [ ] Pass [ ] Fail

---

### TC-DB-05: Real-time Listeners

**Description**: Verify Firebase real-time event listeners  
**Priority**: High

**Steps**:

1. Set up listener on `/ride_requests`
2. Add new request from console
3. Verify listener triggers immediately

**Expected Result**:

- Listener receives 'value' event
- Data synchronized within 1-2 seconds
- No connection errors

**Status**: [ ] Pass [ ] Fail

---

### TC-DB-06: Query with Filters

**Description**: Test indexed queries  
**Priority**: Medium

**Steps**:

1. Query rides by status: `orderByChild('status').equalTo('pending')`
2. Query rickshaws by current_station
3. Verify results are filtered correctly

**Expected Result**:

- Only matching records returned
- Query executes quickly (<500ms)
- No full table scan

**Status**: [ ] Pass [ ] Fail

---

### TC-DB-07: Atomic Updates

**Description**: Verify multi-path atomic updates  
**Priority**: High

**Steps**:

1. Prepare updates object with multiple paths
2. Execute `database.ref().update(updates)`
3. Verify all updates succeed or all fail

**Expected Result**:

- All paths updated simultaneously
- No partial updates
- Transaction consistency maintained

**Status**: [ ] Pass [ ] Fail

---

### TC-DB-08: Connection Status

**Description**: Monitor Firebase connection state  
**Priority**: Medium

**Steps**:

1. Listen to `.info/connected`
2. Disable network
3. Re-enable network
4. Verify connection state updates

**Expected Result**:

- Connection status updates correctly
- Offline detection works
- Auto-reconnection on network restore

**Status**: [ ] Pass [ ] Fail

---

## 2️⃣ Web Dashboard Tests

### TC-WEB-01: Dashboard Load

**Description**: Verify dashboard loads successfully  
**Priority**: Critical

**Steps**:

1. Open `web-app/index.html` in browser
2. Check for JavaScript errors (F12 console)
3. Verify Firebase connection status

**Expected Result**:

- Page loads without errors
- Firebase SDK initialized
- Connection status shows "Connected"

**Status**: [ ] Pass [ ] Fail

---

### TC-WEB-02: Rickshaw Selection

**Description**: Test rickshaw dropdown and selection  
**Priority**: Critical

**Steps**:

1. Click rickshaw dropdown
2. Verify rickshaws populate from Firebase
3. Select a rickshaw
4. Check statistics update

**Expected Result**:

- Dropdown populated with rickshaws
- Selection triggers data load
- Statistics display correctly

**Status**: [ ] Pass [ ] Fail

---

### TC-WEB-03: Statistics Display

**Description**: Verify statistics cards show correct data  
**Priority**: High

**Steps**:

1. Select rickshaw with known data
2. Check Total Points
3. Check Total Rides
4. Check Total Earnings
5. Check Rating

**Expected Result**:

- All statistics match database values
- Numbers formatted correctly
- Updates reflect real-time changes

**Status**: [ ] Pass [ ] Fail

---

### TC-WEB-04: Status Indicator

**Description**: Test status dot and text  
**Priority**: Medium

**Steps**:

1. Select available rickshaw
2. Verify status shows "Available" with green dot
3. Accept a ride
4. Verify status changes to "On Ride" with yellow dot

**Expected Result**:

- Status reflects rickshaw state accurately
- Color coding correct
- Updates in real-time

**Status**: [ ] Pass [ ] Fail

---

### TC-WEB-05: Incoming Requests Display

**Description**: Verify ride requests appear in dashboard  
**Priority**: Critical

**Steps**:

1. Create pending ride request in Firebase
2. Verify request card appears on dashboard
3. Check all request details displayed
4. Verify "time ago" calculation

**Expected Result**:

- Request appears within 2 seconds
- All fields visible (pickup, dropoff, distance, fare, points)
- Layout is clean and readable

**Status**: [ ] Pass [ ] Fail

---

### TC-WEB-06: Accept Ride Button

**Description**: Test accepting ride request  
**Priority**: Critical

**Steps**:

1. Click "Accept Ride" on pending request
2. Verify request moves to active ride section
3. Check database updates
4. Verify rickshaw status changes to "busy"

**Expected Result**:

- Request accepted successfully
- UI updates immediately
- Database reflects changes
- Active ride section displays

**Status**: [ ] Pass [ ] Fail

---

### TC-WEB-07: Reject Ride Button

**Description**: Test rejecting ride request  
**Priority**: High

**Steps**:

1. Click "Reject" on pending request
2. Verify request status changes to "rejected"
3. Check request disappears from pending list

**Expected Result**:

- Request rejected successfully
- Removed from pending list
- Database updated
- Rickshaw remains available

**Status**: [ ] Pass [ ] Fail

---

### TC-WEB-08: Active Ride Display

**Description**: Verify active ride section shows correctly  
**Priority**: Critical

**Steps**:

1. Accept a ride request
2. Check active ride section appears
3. Verify route display (from → to)
4. Check ride details (distance, fare, points)

**Expected Result**:

- Active ride section visible
- Route displayed with arrow
- All details correct
- "Confirm Pickup" button visible

**Status**: [ ] Pass [ ] Fail

---

### TC-WEB-09: Confirm Pickup Button

**Description**: Test pickup confirmation  
**Priority**: Critical

**Steps**:

1. In active ride, click "Confirm Pickup"
2. Verify ride status changes to "in_progress"
3. Check pickup_time is set
4. Verify button changes to "Confirm Drop-off"

**Expected Result**:

- Pickup confirmed
- Timestamp recorded
- Button text updates
- Database updated

**Status**: [ ] Pass [ ] Fail

---

### TC-WEB-10: Confirm Drop-off Button

**Description**: Test drop-off confirmation and ride completion  
**Priority**: Critical

**Steps**:

1. Click "Confirm Drop-off"
2. Verify completion alert appears
3. Check points added to rickshaw
4. Verify total_rides incremented
5. Check status returns to "available"

**Expected Result**:

- Drop-off confirmed
- Points credited (+8 for 2.5km ride)
- Total rides +1
- Status back to "Available"
- Ride moves to history

**Status**: [ ] Pass [ ] Fail

---

### TC-WEB-11: Ride History Display

**Description**: Verify completed rides in history section  
**Priority**: High

**Steps**:

1. Complete a ride
2. Click "Refresh" in history section
3. Verify ride appears in list
4. Check all details displayed

**Expected Result**:

- Completed ride visible
- Details accurate (route, distance, fare, points)
- Sorted by completion time (newest first)
- Empty state shown when no history

**Status**: [ ] Pass [ ] Fail

---

### TC-WEB-12: Responsive Design

**Description**: Test mobile responsiveness  
**Priority**: Medium

**Steps**:

1. Open dashboard in mobile view (375px width)
2. Test all features
3. Verify layout adjusts correctly

**Expected Result**:

- Layout adapts to mobile screen
- All buttons accessible
- Text readable
- No horizontal scroll

**Status**: [ ] Pass [ ] Fail

---

## 3️⃣ Real-time Synchronization Tests

### TC-SYNC-01: Multi-tab Sync

**Description**: Test real-time sync across browser tabs  
**Priority**: High

**Steps**:

1. Open dashboard in two browser tabs
2. Select same rickshaw in both
3. Accept ride in tab 1
4. Verify tab 2 updates automatically

**Expected Result**:

- Both tabs show identical data
- Updates propagate within 1-2 seconds
- No conflicts or stale data

**Status**: [ ] Pass [ ] Fail

---

### TC-SYNC-02: Database Direct Edit

**Description**: Verify dashboard updates when Firebase edited directly  
**Priority**: High

**Steps**:

1. Open dashboard with selected rickshaw
2. Edit rickshaw data in Firebase Console
3. Verify dashboard updates automatically

**Expected Result**:

- Changes reflected immediately
- No page refresh needed
- Statistics update correctly

**Status**: [ ] Pass [ ] Fail

---

### TC-SYNC-03: New Request Notification

**Description**: Test notification when new request arrives  
**Priority**: High

**Steps**:

1. Dashboard open with rickshaw selected
2. Create new ride request in Firebase
3. Verify request appears immediately

**Expected Result**:

- Request card appears within 2 seconds
- Request count badge updates
- Console logs notification

**Status**: [ ] Pass [ ] Fail

---

### TC-SYNC-04: Concurrent Requests

**Description**: Test multiple simultaneous requests  
**Priority**: Medium

**Steps**:

1. Create 3 ride requests simultaneously
2. Verify all appear on dashboard
3. Accept one, verify others remain

**Expected Result**:

- All requests displayed
- Correct count shown
- Accepting one doesn't affect others

**Status**: [ ] Pass [ ] Fail

---

### TC-SYNC-05: Connection Loss Recovery

**Description**: Test offline behavior and recovery  
**Priority**: Medium

**Steps**:

1. Dashboard connected
2. Disable network
3. Verify "Disconnected" status shown
4. Re-enable network
5. Verify auto-reconnection

**Expected Result**:

- Offline status detected
- UI shows disconnected state
- Auto-reconnects when online
- Data re-syncs automatically

**Status**: [ ] Pass [ ] Fail

---

### TC-SYNC-06: Ride State Transitions

**Description**: Verify all ride state changes sync correctly  
**Priority**: Critical

**Steps**:

1. Monitor ride through full lifecycle
2. pending → accepted → in_progress → completed
3. Verify each transition syncs

**Expected Result**:

- All state changes reflected in real-time
- Database and UI always consistent
- No stale states

**Status**: [ ] Pass [ ] Fail

---

## 4️⃣ Complete Ride Workflow Tests

### TC-FLOW-01: End-to-End Ride (Happy Path)

**Description**: Complete ride from request to completion  
**Priority**: Critical

**Test Flow**:

```
Create Request → Accept → Confirm Pickup → Confirm Drop-off → View History
```

**Steps**:

1. Create ride request: station_1 → station_2
2. Accept request
3. Confirm pickup
4. Confirm drop-off
5. Verify in history

**Expected Result**:

- Complete workflow executes smoothly
- Points earned: 8
- Fare collected: ৳40
- Total rides: +1
- Ride in history

**Status**: [ ] Pass [ ] Fail

---

### TC-FLOW-02: Multiple Sequential Rides

**Description**: Complete 3 rides in sequence  
**Priority**: High

**Steps**:

1. Complete ride 1 (station_1 → station_2)
2. Complete ride 2 (station_2 → station_3)
3. Complete ride 3 (station_3 → station_1)
4. Verify cumulative statistics

**Expected Result**:

- Total points: 24 (8+6+10)
- Total rides: 3
- All rides in history
- Status returns to available after each

**Status**: [ ] Pass [ ] Fail

---

### TC-FLOW-03: Reject Then Accept Different

**Description**: Reject one request, accept another  
**Priority**: Medium

**Steps**:

1. Create two ride requests
2. Reject first request
3. Accept second request
4. Complete second ride

**Expected Result**:

- First request marked rejected
- Second request accepted
- Only accepted ride credited
- No interference between requests

**Status**: [ ] Pass [ ] Fail

---

### TC-FLOW-04: Switch Rickshaws Mid-Session

**Description**: Test switching between rickshaws  
**Priority**: Medium

**Steps**:

1. Select rickshaw_1
2. Complete one ride
3. Select rickshaw_2
4. Complete another ride
5. Switch back to rickshaw_1

**Expected Result**:

- Each rickshaw tracks own statistics
- No data leakage between rickshaws
- Dashboard updates correctly on switch

**Status**: [ ] Pass [ ] Fail

---

### TC-FLOW-05: Long Distance Ride

**Description**: Test ride with maximum distance  
**Priority**: Medium

**Steps**:

1. Create ride: station_1 → station_3 (3.5 km)
2. Accept and complete
3. Verify fare: ৳50
4. Verify points: +10

**Expected Result**:

- Long distance handled correctly
- Higher fare calculated
- More points awarded
- All data accurate

**Status**: [ ] Pass [ ] Fail

---

### TC-FLOW-06: Short Distance Ride

**Description**: Test ride with minimum distance  
**Priority**: Medium

**Steps**:

1. Create ride: station_2 → station_3 (1.5 km)
2. Accept and complete
3. Verify fare: ৳30
4. Verify points: +6

**Expected Result**:

- Short distance handled correctly
- Lower fare calculated
- Fewer points awarded
- All data accurate

**Status**: [ ] Pass [ ] Fail

---

### TC-FLOW-07: Rating Update

**Description**: Verify rating persists after rides  
**Priority**: Low

**Steps**:

1. Note initial rating (5.0)
2. Complete multiple rides
3. Verify rating remains stable
4. (Future: Test rating calculation if implemented)

**Expected Result**:

- Rating value persists
- Displayed correctly
- Updates if rating system implemented

**Status**: [ ] Pass [ ] Fail

---

### TC-FLOW-08: Earnings Calculation

**Description**: Verify total earnings accuracy  
**Priority**: High

**Steps**:

1. Complete ride earning 8 points
2. Check earnings: ৳40 (8 × 5)
3. Complete second ride earning 10 points
4. Check total: ৳90 (18 × 5)

**Expected Result**:

- Earnings = points × 5
- Cumulative calculation correct
- Display formatted with ৳ symbol

**Status**: [ ] Pass [ ] Fail

---

### TC-FLOW-09: Time Tracking

**Description**: Verify timestamps are recorded correctly  
**Priority**: Medium

**Steps**:

1. Accept ride, note accept_time
2. Confirm pickup, note pickup_time
3. Confirm drop-off, note dropoff_time
4. Verify all timestamps in ISO format

**Expected Result**:

- All timestamps recorded
- Format: YYYY-MM-DDTHH:mm:ss.sssZ
- Chronological order maintained

**Status**: [ ] Pass [ ] Fail

---

### TC-FLOW-10: Toast Notifications

**Description**: Verify user feedback messages  
**Priority**: Low

**Steps**:

1. Accept ride → verify "Ride accepted" toast
2. Confirm pickup → verify "Pickup confirmed" toast
3. Confirm dropoff → verify "Completed! +X points" toast
4. Reject request → verify "Request rejected" toast

**Expected Result**:

- Toast appears for each action
- Message accurate and helpful
- Auto-dismisses after 3 seconds

**Status**: [ ] Pass [ ] Fail

---

## 5️⃣ Hardware Integration Tests (ESP32)

### TC-HW-01: ESP32 WiFi Connection

**Description**: Verify ESP32 connects to WiFi  
**Priority**: Critical

**Steps**:

1. Upload code to ESP32
2. Open Serial Monitor
3. Verify WiFi connection established
4. Check IP address assigned

**Expected Result**:

- Connection successful
- IP address obtained
- No connection errors
- Stable connection

**Status**: [ ] Pass [ ] Fail

---

### TC-HW-02: Firebase Connection from ESP32

**Description**: Test ESP32 Firebase connectivity  
**Priority**: Critical

**Steps**:

1. Configure Firebase credentials in ESP32
2. Attempt connection
3. Verify authentication
4. Test read from database

**Expected Result**:

- Firebase connection established
- Authentication successful
- Can read data from database
- No timeout errors

**Status**: [ ] Pass [ ] Fail

---

### TC-HW-03: Ultrasonic Sensor Detection

**Description**: Test user detection at station  
**Priority**: High

**Steps**:

1. Power up station module
2. Approach sensor (< 50cm)
3. Verify detection logged
4. Move away, verify clear

**Expected Result**:

- Detection accurate within 2cm
- Triggers within 100ms
- Debounced (no false triggers)
- Readings consistent

**Status**: [ ] Pass [ ] Fail

---

### TC-HW-04: LDR Laser Detection

**Description**: Test laser privilege verification  
**Priority**: High

**Steps**:

1. Point laser at LDR
2. Verify LDR value changes
3. Test threshold detection
4. Remove laser, verify reset

**Expected Result**:

- LDR detects laser light
- Value crosses threshold
- Privilege verified
- No false positives in ambient light

**Status**: [ ] Pass [ ] Fail

---

### TC-HW-05: Button Input

**Description**: Test destination selection buttons  
**Priority**: Medium

**Steps**:

1. Press button for station_2
2. Verify input registered
3. Test all destination buttons
4. Verify debouncing works

**Expected Result**:

- Button press detected
- Correct destination selected
- No double-triggers
- Responsive (<50ms)

**Status**: [ ] Pass [ ] Fail

---

### TC-HW-06: Create Request from ESP32

**Description**: Test ride request creation from hardware  
**Priority**: Critical

**Steps**:

1. Simulate user flow (detect → verify → select)
2. ESP32 creates ride request in Firebase
3. Verify request appears in database
4. Check web dashboard updates

**Expected Result**:

- Request created successfully
- All fields populated correctly
- Timestamp accurate
- Dashboard receives request

**Status**: [ ] Pass [ ] Fail

---

### TC-HW-07: OLED Display Update

**Description**: Test rickshaw OLED display  
**Priority**: High

**Steps**:

1. Accept ride from web dashboard
2. Verify OLED updates on rickshaw module
3. Check pickup/dropoff displayed
4. Verify fare and distance shown

**Expected Result**:

- OLED updates within 2 seconds
- All information legible
- Layout fits 0.96" screen
- Updates on status changes

**Status**: [ ] Pass [ ] Fail

---

### TC-HW-08: End-to-End Hardware Integration

**Description**: Complete ride using hardware and web together  
**Priority**: Critical

**Steps**:

1. User detected at station (ultrasonic)
2. Laser verified (LDR)
3. Destination selected (button)
4. Request created (ESP32 → Firebase)
5. Accept on web dashboard
6. Confirm pickup on web
7. OLED shows ride info
8. Confirm dropoff on web
9. OLED shows completion

**Expected Result**:

- Complete integration works seamlessly
- Hardware and web sync perfectly
- All sensors function correctly
- No data loss or delays

**Status**: [ ] Pass [ ] Fail

---

## 6️⃣ Edge Cases & Error Handling

### TC-EDGE-01: No Internet Connection

**Description**: Test behavior when offline  
**Priority**: Medium

**Steps**:

1. Disconnect internet
2. Try to accept ride
3. Verify error handling
4. Reconnect and verify recovery

**Expected Result**:

- Graceful degradation
- Error message shown
- Auto-recovery on reconnection
- No data corruption

**Status**: [ ] Pass [ ] Fail

---

### TC-EDGE-02: Rapid Button Clicks

**Description**: Test UI against spam clicks  
**Priority**: Low

**Steps**:

1. Rapidly click "Accept Ride" 10 times
2. Verify only one accept processed
3. Test other buttons similarly

**Expected Result**:

- Button disabled after first click
- No duplicate requests
- No database errors
- UI remains responsive

**Status**: [ ] Pass [ ] Fail

---

### TC-EDGE-03: Empty Database

**Description**: Test dashboard with no data  
**Priority**: Medium

**Steps**:

1. Clear all rickshaws from database
2. Open dashboard
3. Verify empty state messages

**Expected Result**:

- No JavaScript errors
- "No rickshaws" message shown
- Dashboard doesn't break
- Graceful empty states

**Status**: [ ] Pass [ ] Fail

---

### TC-EDGE-04: Invalid Data Types

**Description**: Test with malformed database data  
**Priority**: Low

**Steps**:

1. Set points to string instead of number
2. Set distance_km to null
3. Verify dashboard handles gracefully

**Expected Result**:

- No crashes
- Default values used
- Error logged to console
- UI still functional

**Status**: [ ] Pass [ ] Fail

---

### TC-EDGE-05: Very Long Station Names

**Description**: Test UI with edge case text lengths  
**Priority**: Low

**Steps**:

1. Create station with 100-character name
2. Create ride request using this station
3. Verify display handles overflow

**Expected Result**:

- Text wraps or truncates gracefully
- Layout doesn't break
- Still readable
- No horizontal scroll

**Status**: [ ] Pass [ ] Fail

---

### TC-EDGE-06: Simultaneous Accepts (Race Condition)

**Description**: Test two rickshaws accepting same request  
**Priority**: Medium

**Steps**:

1. Open dashboard for rickshaw_1 and rickshaw_2
2. Create one ride request
3. Both try to accept simultaneously
4. Verify only one accepts

**Expected Result**:

- First accept wins
- Second fails gracefully
- No duplicate rides
- Proper error message

**Status**: [ ] Pass [ ] Fail

---

## 📊 Test Results Summary

### Execution Summary

| Date | Tester | Environment | Pass | Fail | Blocked | Total |
| ---- | ------ | ----------- | ---- | ---- | ------- | ----- |
|      |        |             |      |      |         | 50    |

### Test Coverage

- **Database**: \_\_\_ / 8 passed
- **Web Dashboard**: \_\_\_ / 12 passed
- **Real-time Sync**: \_\_\_ / 6 passed
- **Ride Workflow**: \_\_\_ / 10 passed
- **Hardware Integration**: \_\_\_ / 8 passed
- **Edge Cases**: \_\_\_ / 6 passed

**Overall Pass Rate**: \_\_\_\_%

---

## 🐛 Defects Log

| ID  | Test Case | Severity | Description | Status |
| --- | --------- | -------- | ----------- | ------ |
|     |           |          |             |        |

---

## ✅ Sign-off

**Tested By**: **********\_\_\_**********  
**Date**: **********\_\_\_**********  
**Status**: [ ] All Passed [ ] Passed with Issues [ ] Failed

---

**Next Steps**:

1. Fix any failed test cases
2. Retest defects
3. Update documentation
4. Prepare for Phase 1 submission

---

_Test Plan Version 1.0_  
_IOTrix - Televerse 1.0 Competition_
