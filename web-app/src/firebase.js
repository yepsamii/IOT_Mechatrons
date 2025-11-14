// ===================================
// Firebase Configuration
// ===================================
import { initializeApp } from "firebase/app";
import { getDatabase } from "firebase/database";

const firebaseConfig = {
  apiKey: "AIzaSyCyPJA1r5HQKGPKT_xRPio1Yzafgu1pxAI",
  authDomain: "rickshaw-ride-c5683.firebaseapp.com",
  projectId: "rickshaw-ride-c5683",
  storageBucket: "rickshaw-ride-c5683.firebasestorage.app",
  messagingSenderId: "899569287338",
  appId: "1:899569287338:web:f5bc28bcd16ee6766050d3",
  measurementId: "G-PVHYN92J0X",
  databaseURL:
    "https://rickshaw-ride-c5683-default-rtdb.asia-southeast1.firebasedatabase.app",
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

console.log("Firebase initialized successfully!");

export { database };
