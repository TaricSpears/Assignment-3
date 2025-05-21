const API_BASE_URL = 'http://localhost:5000/api';

export async function fetchTemperatureData() {
    const res = await fetch(`${API_BASE_URL}/temperature`);
    const data = await res.json();
    console.log(data);
    return data;
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