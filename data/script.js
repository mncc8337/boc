async function fetchAndParse() {
    const tbody = document.getElementById('tableBody');
    tbody.innerHTML = "<tr><td colspan='7'>Downloading and parsing...</td></tr>";
    
    try {
        const response = await fetch('/data.log');
        if (!response.ok) throw new Error("Log file not found");
        
        const buffer = await response.arrayBuffer();
        if (buffer.byteLength === 0) {
            tbody.innerHTML = "<tr><td colspan='7'>File is empty.</td></tr>";
            return;
        }

        const view = new DataView(buffer);
        let offset = 0;
        let htmlStr = "";

        while (offset < buffer.byteLength) {
            // --- 1. PARSE HEADER (9 Bytes) ---
            if (offset + 9 > buffer.byteLength) break; 
            
            const start_code = view.getUint32(offset, true); 
            offset += 4;
            
            const ts_unix = view.getUint32(offset, true);
            offset += 4;
            
            const mask = view.getUint8(offset);
            offset += 1;

            if (start_code !== 0x424f4220) {
                console.error("Corrupted packet found at byte", offset);
                break; 
            }

            const dateObj = new Date(ts_unix * 1000);
            const timeStr = dateObj.toLocaleTimeString();

            // --- 2. PARSE FLEXIBLE PAYLOAD ---
            const getFloats = (count) => {
                let vals = [];
                for(let i=0; i<count; i++) {
                    vals.push(view.getFloat32(offset, true).toFixed(2));
                    offset += 4;
                }
                return vals;
            };

            let light = (mask & (1 << 0)) ? getFloats(1)[0] : "-";
            let temp =  (mask & (1 << 1)) ? getFloats(1)[0] : "-";
            let hum =   (mask & (1 << 2)) ? getFloats(1)[0] : "-";
            let press = (mask & (1 << 3)) ? getFloats(1)[0] : "-";
            let accel = (mask & (1 << 4)) ? getFloats(3).join(', ') : "-";
            let gyro =  (mask & (1 << 5)) ? getFloats(3).join(', ') : "-";

            // --- 3. BUILD HTML ROW ---
            htmlStr += `<tr>
                <td>${timeStr}</td>
                <td>${light}</td>
                <td>${temp}</td>
                <td>${hum}</td>
                <td>${press}</td>
                <td>${accel}</td>
                <td>${gyro}</td>
            </tr>`;
        }
        
        tbody.innerHTML = htmlStr || "<tr><td colspan='7'>No valid packets found.</td></tr>";
        
    } catch (err) {
        tbody.innerHTML = `<tr><td colspan='7' style='color:red;'>Error: ${err.message}</td></tr>`;
    }
}
