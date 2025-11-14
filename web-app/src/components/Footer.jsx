import React from 'react';

function Footer({ isConnected }) {
  return (
    <footer className="footer">
      <div className="container">
        <p>&copy; 2025 Smart Rickshaw System | IOTrix - Televerse 1.0</p>
        <p className={`connection-status ${isConnected ? 'connected' : 'disconnected'}`}>
          <span>
            <i className="fas fa-circle"></i>{' '}
            {isConnected ? 'Connected' : 'Disconnected'}
          </span>
        </p>
      </div>
    </footer>
  );
}

export default Footer;

