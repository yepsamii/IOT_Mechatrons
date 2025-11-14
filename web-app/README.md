# Smart Rickshaw Dashboard - React App

A modern React-based dashboard for the Smart Rickshaw Management System.

## Tech Stack

- **React 18** - UI library with hooks
- **Vite** - Build tool and dev server
- **Firebase 12** - Realtime database
- **CSS3** - Styling

## Quick Start

1. **Install dependencies**
   ```bash
   npm install
   ```

2. **Update Firebase config**
   - Edit `src/firebase.js`
   - Add your Firebase credentials

3. **Run development server**
   ```bash
   npm run dev
   ```
   - Opens at http://localhost:3000

## Available Scripts

- `npm run dev` - Start development server
- `npm run build` - Build for production
- `npm run preview` - Preview production build

## Project Structure

```
src/
├── main.jsx              # Entry point
├── App.jsx               # Main component
├── firebase.js           # Firebase config
├── components/           # React components
│   ├── Header.jsx
│   ├── StatisticsCards.jsx
│   ├── ActiveRide.jsx
│   ├── RideRequests.jsx
│   ├── RideHistory.jsx
│   ├── Footer.jsx
│   └── Toast.jsx
└── utils/                # Helper functions
    └── helpers.js
```

## Features

- ✅ Real-time ride requests
- ✅ Accept/reject rides
- ✅ Active ride tracking
- ✅ Statistics dashboard
- ✅ Ride history
- ✅ Responsive design
- ✅ Firebase integration

## Firebase Setup

1. Create Firebase project
2. Enable Realtime Database
3. Get configuration
4. Update `src/firebase.js`
5. Import initial data from `../initial_database.json`

## License

Created for IOTrix - Televerse 1.0 Competition

