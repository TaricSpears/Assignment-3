const API_BASE_URL = 'http://localhost:5000/api';

export async function fetchTemperatureData() {
    const res = await fetch(`${API_BASE_URL}/temperature`);
    if (!res.ok) {
        throw new Error(`HTTP error fetching temperature data! status: ${res.status}`);
    }
    const data = await res.json();
    return data;
}

export async function setWindowOpening(percentage) {
    const res = await fetch(`${API_BASE_URL}/setwindow`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ percentage: parseInt(percentage, 10) })
    });
    if (!res.ok) {
        throw new Error(`HTTP error setting window opening! status: ${res.status}`);
    }
}

export async function getWindowOpening() {
    const res = await fetch(`${API_BASE_URL}/getwindow`);
    if (!res.ok) {
        throw new Error(`HTTP error getting window opening! status: ${res.status}`);
    }
    const data = await res.json();
    return data.value;
}

export async function getSystemState() {
    const res = await fetch(`${API_BASE_URL}/getstate`);
    if (!res.ok) {
        throw new Error(`HTTP error getting system opening! status: ${res.status}`);
    }
    const data = await res.json();
    return data.value;
}


export async function acknowledgeAlarm() {
    const res = await fetch(`${API_BASE_URL}/alarm`, { method: 'POST' });
    if (!res.ok) {
        throw new Error(`HTTP error acknowledging alarm! status: ${res.status}`);
    }
}

