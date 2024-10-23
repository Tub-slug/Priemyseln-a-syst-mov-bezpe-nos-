const express = require('express');
const { SerialPort } = require('serialport');  // Correct import for SerialPort
const { ReadlineParser } = require('@serialport/parser-readline');  // Import ReadlineParser for data parsing
const path = require('path');

const app = express();
const port = new SerialPort({ path: 'COM3', baudRate: 9600 }); // Use path instead of passing directly as the first argument
const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

app.use(express.static(path.join(__dirname, 'public')));

let sensorData = {
    soundValue: 0,
    temperature: 0,
    humidity: 0,
    waterValue: 0,
    motionState1: 0,
    motionState2: 0,
    vibrationState: 0,
    codeMatched: false,
    currentNumber: 0,
    currentIndex: 0,
    buzzerActive: false,
};

parser.on('data', (data) => {
    const values = data.trim().split(',');

    if (values.length >= 9) {
        sensorData.soundValue = parseInt(values[0], 10);
        sensorData.temperature = parseFloat(values[1]);
        sensorData.humidity = parseFloat(values[2]);
        sensorData.waterValue = parseInt(values[3], 10);
        sensorData.motionState1 = parseInt(values[4], 10);
        sensorData.motionState2 = parseInt(values[5], 10);
        sensorData.vibrationState = parseInt(values[6], 10);
        sensorData.codeMatched = values[7] === "Code Matched";
        sensorData.currentNumber = parseInt(values[8], 10);
        sensorData.currentIndex = parseInt(values[9], 10);

        // Update buzzer state based on conditions
        sensorData.buzzerActive = sensorData.vibrationState === 0 || sensorData.waterValue > 600 ||
            sensorData.motionState1 === 1 || sensorData.motionState2 === 1 || sensorData.temperature > 40;
    }

    console.log(sensorData); // Log sensor data for debugging
});

app.get('/sensor-data', (req, res) => {
    res.json(sensorData);
});

app.listen(3000, () => {
    console.log('Server running at http://localhost:3000');
});
