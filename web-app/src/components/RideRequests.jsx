import React, { useEffect } from 'react';
import { ref, update, get } from 'firebase/database';
import { database } from '../firebase';
import { getStationName, getTimeAgo } from '../utils/helpers';

function RideRequests({ pendingRequests, currentRickshawId, showToast }) {
  useEffect(() => {
    if (pendingRequests.length > 0) {
      console.log('🔔 New ride request notification');
    }
  }, [pendingRequests.length]);

  const handleAcceptRide = async (requestId) => {
    if (!currentRickshawId) {
      showToast('Please select a rickshaw first', 'error');
      return;
    }

    console.log(`Accepting ride request: ${requestId}`);

    try {
      const requestSnapshot = await get(ref(database, `ride_requests/${requestId}`));
      const request = requestSnapshot.val();

      if (!request) {
        showToast('Request not found', 'error');
        return;
      }

      // Create ride object
      const rideId = `ride_${Date.now()}`;
      const ride = {
        id: rideId,
        user_id: request.user_id || 'user_1',
        rickshaw_id: currentRickshawId,
        pickup_station: request.pickup_station,
        dropoff_station: request.dropoff_station,
        distance_km: request.distance_km,
        fare: request.estimated_fare,
        points_earned: request.estimated_points,
        status: 'accepted',
        request_time: request.timestamp,
        accept_time: new Date().toISOString(),
        pickup_time: null,
        dropoff_time: null,
      };

      // Update database
      const updates = {};
      updates[`rides/${rideId}`] = ride;
      updates[`ride_requests/${requestId}/status`] = 'accepted';
      updates[`rickshaws/${currentRickshawId}/status`] = 'busy';

      await update(ref(database), updates);
      showToast('Ride accepted successfully!');
      console.log('✅ Ride accepted');
    } catch (error) {
      showToast('Error accepting ride', 'error');
      console.error('❌ Error:', error);
    }
  };

  const handleRejectRide = async (requestId) => {
    console.log(`Rejecting ride request: ${requestId}`);

    try {
      const updates = {};
      updates[`ride_requests/${requestId}/status`] = 'rejected';
      await update(ref(database), updates);
      showToast('Request rejected');
      console.log('✅ Request rejected');
    } catch (error) {
      showToast('Error rejecting request', 'error');
      console.error('❌ Error:', error);
    }
  };

  return (
    <section className="requests-section">
      <div className="section-header">
        <h2>
          <i className="fas fa-bell"></i> Incoming Ride Requests
        </h2>
        <span className="badge">{pendingRequests.length} pending</span>
      </div>
      <div className="requests-container">
        {pendingRequests.length === 0 ? (
          <div className="empty-state">
            <i className="fas fa-inbox"></i>
            <p>No pending requests</p>
            <small>New ride requests will appear here</small>
          </div>
        ) : (
          pendingRequests.map((request) => (
            <div key={request.id} className="request-card">
              <div className="request-header">
                <div className="request-time">
                  <i className="fas fa-clock"></i>
                  <span>{getTimeAgo(request.timestamp)}</span>
                </div>
                <span className="badge badge-warning">New Request</span>
              </div>

              <div className="request-route">
                <div className="request-location">
                  <h4>Pickup</h4>
                  <p>{getStationName(request.pickup_station)}</p>
                </div>
                <div className="request-arrow">
                  <i className="fas fa-arrow-right"></i>
                </div>
                <div className="request-location">
                  <h4>Drop-off</h4>
                  <p>{getStationName(request.dropoff_station)}</p>
                </div>
              </div>

              <div className="request-info">
                <div className="request-info-item">
                  <i className="fas fa-route"></i>
                  <span>{request.distance_km} km</span>
                </div>
                <div className="request-info-item">
                  <i className="fas fa-money-bill-wave"></i>
                  <span>৳{request.estimated_fare}</span>
                </div>
                <div className="request-info-item">
                  <i className="fas fa-star"></i>
                  <span>+{request.estimated_points} points</span>
                </div>
              </div>

              <div className="request-actions">
                <button
                  className="btn btn-success"
                  onClick={() => handleAcceptRide(request.id)}
                >
                  <i className="fas fa-check"></i> Accept Ride
                </button>
                <button
                  className="btn btn-danger"
                  onClick={() => handleRejectRide(request.id)}
                >
                  <i className="fas fa-times"></i> Reject
                </button>
              </div>
            </div>
          ))
        )}
      </div>
    </section>
  );
}

export default RideRequests;

