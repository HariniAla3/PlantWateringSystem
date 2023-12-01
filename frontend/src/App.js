import './App.css';
import React from 'react';
import ENVData from './ENVData';
import { BrowserRouter as Router, Route, Routes} from 'react-router-dom';
function App() {
  return (
    <div className="App">
    <Router>
    <Routes>
      <Route path="/" Component={ENVData} />
    
    </Routes>
    </Router>
    </div>
    
  );
}

export default App;
