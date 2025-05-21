import { createTemperatureChart } from './chart.js';
import { fetchTemperatureData, setMode, setWindowOpening, acknowledgeAlarm } from './api/client.js';

// Inizializzo Chart
const ctx = document.getElementById('temperatureChart').getContext('2d');
const chart = createTemperatureChart(ctx);

// Elementi DOM
const systemStateEl = document.getElementById('systemState');
const windowOpeningEl = document.getElementById('windowOpening');
const manualSlider = document.getElementById('manualSlider');
setInterval(async () => {
    const data = await fetchTemperatureData();

    chart.data.labels = data.map(e => new Date(e.timestamp).toLocaleTimeString());
    chart.data.datasets[0].data = data.map(e => e.temperature);
    chart.update();

    const latest = data[data.length - 1];
    systemStateEl.textContent = latest.state;
    windowOpeningEl.textContent = latest.windowOpening;
}, 2000);

// Event listeners

document.getElementById('autoModeBtn').addEventListener('click', () => setMode('AUTOMATIC'));

document.getElementById('manualModeBtn').addEventListener('click', () => {
    setMode('MANUAL');
    document.getElementById('manualControl').classList.remove('hidden');
});

manualSlider.addEventListener('input', e => setWindowOpening(e.target.value));

document.getElementById('ackAlarmBtn').addEventListener('click', () => acknowledgeAlarm());