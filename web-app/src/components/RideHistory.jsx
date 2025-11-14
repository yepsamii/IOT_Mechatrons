import React from 'react';
import { getStationName } from '../utils/helpers';

function RideHistory({ rideHistory }) {
  return (
    <section className="history-section">
      <div className="section-header">
        <h2>
          <i className="fas fa-history"></i> Ride History
        </h2>
        <button className="btn-secondary">
          <i className="fas fa-sync-alt"></i> Refresh
        </button>
      </div>
      <div className="history-container">
        {rideHistory.length === 0 ? (
          <div className="empty-state">
            <i className="fas fa-clock"></i>
            <p>No ride history</p>
            <small>Completed rides will appear here</small>
          </div>
        ) : (
          rideHistory.map((ride) => {
            const pickupName = getStationName(ride.pickup_station);
            const dropoffName = getStationName(ride.dropoff_station);
            const date = new Date(ride.dropoff_time).toLocaleString();

            return (
              <div key={ride.id} className="history-card">
                <div className="history-header">
                  <span className="history-id">Ride #{ride.id.slice(-8)}</span>
                  <span className="history-date">{date}</span>
                </div>

                <div className="history-route">
                  <div className="history-location">
                    <h4>{pickupName}</h4>
                  </div>
                  <i className="fas fa-arrow-right" style={{ color: 'var(--primary-color)' }}></i>
                  <div className="history-location">
                    <h4>{dropoffName}</h4>
                  </div>
                </div>

                <div className="history-stats">
                  <div className="history-stat">
                    <i className="fas fa-route"></i>
                    <span>{ride.distance_km} km</span>
                  </div>
                  <div className="history-stat">
                    <i className="fas fa-money-bill-wave"></i>
                    <span>৳{ride.fare}</span>
                  </div>
                  <div className="history-stat">
                    <i className="fas fa-star"></i>
                    <span>+{ride.points_earned} pts</span>
                  </div>
                </div>
              </div>
            );
          })
        )}
      </div>
    </section>
  );
}

export default RideHistory;

