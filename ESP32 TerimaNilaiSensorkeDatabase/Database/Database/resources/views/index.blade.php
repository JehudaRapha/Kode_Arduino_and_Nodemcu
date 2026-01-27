<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Dashboard Sensor Keren</title>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
<style>
    /* Latar belakang luar angkasa dengan efek lebih dalam */
    body {
        margin: 0;
        padding: 0;
        font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
        background: radial-gradient(circle at 50% 100%, #1a0a0f, #0b0c1a, #000);
        overflow-x: hidden;
        color: #fff;
        min-height: 100vh;
        position: relative;
    }

    /* Efek bintang background */
    .stars {
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        pointer-events: none;
        overflow: hidden;
    }

    .star {
        position: absolute;
        width: 2px;
        height: 2px;
        background: #fff;
        border-radius: 50%;
        animation: twinkle 3s infinite;
    }

    @keyframes twinkle {
        0%, 100% { opacity: 0.3; }
        50% { opacity: 1; }
    }

    /* Bulan dengan efek glow */
    .moon {
        position: fixed;
        top: 40px;
        right: 40px;
        width: 140px;
        height: 140px;
        background: radial-gradient(circle at 32% 32%, #f5f5f5, #d0d0d0, #808080);
        border-radius: 50%;
        box-shadow: 0 0 50px rgba(255, 255, 255, 0.3),
                    0 0 80px rgba(139, 0, 0, 0.2),
                    inset -15px -15px 30px rgba(0, 0, 0, 0.3);
        z-index: 5;
    }

    .moon::before {
        content: '';
        position: absolute;
        width: 100%;
        height: 100%;
        border-radius: 50%;
        background: radial-gradient(circle at 35% 35%, rgba(255,255,255,0.1), transparent);
        box-shadow: 0 0 60px rgba(139, 0, 0, 0.15);
    }



    /* Header dengan gradien dinamis */
    .header-section {
        text-align: center;
        padding: 40px 20px 30px;
        background: linear-gradient(180deg, rgba(139, 0, 0, 0.1), transparent);
        border-bottom: 2px solid rgba(139, 0, 0, 0.3);
        margin-bottom: 30px;
        animation: slideDown 0.8s ease-out;
    }

    @keyframes slideDown {
        from {
            opacity: 0;
            transform: translateY(-20px);
        }
        to {
            opacity: 1;
            transform: translateY(0);
        }
    }

    .header-section h1 {
        font-size: 3rem;
        font-weight: 800;
        background: linear-gradient(135deg, #ff6b6b, #8b0000, #660000);
        -webkit-background-clip: text;
        -webkit-text-fill-color: transparent;
        background-clip: text;
        margin: 0;
        text-shadow: 0 0 20px rgba(139, 0, 0, 0.3);
    }

    /* Card dashboard utama */
    .dashboard-card {
        background: linear-gradient(135deg, rgba(139, 0, 0, 0.9), rgba(179, 0, 0, 0.8));
        color: #fff;
        border-radius: 25px;
        padding: 50px 40px;
        text-align: center;
        margin: 30px auto;
        max-width: 550px;
        box-shadow:
            0 15px 50px rgba(139, 0, 0, 0.4),
            0 0 30px rgba(255, 107, 107, 0.2),
            inset 0 1px 0 rgba(255, 255, 255, 0.1);
        border: 1px solid rgba(255, 255, 255, 0.1);
        backdrop-filter: blur(10px);
        animation: fadeInUp 0.8s ease-out;
        position: relative;
        overflow: hidden;
    }

    .dashboard-card::before {
        content: '';
        position: absolute;
        top: -50%;
        left: -50%;
        width: 200%;
        height: 200%;
        background: radial-gradient(circle, rgba(255,255,255,0.1), transparent);
        animation: shimmer 3s infinite;
    }

    @keyframes fadeInUp {
        from {
            opacity: 0;
            transform: translateY(30px);
        }
        to {
            opacity: 1;
            transform: translateY(0);
        }
    }

    @keyframes shimmer {
        0%, 100% { transform: translate(-50%, -50%); }
        50% { transform: translate(50%, 50%); }
    }

    .dashboard-card h2 {
        font-size: 1.2rem;
        font-weight: 600;
        color: #ffe0e0;
        margin-bottom: 15px;
        letter-spacing: 1px;
        position: relative;
        z-index: 1;
    }

    .dashboard-card .total {
        font-size: 5rem;
        font-weight: 900;
        margin-top: 20px;
        color: #fff;
        text-shadow: 0 0 20px rgba(255, 107, 107, 0.5);
        animation: pulse 2s ease-in-out infinite;
        position: relative;
        z-index: 1;
    }

    @keyframes pulse {
        0%, 100% { opacity: 1; }
        50% { opacity: 0.8; }
    }

    /* Container tabel */
    .sensor-section {
        max-width: 1000px;
        margin: 40px auto;
        padding: 0 20px;
    }

    .sensor-section h3 {
        color: #ff6b6b;
        font-weight: 700;
        margin-bottom: 20px;
        font-size: 1.4rem;
        text-transform: uppercase;
        letter-spacing: 2px;
        text-shadow: 0 0 10px rgba(139, 0, 0, 0.3);
    }

    /* Tabel dengan styling premium */
    .sensor-table {
        width: 100%;
        background: linear-gradient(135deg, rgba(139, 0, 0, 0.15), rgba(26, 28, 46, 0.8));
        border-radius: 20px;
        overflow: hidden;
        border: 1px solid rgba(139, 0, 0, 0.3);
        box-shadow:
            0 10px 40px rgba(139, 0, 0, 0.2),
            inset 0 1px 0 rgba(255, 255, 255, 0.1);
        backdrop-filter: blur(10px);
    }

    table {
        color: #fff;
        width: 100%;
        border-collapse: collapse;
    }

    thead {
        background: linear-gradient(90deg, rgba(139, 0, 0, 0.8), rgba(179, 0, 0, 0.7));
        border-bottom: 2px solid rgba(255, 107, 107, 0.3);
    }

    thead th {
        padding: 20px 15px;
        font-weight: 700;
        font-size: 1rem;
        letter-spacing: 1px;
        text-transform: uppercase;
        text-align: center;
        color: #fff;
    }

    tbody tr {
        border-bottom: 1px solid rgba(139, 0, 0, 0.2);
        transition: all 0.3s ease;
    }

    tbody tr:hover {
        background: rgba(255, 107, 107, 0.15);
        transform: scale(1.01);
    }

    tbody tr:nth-child(odd) {
        background: rgba(255, 255, 255, 0.02);
    }

    td {
        padding: 18px 15px;
        text-align: center;
        vertical-align: middle;
        font-size: 0.95rem;
    }

    /* Status badge */
    .status-badge {
        display: inline-block;
        padding: 6px 14px;
        border-radius: 20px;
        background: linear-gradient(135deg, #ff6b6b, #8b0000);
        color: #fff;
        font-size: 0.85rem;
        font-weight: 600;
        box-shadow: 0 0 10px rgba(255, 107, 107, 0.3);
    }

    /* Empty state */
    .empty-state {
        text-align: center;
        padding: 50px 20px;
        color: rgba(255, 255, 255, 0.5);
        font-style: italic;
    }

    /* Footer info */
    .footer-info {
        text-align: center;
        padding: 30px 20px;
        color: rgba(255, 255, 255, 0.4);
        font-size: 0.85rem;
        border-top: 1px solid rgba(139, 0, 0, 0.2);
    }

    /* Glow orb background */
    .glow-orb {
        position: fixed;
        border-radius: 50%;
        background: radial-gradient(circle, rgba(255, 107, 107, 0.15), transparent);
        filter: blur(60px);
        pointer-events: none;
    }

    .glow-orb-1 {
        width: 400px;
        height: 400px;
        top: -100px;
        left: -100px;
        animation: float 15s ease-in-out infinite;
    }

    .glow-orb-2 {
        width: 350px;
        height: 350px;
        bottom: -50px;
        right: -50px;
        animation: float 12s ease-in-out infinite reverse;
    }

    @keyframes float {
        0%, 100% { transform: translate(0, 0); }
        50% { transform: translate(30px, -30px); }
    }

    /* Responsive */
    @media (max-width: 768px) {
        .moon {
            width: 80px;
            height: 80px;
            top: 20px;
            right: 20px;
        }

        .header-section h1 {
            font-size: 2rem;
        }

        .dashboard-card {
            padding: 30px 20px;
        }

        .dashboard-card .total {
            font-size: 3.5rem;
        }

        table {
            font-size: 0.85rem;
        }

        td, th {
            padding: 12px 8px;
        }

        .glow-orb-1 {
            width: 250px;
            height: 250px;
        }

        .glow-orb-2 {
            width: 200px;
            height: 200px;
        }
    }
</style>
</head>
<body>

<div class="stars" id="starsContainer"></div>

<div class="glow-orb glow-orb-1"></div>
<div class="glow-orb glow-orb-2"></div>

<div class="moon"></div>

<div class="header-section">
    <h1>📊 SENSOR DASHBOARD</h1>
</div>

<div class="dashboard-card">
    <h2>Total Orang di Dalam</h2>
    <div class="total" id="totalOrang">0</div>
</div>

<div class="sensor-section">
    <h3>📡 Data Sensor Real-time</h3>
    <div class="sensor-table">
        <table>
            <thead>
                <tr>
                    <th>ID</th>
                    <th>Sensor Name</th>
                    <th>Value</th>
                    <th>Data Masuk</th>
                </tr>
            </thead>
            <tbody id="sensorTableBody">
                <tr><td colspan="4" class="empty-state">Belum ada data</td></tr>
            </tbody>
        </table>
    </div>
</div>

<div class="footer-info">
    <p>🔄 Auto-refresh setiap detik | Last updated: <span id="lastUpdate">-</span></p>
</div>

<script>
    const API_URL = "http://localhost:8000/api/sensors/inside";

    // Generate bintang random di background
    function generateStars() {
        const container = document.getElementById('starsContainer');
        const starCount = 100;
        for (let i = 0; i < starCount; i++) {
            const star = document.createElement('div');
            star.classList.add('star');
            star.style.left = Math.random() * 100 + '%';
            star.style.top = Math.random() * 100 + '%';
            star.style.animationDelay = Math.random() * 3 + 's';
            container.appendChild(star);
        }
    }



    generateStars();

    // Fetch data API
    async function fetchData() {
        try {
            const res = await fetch(API_URL);
            const json = await res.json();

            if (json.total_orang !== undefined) {
                document.getElementById("totalOrang").innerText = json.total_orang;
            }

            const tableBody = document.getElementById("sensorTableBody");
            tableBody.innerHTML = "";
            if (json.data && json.data.length > 0) {
                json.data.forEach((data, index) => {
                    const row = document.createElement("tr");
                    const date = new Date(data.created_at).toLocaleString();
                    row.innerHTML = `
                        <td>${index + 1}</td> <!-- nomor urut mulai dari 1 -->
                        <td><strong>${data.sensor_name}</strong></td>
                        <td><span class="status-badge">${data.value}</span></td>
                        <td>${date}</td>
                    `;
                    tableBody.appendChild(row);
                });
            } else {
                tableBody.innerHTML = `<tr><td colspan="4" class="empty-state">Belum ada data</td></tr>`;
            }


            // Update last refresh time
            document.getElementById("lastUpdate").innerText = new Date().toLocaleTimeString();
        } catch (err) {
            console.error("Error fetch data:", err);
        }
    }

    fetchData();
    setInterval(fetchData, 1000);
</script>

</body>
</html>
