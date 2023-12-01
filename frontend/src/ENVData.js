import React, { useEffect, useState } from "react";

import { Link } from 'react-router-dom';
import axios from 'axios';
import './App.css';


const ENVData = ({}) => {
 
  const [values, setvalues] = useState([]);
  const [temperature, setTemperature] = useState("");
  const [pressure, setPressure] = useState("");
  const [motorStatus, setMotorStatus] = useState("");


  const handleCheckStatus = async (e) => {
    e.preventDefault();

    const response = await fetch("https://emerging-technology-project.onrender.com/sendData", {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
      }
    });
    const data = await response.json();
    console.log(response)
    setvalues(data)
    setTemperature(data.temperature)
    setPressure(data.pressure)
    setMotorStatus(data.motorStatus)
    
  };
   
  
    return (
        <div className="App">
        

        
            <button type="submit" onClick = {handleCheckStatus} >Check Status</button>
            <br/>
            <label> temperature : {temperature}</label><br/>
            <label> pressure : {pressure}</label><br/>
            <label> Motor Status : {motorStatus}</label>
            

        </div>
      
  );  
};

export default ENVData;