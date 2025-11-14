import React from 'react';

function StatisticsCards({ currentRickshaw }) {
  const totalPoints = currentRickshaw?.total_points || 0;
  const totalRides = currentRickshaw?.total_rides || 0;
  const earnings = totalPoints * 5;
  const rating = (currentRickshaw?.rating || 5.0).toFixed(1);

  return (
    <section className="stats-section">
      <div className="stats-grid">
        <div className="stat-card">
          <div className="stat-icon blue">
            <i className="fas fa-star"></i>
          </div>
          <div className="stat-info">
            <h3>{totalPoints}</h3>
            <p>Total Points</p>
          </div>
        </div>
        
        <div className="stat-card">
          <div className="stat-icon green">
            <i className="fas fa-route"></i>
          </div>
          <div className="stat-info">
            <h3>{totalRides}</h3>
            <p>Total Rides</p>
          </div>
        </div>
        
        <div className="stat-card">
          <div className="stat-icon yellow">
            <i className="fas fa-money-bill-wave"></i>
          </div>
          <div className="stat-info">
            <h3>৳{earnings}</h3>
            <p>Total Earnings</p>
          </div>
        </div>
        
        <div className="stat-card">
          <div className="stat-icon purple">
            <i className="fas fa-star-half-alt"></i>
          </div>
          <div className="stat-info">
            <h3>{rating}</h3>
            <p>Rating</p>
          </div>
        </div>
      </div>
    </section>
  );
}

export default StatisticsCards;

