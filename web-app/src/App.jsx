import { useState, useEffect } from 'react';
import { ref, onValue } from 'firebase/database';
import { database } from './firebase';
import Header from './components/Header';
import StatisticsCards from './components/StatisticsCards';
import ActiveRide from './components/ActiveRide';
import RideRequests from './components/RideRequests';
import RideHistory from './components/RideHistory';
import Footer from './components/Footer';
import Toast from './components/Toast';

function App() {
  const [currentRickshawId, setCurrentRickshawId] = useState(null);
  const [currentRickshaw, setCurrentRickshaw] = useState(null);
  const [rickshaws, setRickshaws] = useState({});
  const [activeRide, setActiveRide] = useState(null);
  const [pendingRequests, setPendingRequests] = useState([]);
  const [rideHistory, setRideHistory] = useState([]);
  const [isConnected, setIsConnected] = useState(false);
  const [toast, setToast] = useState({ show: false, message: '', type: 'success' });

  // Firebase connection check
  useEffect(() => {
    const connectedRef = ref(database, '.info/connected');
    const unsubscribe = onValue(connectedRef, (snapshot) => {
      setIsConnected(snapshot.val() === true);
      if (snapshot.val() === true) {
        console.log('✅ Firebase connected');
      } else {
        console.log('❌ Firebase disconnected');
      }
    });

    return () => unsubscribe();
  }, []);

  // Load rickshaws
  useEffect(() => {
    const rickshawsRef = ref(database, 'rickshaws');
    const unsubscribe = onValue(rickshawsRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        setRickshaws(data);
        console.log(`✅ Loaded ${Object.keys(data).length} rickshaws`);
      }
    });

    return () => unsubscribe();
  }, []);

  // Load current rickshaw data
  useEffect(() => {
    if (!currentRickshawId) {
      setCurrentRickshaw(null);
      return;
    }

    const rickshawRef = ref(database, `rickshaws/${currentRickshawId}`);
    const unsubscribe = onValue(rickshawRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        setCurrentRickshaw(data);
      }
    });

    return () => unsubscribe();
  }, [currentRickshawId]);

  // Listen for ride requests
  useEffect(() => {
    const requestsRef = ref(database, 'ride_requests');
    const unsubscribe = onValue(requestsRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        const pending = Object.values(data).filter(req => req.status === 'pending');
        setPendingRequests(pending);
      } else {
        setPendingRequests([]);
      }
    });

    return () => unsubscribe();
  }, []);

  // Listen for active rides
  useEffect(() => {
    if (!currentRickshawId) {
      setActiveRide(null);
      return;
    }

    const ridesRef = ref(database, 'rides');
    const unsubscribe = onValue(ridesRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        const active = Object.values(data).find(
          ride => ride.rickshaw_id === currentRickshawId && 
                 (ride.status === 'accepted' || ride.status === 'in_progress')
        );
        setActiveRide(active || null);
      } else {
        setActiveRide(null);
      }
    });

    return () => unsubscribe();
  }, [currentRickshawId]);

  // Load ride history
  useEffect(() => {
    if (!currentRickshawId) {
      setRideHistory([]);
      return;
    }

    const ridesRef = ref(database, 'rides');
    const unsubscribe = onValue(ridesRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        const history = Object.values(data)
          .filter(ride => ride.rickshaw_id === currentRickshawId && ride.status === 'completed')
          .sort((a, b) => new Date(b.dropoff_time) - new Date(a.dropoff_time))
          .slice(0, 10);
        setRideHistory(history);
        console.log(`✅ Loaded ${history.length} completed rides`);
      } else {
        setRideHistory([]);
      }
    });

    return () => unsubscribe();
  }, [currentRickshawId]);

  // Show welcome toast on mount
  useEffect(() => {
    showToast('Welcome to Smart Rickshaw Dashboard!');
  }, []);

  const showToast = (message, type = 'success') => {
    setToast({ show: true, message, type });
    setTimeout(() => {
      setToast(prev => ({ ...prev, show: false }));
    }, 3000);
  };

  const handleRickshawSelection = (rickshawId) => {
    setCurrentRickshawId(rickshawId || null);
    if (rickshawId) {
      console.log(`Selected rickshaw: ${rickshawId}`);
    }
  };

  return (
    <div className="app">
      <Header
        rickshaws={rickshaws}
        currentRickshawId={currentRickshawId}
        currentRickshaw={currentRickshaw}
        onRickshawSelect={handleRickshawSelection}
      />

      <main className="main-content">
        <div className="container">
          <StatisticsCards currentRickshaw={currentRickshaw} />
          
          {activeRide && (
            <ActiveRide
              activeRide={activeRide}
              currentRickshawId={currentRickshawId}
              currentRickshaw={currentRickshaw}
              showToast={showToast}
            />
          )}

          <RideRequests
            pendingRequests={pendingRequests}
            currentRickshawId={currentRickshawId}
            showToast={showToast}
          />

          <RideHistory
            rideHistory={rideHistory}
          />
        </div>
      </main>

      <Footer isConnected={isConnected} />
      
      <Toast
        show={toast.show}
        message={toast.message}
        type={toast.type}
      />
    </div>
  );
}

export default App;

