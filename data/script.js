let charts = {};

async function fetchAndParse() {
    const tbody = document.getElementById('tableBody');
    const status = document.getElementById('status');
    tbody.innerHTML = "<tr><td colspan='7'>Downloading and parsing...</td></tr>";
    
    let labels = [], dataLight = [], dataTemp = [], dataHum = [], dataPress = [];
    let accelX = [], accelY = [], accelZ = [];
    let gyroX = [], gyroY = [], gyroZ = [];

    try {
        const response = await fetch('/data.log');
        if(!response.ok) throw new Error("Log file not found");
        
        const buffer = await response.arrayBuffer();
        if(buffer.byteLength === 0) {
            tbody.innerHTML = "<tr><td colspan='7'>File is empty.</td></tr>";
            return;
        }

        const view = new DataView(buffer);
        let offset = 0;
        let htmlStr = "";

        while(offset < buffer.byteLength) {
            if(offset + 9 > buffer.byteLength) break; 
            
            const start_code = view.getUint32(offset, true); 
            offset += 4;
            const ts_unix = view.getUint32(offset, true);
            offset += 4;
            const mask = view.getUint8(offset);
            offset += 1;

            if(start_code !== 0x424f4220) break; 

            const dateObj = new Date(ts_unix * 1000);
            const timeStr = dateObj.toLocaleTimeString();
            labels.push(timeStr);

            const getFloats = (count) => {
                let vals = [];
                for(let i=0; i<count; i++) {
                    vals.push(parseFloat(view.getFloat32(offset, true).toFixed(2)));
                    offset += 4;
                }
                return vals;
            };

            let lightVal = (mask & (1 << 0)) ? getFloats(1)[0] : null;
            let tempVal  = (mask & (1 << 1)) ? getFloats(1)[0] : null;
            let humVal   = (mask & (1 << 2)) ? getFloats(1)[0] : null;
            let pressVal = (mask & (1 << 3)) ? getFloats(1)[0] : null;
            let accelVals = (mask & (1 << 4)) ? getFloats(3) : [null, null, null];
            let gyroVals  = (mask & (1 << 5)) ? getFloats(3) : [null, null, null];

            dataLight.push(lightVal);
            dataTemp.push(tempVal);
            dataHum.push(humVal);
            dataPress.push(pressVal);
            accelX.push(accelVals[0]); accelY.push(accelVals[1]); accelZ.push(accelVals[2]);
            gyroX.push(gyroVals[0]);   gyroY.push(gyroVals[1]);   gyroZ.push(gyroVals[2]);

            htmlStr += `<tr>
                <td>${timeStr}</td>
                <td>${lightVal ?? "-"}</td>
                <td>${tempVal ?? "-"}</td>
                <td>${humVal ?? "-"}</td>
                <td>${pressVal ?? "-"}</td>
                <td>${accelVals[0] !== null ? accelVals.join(', ') : "-"}</td>
                <td>${gyroVals[0] !== null ? gyroVals.join(', ') : "-"}</td>
            </tr>`;
        }
        
        tbody.innerHTML = htmlStr || "<tr><td colspan='7'>No valid packets found.</td></tr>";
        status.innerText = "Status: Data Loaded";

        renderSingleChart('tempChart', 'Temperature (°C)', labels, dataTemp, '#ff4444');
        renderSingleChart('humChart', 'Humidity (%)', labels, dataHum, '#4444ff');
        renderSingleChart('pressChart', 'Pressure (hPa)', labels, dataPress, '#44ff44');
        renderSingleChart('lightChart', 'Illuminance (Lux)', labels, dataLight, '#ffeb3b');
        
        renderIMUChart('accelChart', 'Accelerometer (m/s2)', labels, accelX, accelY, accelZ);
        renderIMUChart('gyroChart', 'Gyroscope (rad/s)', labels, gyroX, gyroY, gyroZ);
        
    } catch(err) {
        tbody.innerHTML = `<tr><td colspan='7' style='color:red;'>Error: ${err.message}</td></tr>`;
    }
}

function renderSingleChart(id, label, labels, data, color) {
    createChart(id, labels, [{ label: label, data: data, borderColor: color }]);
}

function renderIMUChart(id, title, labels, x, y, z) {
    createChart(id, labels, [
        { label: title + ' x', data: x, borderColor: '#ff4444' },
        { label: title + ' y', data: y, borderColor: '#44ff44' },
        { label: title + ' z', data: z, borderColor: '#4444ff' }
    ]);
}

function createChart(id, labels, datasets) {
    if(charts[id]) charts[id].destroy();
    const canvas = document.getElementById(id);
    if (!canvas) return;
    const ctx = canvas.getContext('2d');

    charts[id] = new Chart(ctx, {
        type: 'line',
        data: { 
            labels, 
            datasets: datasets.map(d => ({
                ...d, 
                tension: 0.1, 
                pointRadius: 2, 
                spanGaps: true
            })) 
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            animation: {
                duration: 0
            },
            transitions: {
                active: {
                    animation: {
                        duration: 0
                    }
                },
                resize: {
                    animation: {
                        duration: 0 
                    }
                }
            },
            scales: {
                x: { ticks: { color: '#aaa' }, grid: { color: '#333' } },
                y: { ticks: { color: '#aaa' }, grid: { color: '#333' } }
            },
            plugins: {
                legend: { labels: { color: '#fff' } },
                zoom: {
                    pan: { enabled: true, mode: 'xy' },
                    zoom: {
                        wheel: { enabled: true, modifierKey: 'ctrl' },
                        pinch: { enabled: true },
                        mode: 'xy'
                    }
                }
            }
        }
    });
}

function resetAllZooms() {
    Object.values(charts).forEach(chart => chart.resetZoom());
}
