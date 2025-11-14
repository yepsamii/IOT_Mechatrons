import React from 'react';
import { ref, update } from 'firebase/database';
import { database } from '../firebase';
import { getStationName } from '../utils/helpers';

function ActiveRide({ activeRide, currentRickshawId, currentRickshaw, showToast }) {
  const handleConfirmPickup = async () => {
    if (!activeRide) return;

    console.log('Confirming pickup');

    const updates = {};
    updates[`rides/${activeRide.id}/status`] = 'in_progress';
    updates[`rides/${activeRide.id}/pickup_time`] = new Date().toISOString();

    try {
      await update(ref(database), updates);
      showToast('Pickup confirmed! Drive safely.');
      console.log('✅ Pickup confirmed');
    } catch (error) {
      showToast('Error confirming pickup', 'error');
      console.error('❌ Error:', error);
    }
  };

  const handleConfirmDropoff = async () => {
    if (!activeRide) return;

    console.log('Confirming drop-off');

    const updates = {};
    updates[`rides/${activeRide.id}/status`] = 'completed';
    updates[`rides/${activeRide.id}/dropoff_time`] = new Date().toISOString();
    updates[`rickshaws/${currentRickshawId}/status`] = 'available';
    updates[`rickshaws/${currentRickshawId}/total_points`] =
      (currentRickshaw.total_points || 0) + activeRide.points_earned;
    updates[`rickshaws/${currentRickshawId}/total_rides`] =
      (currentRickshaw.total_rides || 0) + 1;

    try {
      await update(ref(database), updates);
      showToast(`Ride completed! +${activeRide.points_earned} points earned!`);
      console.log('✅ Drop-off confirmed');

      // Show completion summary
      setTimeout(() => {
        alert(
          `🎉 Ride Completed!\n\nPoints Earned: ${activeRide.points_earned}\nFare: ৳${activeRide.fare}\nDistance: ${activeRide.distance_km} km\n\nYou're now available for new requests!`
        );
      }, 500);
    } catch (error) {
      showToast('Error confirming drop-off', 'error');
      console.error('❌ Error:', error);
    }
  };

  const pickupName = getStationName(activeRide.pickup_station);
  const dropoffName = getStationName(activeRide.dropoff_station);
  const isPickedUp = activeRide.status === 'in_progress';

  return (
    <section className="active-ride-section">
      <div className="section-header">
        <h2>
          <i className="fas fa-car"></i> Active Ride
        </h2>
        <span className="badge badge-success">In Progress</span>
      </div>
      <div className="active-ride-card">
        <div className="ride-route">
          <div className="ride-location">
            <h3>From</h3>
            <p>{pickupName}</p>
          </div>
          <div className="ride-arrow">
            <i className="fas fa-arrow-right"></i>
          </div>
          <div className="ride-location">
            <h3>To</h3>
            <p>{dropoffName}</p>
          </div>
        </div>

        <div className="ride-details">
          <div className="ride-detail-item">
            <i className="fas fa-route"></i>
            <p>Distance</p>
            <strong>{activeRide.distance_km} km</strong>
          </div>
          <div className="ride-detail-item">
            <i className="fas fa-money-bill-wave"></i>
            <p>Fare</p>
            <strong>৳{activeRide.fare}</strong>
          </div>
          <div className="ride-detail-item">
            <i className="fas fa-star"></i>
            <p>Points</p>
            <strong>+{activeRide.points_earned}</strong>
          </div>
        </div>

        <div className="ride-actions">
          {!isPickedUp ? (
            <button className="btn btn-success btn-block" onClick={handleConfirmPickup}>
              <i className="fas fa-user-check"></i> Confirm Pickup
            </button>
          ) : (
            <button className="btn btn-success btn-block" onClick={handleConfirmDropoff}>
              <i className="fas fa-flag-checkered"></i> Confirm Drop-off
            </button>
          )}
        </div>
      </div>
    </section>
  );
}

export default ActiveRide;

