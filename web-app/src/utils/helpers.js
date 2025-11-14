// ===================================
// Helper Functions - AERAS System
// ===================================

// Get location block name (updated for AERAS)
export function getLocationName(blockId) {
  const locationNames = {
    // New AERAS location blocks
    cuet_campus: 'CUET Campus',
    pahartali: 'Pahartali',
    noapara: 'Noapara',
    raojan: 'Raojan',
    // Legacy station names (backward compatibility)
    station_1: 'Station A - University Gate',
    station_2: 'Station B - City Center',
    station_3: 'Station C - Market Area',
  };

  return locationNames[blockId] || blockId;
}

// Backward compatibility
export function getStationName(stationId) {
  return getLocationName(stationId);
}

export function getTimeAgo(timestamp) {
  // Handle both Unix timestamp (milliseconds) and ISO date strings
  const now = Date.now();
  const time = typeof timestamp === 'number' ? timestamp : new Date(timestamp).getTime();
  const diff = Math.floor((now - time) / 1000); // seconds

  if (diff < 60) return 'Just now';
  if (diff < 3600) return `${Math.floor(diff / 60)} minutes ago`;
  if (diff < 86400) return `${Math.floor(diff / 3600)} hours ago`;
  return `${Math.floor(diff / 86400)} days ago`;
}

// Format points status for display
export function getPointsStatusBadge(status) {
  const statusConfig = {
    rewarded: { text: 'Rewarded', class: 'badge-success', icon: 'check-circle' },
    reduced: { text: 'Reduced', class: 'badge-warning', icon: 'exclamation-triangle' },
    pending: { text: 'Pending Review', class: 'badge-info', icon: 'clock' },
    manual_verification_required: { text: 'Manual Verify', class: 'badge-secondary', icon: 'question-circle' }
  };

  return statusConfig[status] || { text: status, class: 'badge-secondary', icon: 'info-circle' };
}

