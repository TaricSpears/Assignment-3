const API_BASE_URL = 'http://localhost:5000/api';

export async function fetchTemperatureData() {
    const res = await fetch(`${API_BASE_URL}/temperature`);
    if (!res.ok) {
        throw new Error(`HTTP error fetching temperature data! status: ${res.status}`);
    }
    const data = await res.json();
    return data;
}

export async function setMode(mode) {
    const res = await fetch(`${API_BASE_URL}/setmode`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ mode })
    });
    if (!res.ok) {
        throw new Error(`HTTP error setting mode! status: ${res.status}`);
    }
}

export async function setWindowOpening(percentage) {
    const res = await fetch(`${API_BASE_URL}/window`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ percentage: parseInt(percentage, 10) })
    });
    if (!res.ok) {
        throw new Error(`HTTP error setting window opening! status: ${res.status}`);
    }
}

export async function acknowledgeAlarm() {
    const res = await fetch(`${API_BASE_URL}/alarm`, { method: 'POST' });
    if (!res.ok) {
        throw new Error(`HTTP error acknowledging alarm! status: ${res.status}`);
    }
}