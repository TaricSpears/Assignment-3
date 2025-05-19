import Chart from 'chart.js/auto';

export function createTemperatureChart(ctx) {
    return new Chart(ctx, {
        type: 'line',
        data: {
            labels: [], datasets: [{
                label: 'Temperatura (°C)',
                data: [],
                borderColor: 'rgb(75, 192, 192)',
                tension: 0.1
            }]
        },
        options: {
            scales: {
                x: { title: { display: true, text: 'Ora' } },
                y: { title: { display: true, text: 'Temperatura (°C)' } }
            }
        }
    });
}