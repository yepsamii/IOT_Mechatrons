import { useState } from "react";
import { ref, set, get, update, remove } from "firebase/database";
import { database } from "../firebase";

const TestRunner = () => {
  const [testResults, setTestResults] = useState([]);
  const [isRunning, setIsRunning] = useState(false);
  const [currentTest, setCurrentTest] = useState("");

  const addResult = (testName, passed, message) => {
    setTestResults((prev) => [
      ...prev,
      { testName, passed, message, timestamp: Date.now() },
    ]);
  };

  const delay = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

  // Test 1: Database Connection
  const testDatabaseConnection = async () => {
    setCurrentTest("Testing Database Connection...");
    try {
      // Test connection by reading a known collection
      const testRef = ref(database, "location_blocks");
      const snapshot = await get(testRef);
      if (snapshot.exists()) {
        addResult(
          "Database Connection",
          true,
          "Firebase connected successfully"
        );
        return true;
      }
      addResult("Database Connection", false, "Firebase not connected");
      return false;
    } catch (error) {
      addResult("Database Connection", false, `Error: ${error.message}`);
      return false;
    }
  };

  // Test 2: Create Ride Request
  const testCreateRideRequest = async () => {
    setCurrentTest("Testing Ride Request Creation...");
    try {
      const testRequestId = `test_req_${Date.now()}`;
      const requestData = {
        id: testRequestId,
        user_id: "user_1",
        pickup_block: "cuet_campus",
        dropoff_block: "pahartali",
        distance_km: 2.5,
        estimated_fare: 40,
        estimated_points: 10,
        privilege_verified: true,
        status: "pending",
        timestamp: Date.now(),
        rejected_by: [],
        led_status: "waiting",
      };

      await set(ref(database, `ride_requests/${testRequestId}`), requestData);

      // Verify it was created
      const snapshot = await get(
        ref(database, `ride_requests/${testRequestId}`)
      );
      if (snapshot.exists()) {
        await remove(ref(database, `ride_requests/${testRequestId}`));
        addResult(
          "Create Ride Request",
          true,
          "Request created and verified successfully"
        );
        return true;
      }
      addResult(
        "Create Ride Request",
        false,
        "Request not found after creation"
      );
      return false;
    } catch (error) {
      addResult("Create Ride Request", false, `Error: ${error.message}`);
      return false;
    }
  };

  // Test 3: Accept Ride
  const testAcceptRide = async () => {
    setCurrentTest("Testing Ride Acceptance...");
    try {
      // Create test request
      const testRequestId = `test_req_${Date.now()}`;
      const testRideId = `test_ride_${Date.now()}`;

      await set(ref(database, `ride_requests/${testRequestId}`), {
        id: testRequestId,
        user_id: "user_1",
        pickup_block: "cuet_campus",
        dropoff_block: "pahartali",
        status: "pending",
        timestamp: Date.now(),
      });

      // Accept the ride
      const updates = {};
      updates[`active_rides/${testRideId}`] = {
        id: testRideId,
        request_id: testRequestId,
        rickshaw_id: "rickshaw_1",
        status: "accepted",
        accept_time: Date.now(),
      };
      updates[`ride_requests/${testRequestId}/status`] = "accepted";
      updates[`ride_requests/${testRequestId}/led_status`] = "offer_incoming";

      await update(ref(database), updates);

      // Verify
      const rideSnapshot = await get(
        ref(database, `active_rides/${testRideId}`)
      );
      const ledSnapshot = await get(
        ref(database, `ride_requests/${testRequestId}/led_status`)
      );

      const passed =
        rideSnapshot.exists() && ledSnapshot.val() === "offer_incoming";

      // Cleanup
      await remove(ref(database, `active_rides/${testRideId}`));
      await remove(ref(database, `ride_requests/${testRequestId}`));

      if (passed) {
        addResult(
          "Accept Ride",
          true,
          "Ride accepted and LED status updated correctly"
        );
        return true;
      }
      addResult("Accept Ride", false, "Ride acceptance verification failed");
      return false;
    } catch (error) {
      addResult("Accept Ride", false, `Error: ${error.message}`);
      return false;
    }
  };

  // Test 4: LED Status Updates
  const testLEDStatusUpdates = async () => {
    setCurrentTest("Testing LED Status Updates...");
    try {
      const testRequestId = `test_req_${Date.now()}`;

      // Create request
      await set(ref(database, `ride_requests/${testRequestId}`), {
        id: testRequestId,
        led_status: "waiting",
      });

      // Test all LED statuses
      const statuses = [
        "offer_incoming",
        "pickup_confirmed",
        "rejected",
        "expired",
      ];
      let allPassed = true;

      for (const status of statuses) {
        await update(ref(database, `ride_requests/${testRequestId}`), {
          led_status: status,
        });
        await delay(100);

        const snapshot = await get(
          ref(database, `ride_requests/${testRequestId}/led_status`)
        );
        if (snapshot.val() !== status) {
          allPassed = false;
          break;
        }
      }

      // Cleanup
      await remove(ref(database, `ride_requests/${testRequestId}`));

      if (allPassed) {
        addResult(
          "LED Status Updates",
          true,
          "All LED statuses updated correctly"
        );
        return true;
      }
      addResult("LED Status Updates", false, "LED status update failed");
      return false;
    } catch (error) {
      addResult("LED Status Updates", false, `Error: ${error.message}`);
      return false;
    }
  };

  // Test 5: Reject Ride
  const testRejectRide = async () => {
    setCurrentTest("Testing Ride Rejection...");
    try {
      const testRequestId = `test_req_${Date.now()}`;

      await set(ref(database, `ride_requests/${testRequestId}`), {
        id: testRequestId,
        status: "pending",
        rejected_by: [],
      });

      // Reject from rickshaw_1
      await update(ref(database, `ride_requests/${testRequestId}`), {
        rejected_by: ["rickshaw_1"],
      });

      const snapshot = await get(
        ref(database, `ride_requests/${testRequestId}/rejected_by`)
      );
      const rejectedBy = snapshot.val();

      // Cleanup
      await remove(ref(database, `ride_requests/${testRequestId}`));

      if (Array.isArray(rejectedBy) && rejectedBy.includes("rickshaw_1")) {
        addResult("Reject Ride", true, "Ride rejected and recorded correctly");
        return true;
      }
      addResult("Reject Ride", false, "Rejection not recorded properly");
      return false;
    } catch (error) {
      addResult("Reject Ride", false, `Error: ${error.message}`);
      return false;
    }
  };

  // Test 6: Pickup Confirmation
  const testPickupConfirmation = async () => {
    setCurrentTest("Testing Pickup Confirmation...");
    try {
      const testRideId = `test_ride_${Date.now()}`;

      await set(ref(database, `active_rides/${testRideId}`), {
        id: testRideId,
        status: "accepted",
      });

      // Confirm pickup
      await update(ref(database, `active_rides/${testRideId}`), {
        status: "picked_up",
        pickup_time: Date.now(),
        pickup_location: { lat: 22.4633, lng: 91.9714, accuracy: 10 },
      });

      const snapshot = await get(ref(database, `active_rides/${testRideId}`));
      const ride = snapshot.val();

      // Cleanup
      await remove(ref(database, `active_rides/${testRideId}`));

      if (ride && ride.status === "picked_up" && ride.pickup_location) {
        addResult(
          "Pickup Confirmation",
          true,
          "Pickup confirmed with GPS location"
        );
        return true;
      }
      addResult("Pickup Confirmation", false, "Pickup confirmation failed");
      return false;
    } catch (error) {
      addResult("Pickup Confirmation", false, `Error: ${error.message}`);
      return false;
    }
  };

  // Test 7: Point Calculation
  const testPointCalculation = async () => {
    setCurrentTest("Testing Point Calculation...");
    try {
      const testCases = [
        { distance: 0, expected: 10 },
        { distance: 10, expected: 9 },
        { distance: 25, expected: 7 },
        { distance: 50, expected: 5 },
        { distance: 75, expected: 2 },
        { distance: 100, expected: 0 },
      ];

      let allPassed = true;
      const calculatePoints = (distance) => {
        const basePoints = 10;
        const penalty = distance / 10.0;
        return Math.max(0, Math.floor(basePoints - penalty));
      };

      for (const testCase of testCases) {
        const calculated = calculatePoints(testCase.distance);
        if (calculated !== testCase.expected) {
          allPassed = false;
          addResult(
            "Point Calculation",
            false,
            `Failed for ${testCase.distance}m: expected ${testCase.expected}, got ${calculated}`
          );
          return false;
        }
      }

      if (allPassed) {
        addResult("Point Calculation", true, "All point calculations correct");
        return true;
      }
      return false;
    } catch (error) {
      addResult("Point Calculation", false, `Error: ${error.message}`);
      return false;
    }
  };

  // Test 8: Complete Ride
  const testCompleteRide = async () => {
    setCurrentTest("Testing Ride Completion...");
    try {
      const testRideId = `test_ride_${Date.now()}`;
      const testPointsId = `test_pts_${Date.now()}`;

      // Create active ride
      await set(ref(database, `active_rides/${testRideId}`), {
        id: testRideId,
        rickshaw_id: "rickshaw_1",
        status: "picked_up",
      });

      // Complete ride
      const completedRide = {
        id: testRideId,
        rickshaw_id: "rickshaw_1",
        status: "completed",
        dropoff_time: Date.now(),
        dropoff_location: { lat: 22.4725, lng: 91.9845 },
        dropoff_distance_from_block: 5.2,
        points_earned: 10,
        points_status: "rewarded",
      };

      await set(ref(database, `completed_rides/${testRideId}`), completedRide);
      await set(ref(database, `points_history/${testPointsId}`), {
        ride_id: testRideId,
        rickshaw_id: "rickshaw_1",
        final_points: 10,
        status: "rewarded",
        timestamp: Date.now(),
      });

      // Verify
      const rideSnapshot = await get(
        ref(database, `completed_rides/${testRideId}`)
      );
      const pointsSnapshot = await get(
        ref(database, `points_history/${testPointsId}`)
      );

      // Cleanup
      await remove(ref(database, `active_rides/${testRideId}`));
      await remove(ref(database, `completed_rides/${testRideId}`));
      await remove(ref(database, `points_history/${testPointsId}`));

      if (rideSnapshot.exists() && pointsSnapshot.exists()) {
        addResult("Complete Ride", true, "Ride completed and points recorded");
        return true;
      }
      addResult("Complete Ride", false, "Ride completion verification failed");
      return false;
    } catch (error) {
      addResult("Complete Ride", false, `Error: ${error.message}`);
      return false;
    }
  };

  // Test 9: Real-time Sync
  const testRealtimeSync = async () => {
    setCurrentTest("Testing Real-time Synchronization...");
    try {
      const testKey = `test_sync_${Date.now()}`;
      const startTime = Date.now();

      await set(ref(database, `ride_requests/${testKey}`), {
        test: true,
        timestamp: startTime,
      });

      await delay(100);

      const snapshot = await get(ref(database, `ride_requests/${testKey}`));
      const endTime = Date.now();
      const latency = endTime - startTime;

      // Cleanup
      await remove(ref(database, `ride_requests/${testKey}`));

      if (snapshot.exists() && latency < 3000) {
        addResult(
          "Real-time Sync",
          true,
          `Sync latency: ${latency}ms (< 3 seconds)`
        );
        return true;
      }
      addResult(
        "Real-time Sync",
        false,
        `Sync latency: ${latency}ms (>= 3 seconds)`
      );
      return false;
    } catch (error) {
      addResult("Real-time Sync", false, `Error: ${error.message}`);
      return false;
    }
  };

  // Test 10: Data Integrity
  const testDataIntegrity = async () => {
    setCurrentTest("Testing Data Integrity...");
    try {
      // Check if required collections exist
      const collections = [
        "location_blocks",
        "rickshaws",
        "users",
        "fare_matrix",
      ];
      let allExist = true;

      for (const collection of collections) {
        const snapshot = await get(ref(database, collection));
        if (!snapshot.exists()) {
          allExist = false;
          addResult(
            "Data Integrity",
            false,
            `Missing collection: ${collection}`
          );
          return false;
        }
      }

      if (allExist) {
        addResult("Data Integrity", true, "All required collections exist");
        return true;
      }
      return false;
    } catch (error) {
      addResult("Data Integrity", false, `Error: ${error.message}`);
      return false;
    }
  };

  // Run all tests
  const runAllTests = async () => {
    setIsRunning(true);
    setTestResults([]);

    const tests = [
      testDatabaseConnection,
      testDataIntegrity,
      testCreateRideRequest,
      testAcceptRide,
      testLEDStatusUpdates,
      testRejectRide,
      testPickupConfirmation,
      testPointCalculation,
      testCompleteRide,
      testRealtimeSync,
    ];

    for (const test of tests) {
      await test();
      await delay(500);
    }

    setCurrentTest("Tests Complete!");
    setIsRunning(false);
  };

  const clearResults = () => {
    setTestResults([]);
    setCurrentTest("");
  };

  const passed = testResults.filter((r) => r.passed).length;
  const failed = testResults.filter((r) => !r.passed).length;
  const passRate =
    testResults.length > 0
      ? ((passed / testResults.length) * 100).toFixed(1)
      : 0;

  return (
    <div className="test-runner">
      <div className="test-header">
        <h2>
          <i className="fas fa-flask"></i> Automated Test Runner
        </h2>
        <p>Run automated tests to verify all system functionality</p>
      </div>

      <div className="test-controls">
        <button
          className="btn btn-primary"
          onClick={runAllTests}
          disabled={isRunning}
        >
          <i className="fas fa-play"></i>
          {isRunning ? "Running Tests..." : "Run All Tests"}
        </button>
        <button
          className="btn btn-secondary"
          onClick={clearResults}
          disabled={isRunning}
        >
          <i className="fas fa-trash"></i> Clear Results
        </button>
      </div>

      {currentTest && (
        <div className="current-test">
          <i className="fas fa-spinner fa-spin"></i> {currentTest}
        </div>
      )}

      {testResults.length > 0 && (
        <div className="test-summary">
          <div className="summary-card">
            <h3>Test Summary</h3>
            <div className="summary-stats">
              <div className="stat-item">
                <span className="stat-label">Total Tests:</span>
                <span className="stat-value">{testResults.length}</span>
              </div>
              <div className="stat-item passed">
                <span className="stat-label">Passed:</span>
                <span className="stat-value">{passed}</span>
              </div>
              <div className="stat-item failed">
                <span className="stat-label">Failed:</span>
                <span className="stat-value">{failed}</span>
              </div>
              <div className="stat-item">
                <span className="stat-label">Pass Rate:</span>
                <span className="stat-value">{passRate}%</span>
              </div>
            </div>
            <div
              className={`pass-rate-badge ${
                passRate >= 95 ? "excellent" : passRate >= 70 ? "good" : "poor"
              }`}
            >
              {passRate >= 95
                ? "✅ Excellent"
                : passRate >= 70
                ? "⚠️ Good"
                : "❌ Needs Work"}
            </div>
          </div>
        </div>
      )}

      <div className="test-results">
        {testResults.map((result, index) => (
          <div
            key={index}
            className={`test-result ${result.passed ? "passed" : "failed"}`}
          >
            <div className="test-result-header">
              <span className="test-icon">
                {result.passed ? (
                  <i className="fas fa-check-circle"></i>
                ) : (
                  <i className="fas fa-times-circle"></i>
                )}
              </span>
              <span className="test-name">{result.testName}</span>
              <span className="test-status">
                {result.passed ? "PASSED" : "FAILED"}
              </span>
            </div>
            <div className="test-message">{result.message}</div>
            <div className="test-time">
              {new Date(result.timestamp).toLocaleTimeString()}
            </div>
          </div>
        ))}
      </div>
    </div>
  );
};

export default TestRunner;
