import React from 'react';

function Header({ rickshaws, currentRickshawId, currentRickshaw, onRickshawSelect }) {
  const getStatusInfo = () => {
    if (!currentRickshaw) {
      return { text: 'Offline', className: '' };
    }
    if (currentRickshaw.status === 'available') {
      return { text: 'Available', className: 'online' };
    }
    if (currentRickshaw.status === 'busy') {
      return { text: 'On Ride', className: 'busy' };
    }
    return { text: 'Offline', className: '' };
  };

  const statusInfo = getStatusInfo();

  return (
    <header className="header">
      <div className="container">
        <div className="header-content">
          <div className="logo">
            <i className="fas fa-taxi"></i>
            <h1>Smart Rickshaw - AERAS</h1>
          </div>
          <div className="header-info">
            <div className="rickshaw-selector">
              <label htmlFor="rickshaw-select">
                <i className="fas fa-user-circle"></i>
              </label>
              <select
                id="rickshaw-select"
                className="rickshaw-dropdown"
                value={currentRickshawId || ''}
                onChange={(e) => onRickshawSelect(e.target.value)}
              >
                <option value="">Select Rickshaw...</option>
                {Object.values(rickshaws).map((rickshaw) => (
                  <option key={rickshaw.id} value={rickshaw.id}>
                    {rickshaw.puller_name} ({rickshaw.license_number})
                  </option>
                ))}
              </select>
            </div>
            <div className="status-indicator">
              <span className={`status-dot ${statusInfo.className}`}></span>
              <span>{statusInfo.text}</span>
            </div>
          </div>
        </div>
      </div>
    </header>
  );
}

export default Header;

