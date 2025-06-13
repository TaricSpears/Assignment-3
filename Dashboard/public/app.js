import { createTemperatureChart } from './chart.js';
import { fetchTemperatureData, setMode, setWindowOpening, acknowledgeAlarm, getSystemState, getWindowOpening } from './api/client.js'; // Assuming client.js is in 'api' folder

const ctx = document.getElementById('temperatureChart').getContext('2d');
const chart = createTemperatureChart(ctx);

const currentModeDisplayEl = document.getElementById('currentModeDisplay');
const systemStateDisplayEl = document.getElementById('systemStateDisplay');
const windowOpeningDisplayEl = document.getElementById('windowOpeningDisplay');
const avgTempDisplayEl = document.getElementById('avgTempDisplay');
const minTempDisplayEl = document.getElementById('minTempDisplay');
const maxTempDisplayEl = document.getElementById('maxTempDisplay');

const autoModeBtn = document.getElementById('autoModeBtn');
const manualModeBtn = document.getElementById('manualModeBtn');

const manualControlAreaEl = document.getElementById('manualControlArea');
const manualSliderEl = document.getElementById('manualSlider');
const manualSliderValueEl = document.getElementById('manualSliderValue');

const alarmAreaEl = document.getElementById('alarmArea');
const ackAlarmBtn = document.getElementById('ackAlarmBtn');

let currentSystemMode = 'AUTOMATIC';
let currentState = { temperature: null, state: 'UNKNOWN', windowOpening: 0 };

function calculateTemperatureStats(temperatureData) {
    if (!temperatureData || temperatureData.length === 0) {
        return {
            avg: '--',
            min: '--',
            max: '--'
        };
    }

    const temperatures = temperatureData.map(e => e.temperature);
    const avg = temperatures.reduce((a, b) => a + b, 0) / temperatures.length;
    const min = Math.min(...temperatures);
    const max = Math.max(...temperatures);

    return {
        avg: avg.toFixed(1),
        min: min.toFixed(1),
        max: max.toFixed(1)
    };
}

function updateUI() {
    currentModeDisplayEl.textContent = currentSystemMode.charAt(0).toUpperCase() + currentSystemMode.slice(1).toLowerCase();
    systemStateDisplayEl.textContent = currentState.state;
    windowOpeningDisplayEl.textContent = currentState.windowOpening;

    // Update temperature statistics
    const stats = calculateTemperatureStats(chart.data.datasets[0].data.map((temp, index) => ({
        temperature: temp,
        timestamp: chart.data.labels[index]
    })));
    avgTempDisplayEl.textContent = stats.avg;
    minTempDisplayEl.textContent = stats.min;
    maxTempDisplayEl.textContent = stats.max;

    console.log(`Current Mode: ${currentSystemMode}`);
    if (currentSystemMode === 'AUTOMATIC') {
        manualModeBtn.classList.remove('hidden');
        autoModeBtn.classList.add('hidden');
        manualControlAreaEl.classList.add('hidden');
    } else if (currentSystemMode === 'MANUAL') {
        autoModeBtn.classList.remove('hidden');
        manualModeBtn.classList.add('hidden');
        manualControlAreaEl.classList.remove('hidden');
        manualSliderEl.value = currentState.windowOpening;
        manualSliderValueEl.textContent = currentState.windowOpening;
    }
    if (currentState.state === 'ALARM') {
        alarmAreaEl.classList.remove('hidden');
    } else {
        alarmAreaEl.classList.add('hidden');
    }
}

async function refreshData() {
    try {
        const temperature_data = await fetchTemperatureData();
        const window_opening = await getWindowOpening();
        const system_state = await getSystemState();
        if (temperature_data && temperature_data.length > 0) {
            chart.data.labels = temperature_data.map(e => new Date(e.timestamp * 1000).toLocaleTimeString());
            chart.data.datasets[0].data = temperature_data.map(e => e.temperature);
            chart.update();
        }
        currentState.temperature = temperature_data[temperature_data.length - 1].temperature;
        currentState.windowOpening = window_opening;
        currentState.state = system_state;
    } catch (error) {
        console.error("Failed to fetch data:", error);
        currentState.state = "Error fetching data";
    }
    updateUI();
}

autoModeBtn.addEventListener('click', async () => {
    try {

        await setMode('AUTOMATIC');
        currentSystemMode = 'AUTOMATIC';
        console.log('Mode set to AUTOMATIC');
        updateUI();
    } catch (error) {
        console.error("Failed to set mode to AUTOMATIC:", error);
        alert("Error setting mode to Automatic. Please try again.");
    }
});

manualModeBtn.addEventListener('click', async () => {
    try {
        await setMode('MANUAL');
        currentSystemMode = 'MANUAL';
        console.log('Mode set to MANUAL');
        updateUI();
    } catch (error) {
        console.error("Failed to set mode to MANUAL:", error);
        alert("Error setting mode to Manual. Please try again.");
    }
});

manualSliderEl.addEventListener('input', (e) => {
    manualSliderValueEl.textContent = e.target.value;
});

manualSliderEl.addEventListener('change', async (e) => {
    const percentage = parseInt(e.target.value, 10);
    try {
        await setWindowOpening(percentage);
        currentState.windowOpening = percentage;
        console.log(`Window opening set to ${percentage}%`);
        updateUI();
    } catch (error) {
        console.error("Failed to set window opening:", error);
        alert("Error setting window opening. Please try again.");
    }
});

ackAlarmBtn.addEventListener('click', async () => {
    try {
        await acknowledgeAlarm();
        console.log('Alarm acknowledged');
        await refreshData();
    } catch (error) {
        console.error("Failed to acknowledge alarm:", error);
        alert("Error acknowledging alarm. Please try again.");
    }
});

async function initializeApp() {
    await refreshData();
    setInterval(refreshData, 500);
}

initializeApp();