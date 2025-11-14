// ===================================
// Helper Functions
// ===================================

export function getStationName(stationId) {
  const stationNames = {
    station_1: 'Station A - University Gate',
    station_2: 'Station B - City Center',
    station_3: 'Station C - Market Area',
  };

  return stationNames[stationId] || stationId;
}

export function getTimeAgo(timestamp) {
  const now = new Date();
  const time = new Date(timestamp);
  const diff = Math.floor((now - time) / 1000); // seconds

  if (diff < 60) return 'Just now';
  if (diff < 3600) return `${Math.floor(diff / 60)} minutes ago`;
  if (diff < 86400) return `${Math.floor(diff / 3600)} hours ago`;
  return `${Math.floor(diff / 86400)} days ago`;
}

