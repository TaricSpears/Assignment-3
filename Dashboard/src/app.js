import { createTemperatureChart } from './chart.js';
import { fetchTemperatureData, setMode, setWindowOpening, acknowledgeAlarm } from './api/client.js';

// Initialize Chart
const ctx = document.getElementById('temperatureChart').getContext('2d');
const chart = createTemperatureChart(ctx);

// DOM Elements
const systemStateEl = document.getElementById('systemState');
const windowOpeningEl = document.getElementById('windowOpening');
const manualSlider = document.getElementById('manualSlider');

// Update data every 2 seconds
setInterval(async () => {
    const data = await fetchTemperatureData();

    // Update chart
    chart.data.labels = data.map(entry => new Date(entry.timestamp).toLocaleTimeString());
    chart.data.datasets[0].data = data.map(entry => entry.temperature);
    chart.update();

    // Update system state
    systemStateEl.textContent = data[data.length - 1].state;
    windowOpeningEl.textContent = data[data.length - 1].windowOpening;
}, 2000);

// Event Listeners
document.getElementById('autoModeBtn').addEventListener('click', () => {
    setMode('AUTOMATIC');
});

document.getElementById('manualModeBtn').addEventListener('click', () => {
    setMode('MANUAL');
    document.getElementById('manualControl').style.display = 'block';
});

manualSlider.addEventListener('input', (e) => {
    setWindowOpening(e.target.value);
});

document.getElementById('ackAlarmBtn').addEventListener('click', () => {
    acknowledgeAlarm();
});