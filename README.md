# ⚡ Energy Consumption Controller

**Energy Consumption Controller** is an **ESP8266-based** monitoring system designed to measure and log **AC power consumption** in real time. It communicates with the **PZEM-014/016**  sensor module using the **Modbus protocol** and provides a user-friendly **web interface**  for live data viewing, historical energy stats, and system configuration.

## ✨ Features

* 📊 Collects **voltage, current, power, energy, frequency, and power factor** from a PZEM-014/016 sensor
* 💾 Saves data to **CSV** format on internal storage (LittleFS)
* 📆 Calculates ☀️**daily** and 🌙 **nightly** energy statistics
* 🖥️ Web interface includes:

  * 📱 **Fully responsive design** — works great on both mobile and desktop devices
  * 🔌 **Live sensor data** (via WebSocket)
  * 📈 **Interactive charts** for voltage and power
  * 📋 **Real-time logs** with log level filtering
  * ⚙️ **Settings page** to configure interval, timezone, log level, and data retention
* 📥 Download CSV data and energy summaries
* ⏱️ Automatically syncs time after WiFi connection
* 📡 Smart AP fallback mode with **WiFi credentials page** if STA connection fails

## 🔧 Installation & Setup

### Requirements

* [PlatformIO](https://platformio.org/) extension for VS Code (or CLI)
* ESP8266-based board (e.g., NodeMCU v3)
* [PZEM-014/016](/docs/PZEM-014,016%20specification.pdf) sensor

### Flashing / OTA

* Firmware can be uploaded via USB or **OTA**.
* OTA upload uses the IP address of the ESP (set in `platformio.ini`).

### File Upload & Access

* Web UI files are located in the `/data` directory.

* Upload files to the ESP8266's filesystem using:

  ```
  pio run --target uploadfs
  ```

* Files on the device can also be accessed and managed via **FTP** (using the included FTPClientServer library).


## 🧭 Usage & Web Interface Overview

### 🔌 First Boot & Setup

When the device boots for the first time:

* It starts in **Access Point (AP)** mode.
* A **Wi-Fi portal** is opened. Connect to the ESP8266 Wi-Fi and open any page — it will redirect to the Wi-Fi login page.
* After you enter valid Wi-Fi credentials, the controller connects to your router and syncs time via NTP.
* Data logging starts automatically after the first connection.
* If Wi-Fi connection is lost, the AP opens again automatically — previously saved credentials are remembered.

> ![Wi-Fi Setup Screenshot](docs/img/auth.png)

---

### 🌐 Web Interface Overview

Once the device is connected to Wi-Fi, open its IP address in your browser. You'll see the web interface with multiple pages:

#### `/` — **Dashboard**

* Live sensor data (via WebSocket)
* Realtime voltage and power charts (parsed from CSV)

> ![Dashboard Screenshot](docs/img//main.png)

---

#### `/settings` — **Settings Page**

* Modify controller configuration:

  * Data reading interval
  * Timezone and day/night start times
  * Retention (how long to keep sensor data in storage)
  * Log level
* Reset energy counter
* Download files with sensor data and energy stats
* Change Wi-Fi credentials

> ⚠️ After changing Wi-Fi credentials, the controller disconnects from the current network and connects to the new one — you won’t see connection status. To access the interface again, connect to the new Wi-Fi network manually

![Settings Screenshot](docs/img/settings.png)

---

#### `/logs` — **Logs Page**

* View real-time logs (via WebSocket)
* Includes saved log history

---

### 🔄 API Routes

You can access most functionality using HTTP routes or WebSockets. Useful for automation, integrations, or advanced control.

<details>
<summary><strong>🔧 System Routes</strong></summary>

| Route              | Method | Description                                                              |
| ------------------ | ------ | ------------------------------------------------------------------------ |
| `/api/restart`     | POST   | Restart the controller                                                   |
| `/api/pause`       | POST   | Pause all logic (data/log saving, time sync) but keep interfaces running |
| `/api/resume`      | POST   | Resume functionality after pause                                         |
| `/api/system-info` | GET    | Get info about chip, memory, filesystem, etc.                            |
| `/api/clear-old`   | POST   | Delete old data from CSV based on retention setting                      |

</details>

<details>
<summary><strong>⚙️ Settings & Wi-Fi Routes</strong></summary>

| Route               | Method     | Description                           |
| ------------------- | ---------- | ------------------------------------- |
| `/api/settings`     | GET / POST | Get or update configuration           |
| `/api/reset-energy` | PUT        | Reset the energy measurement baseline |
| `/api/connect-wifi` | POST       | Set new Wi-Fi credentials             |

> ⚠️ All changes are applied **immediately** and saved to flash.
</details>

<details>
<summary><strong>📡 WebSocket Endpoints</strong></summary>

| Socket Path | Purpose                |
| ----------- | ---------------------- |
| `/ws/data`  | Realtime sensor values |
| `/ws/logs`  | Realtime logs          |

</details>

---

### 📂 File Access

* **Raw sensor data** and **energy stats** are continuously saved as **CSV files** in internal storage.
* Stored files can be:

  * Accessed **directly by path**, e.g. `/sensor-data.csv`, `/energy-stats.csv`, `/configs.json`
  * **Uploaded/downloaded via FTP**
* Files downloaded from the **Settings page** are **generated dynamically** — they are **not stored** as-is in the file system. These versions are processed/filtered to be user-friendly for export
