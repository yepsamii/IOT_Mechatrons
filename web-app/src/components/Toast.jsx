import React from 'react';

function Toast({ show, message, type }) {
  return (
    <div className={`toast ${show ? 'show' : ''} ${type === 'error' ? 'error' : ''}`}>
      <i className="fas fa-check-circle"></i>
      <span>{message}</span>
    </div>
  );
}

export default Toast;

