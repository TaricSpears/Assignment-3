const API_BASE_URL = 'http://localhost:3000/api';

export async function fetchTemperatureData() {
    const res = await fetch(`${API_BASE_URL}/temperature`);
    return res.json();
}

export async function setMode(mode) {
    await fetch(`${API_BASE_URL}/mode`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ mode })
    });
}

export async function setWindowOpening(percentage) {
    await fetch(`${API_BASE_URL}/window`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ percentage })
    });
}

export async function acknowledgeAlarm() {
    await fetch(`${API_BASE_URL}/alarm`, { method: 'POST' });
}