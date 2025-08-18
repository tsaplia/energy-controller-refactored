# ⚡ Energy Consumption Controller

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange.svg)](https://platformio.org/)
[![ESP8266](https://img.shields.io/badge/ESP8266-Compatible-blue.svg)](https://www.espressif.com/en/products/socs/esp8266)
[![PZEM](https://img.shields.io/badge/PZEM-014%2F016-green.svg)](#hardware-requirements)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

> **Professional-grade AC power monitoring system** for home and industrial applications. Monitor voltage, current, power consumption, and energy usage with real-time web interface and historical analytics.

## 🎯 Why Choose This Controller?

✅ **Industrial Accuracy** - 0.5% measurement precision with PZEM sensors  
✅ **Real-time Monitoring** - Live data streaming with interactive charts  
✅ **Smart Data Management** - Automatic CSV logging with configurable retention  
✅ **Mobile-First Design** - Responsive web interface works on any device  

## 📋 Table of Contents

- [⚠️ Safety & Prerequisites](#️-safety--prerequisites)
- [🛠️ Hardware Requirements](#️-hardware-requirements)
  - [Component Selection Guide](#component-selection-guide)
  - [Wiring & Connections](#wiring--connections)
- [🚀 Quick Start](#-quick-start)
- [📱 Detailed Setup](#-detailed-setup)
- [🌐 Web Interface Guide](#-web-interface-guide)
  - [Dashboard Overview](#dashboard-overview)
  - [Settings Configuration](#settings-configuration)
  - [System Logs](#system-logs)
- [🔌 Usage Examples](#-usage-examples)
- [🔄 API Reference](#-api-reference)
- [📂 File System Structure](#-file-system-structure)
- [🔧 Troubleshooting](#-troubleshooting)
- [🚀 Advanced Features](#-advanced-features)
- [🤝 Contributing](#-contributing)

---

## ⚠️ Safety & Prerequisites

### ⚡ Important Safety Notice

> **Working with AC Power**  
> This project measures AC mains voltage. While the PZEM sensors provide isolation, basic electrical safety is important. Turn off power when making connections and double-check your wiring.

**Basic Safety Steps:**
- ⚡ Turn off circuit breaker when connecting wires
- 🔌 Use proper wire connections (no loose wires)
- 📦 Keep electronics in a suitable enclosure
- 🔍 Double-check connections before powering on

### 📋 What You Need to Know

**Coding Experience:**
- **None required!** Just follow the setup instructions
- Copy and paste - the code is ready to use
- Basic computer skills (downloading, uploading files)

**Basic Electronics:**
- Connecting wires (positive, negative, data lines)
- Using a USB cable to program the ESP8266
- Basic understanding that electricity can be dangerous

### 🔧 Hardware Specifications

**ESP8266 Board:**
- NodeMCU v3 or similar ESP8266 development board
- USB cable for programming
- 5V power supply (phone charger works fine)

**PZEM Sensor Options:**
- **PZEM-014**: For loads up to 10A (most household circuits)
- **PZEM-016**: For loads up to 100A (requires external current transformer)
- Measures: voltage, current, power, energy, frequency, power factor
- Accuracy: ±0.5% 
- Built-in isolation for safety

### ⏱️ Time Estimate

- **Hardware assembly**: 1-2 hours (simple wire connections)
- **Software setup**: 15-30 minutes (mostly downloading and uploading)
- **Getting it working**: Usually works right away if wired correctly

### 🛡️ Quick Safety Checklist

- [ ] I'll turn off power when connecting wires
- [ ] I'll double-check connections before turning power back on  
- [ ] I understand this involves electrical work (but it's manageable)
- [ ] I'll ask for help if I'm unsure about anything

---

## 🛠️ Hardware Requirements

### 📦 Shopping List

#### **Core Components**

**ESP8266 Development Board**
- Any ESP8266 board with accessible GPIO pins
- Examples: NodeMCU, Wemos D1 Mini, ESP-01 (with adapter), etc.
- Must have 3.3V and 5V power available
- USB programming interface recommended for easy setup

**Power Monitoring Sensor:**

| Model | Max Current | Best For |
|-------|-------------|----------|
| **PZEM-014** | 10A | Household circuits, small appliances |
| **PZEM-016** | 100A | Main panels, large loads (needs external CT) |

> 📖 **Detailed specifications**: See [PZEM-014/016 Technical Manual](docs/PZEM-014,016%20specification.pdf) for complete electrical specifications and communication protocol details.

**Note**: This project is specifically designed for PZEM-014/016 sensors due to their Modbus RTU communication protocol. Other power sensors would require code modifications.

#### **Additional Components**
- **Jumper wires** (male-to-female, 4 pieces minimum)
- **5V Power supply** (USB phone charger works fine)
- **Project enclosure** (optional but recommended for permanent installation)

### 🔌 Component Selection Guide

**Choose PZEM-014 if:**
- ✅ Monitoring individual appliances or circuits under 10A
- ✅ Want simpler installation (built-in current measurement)
- ✅ Most household applications (lights, outlets, small appliances)

**Choose PZEM-016 if:**
- ✅ Monitoring main electrical panel or large loads
- ✅ Need to measure over 10A (up to 100A)
- ✅ Already have or can install current transformer (CT)

### 🔧 Wiring & Connections

#### **ESP8266 to PZEM Connection**
```
ESP8266                PZEM-014/016
==================    ============
5V or VIN        -->  VCC (+5V)
GND              -->  GND  
GPIO13 (D7)*     -->  TX (Data out from PZEM)
GPIO15 (D8)*     -->  RX (Data in to PZEM)
```
*Pin numbers may vary by board - check your specific ESP8266 board pinout

#### **PZEM Power Connections**

⚡ **IMPORTANT**: Turn off circuit breaker before making these connections!

**For PZEM-014:**
- Connect **L** (Live/Hot) wire through the device  
- Connect **N** (Neutral) wire through the device
- Built-in current sensing - no external wires needed

**For PZEM-016:**
- Connect **L** and **N** for voltage sensing
- Install **current transformer (CT)** around the main wire
- CT connects to designated terminals on PZEM-016

> 🔗 **Detailed wiring diagrams**: See figures 4.1 and 4.2 in the [PZEM Technical Manual](docs/PZEM-014,016%20specification.pdf)

### 📋 Optional Accessories

- **Project enclosure**: Protect electronics (DIN rail mount available)
- **Heat shrink tubing**: Professional wire connections
- **Terminal blocks**: Easier wire connections
- **Current transformer**: Required for PZEM-016 (usually included)

---

## 🚀 Quick Start

> **For experienced users** - Get running in 30 minutes. Need more detail? Jump to [Detailed Setup](#-detailed-setup).

**Prerequisites:** [PlatformIO](https://platformio.org/) installed (VS Code extension or CLI)

### **Step 1: Wire Hardware**
```
ESP8266 → PZEM-014/016
5V/VIN  → VCC
GND     → GND  
GPIO13  → TX
GPIO15  → RX
```
Connect PZEM to your AC circuit (⚡ **power off first**).

### **Step 2: Clone & Setup**
```bash
git clone https://github.com/yourusername/energy-consumption-controller.git
cd energy-consumption-controller
```

### **Step 3: Build & Upload**
```bash
platformio run --target upload          # Upload firmware via USB
platformio run --target uploadfs        # Upload web interface files
```
*Or use VS Code PlatformIO extension buttons*

### **Step 4: Configure WiFi**
1. ESP creates AP `ESP8266-Sensor` (password: `password`)
2. Connect and enter your WiFi credentials  
3. Device connects to your network and shows IP address

### **Step 5: Access Interface**
- Find ESP IP address (router admin or serial monitor)
- Open `http://ESP_IP_ADDRESS` in browser
- Start monitoring power consumption!

### **⚡ Quick Troubleshooting**
- **No sensor data?** → Check wiring, verify PZEM power
- **Can't connect to WiFi?** → Reset and try AP mode again  
- **Upload failed?** → Check USB cable, try different port
- **Web interface not loading (Error 404)?** → Verify filesystem upload completed

📖 **Need help?** See [Detailed Setup](#-detailed-setup) or [Troubleshooting](#-troubleshooting) sections.

---

## 📱 Detailed Setup

> **Step-by-step guide for beginners** - No prior experience needed!

### **Step 1: Install Development Environment**

#### **Option A: VS Code + PlatformIO (Recommended)**
1. **Download VS Code**: Go to [code.visualstudio.com](https://code.visualstudio.com/) and install
2. **Install PlatformIO extension**:
   - Open VS Code
   - Click Extensions icon (left sidebar) 
   - Search "PlatformIO IDE"
   - Click Install on the official extension
   - Restart VS Code when prompted

#### **Option B: PlatformIO CLI**
1. **Install Python** (if not already installed)
2. **Install PlatformIO Core**:
   ```bash
   pip install platformio
   ```

### **Step 2: Download Project Files**

#### **If you have Git:**
```bash
git clone https://github.com/tsaplia/energy-controller-refactored.git
cd energy-controller-refactored
```

#### **If you don't have Git:**
1. Go to the project's GitHub page
2. Click green "Code" button → "Download ZIP"  
3. Extract ZIP file to a folder
4. Remember the folder location

### **Step 3: Hardware Assembly**

#### **Gather Your Components**
- ESP8266 board (any model with GPIO pins)
- PZEM-014 or PZEM-016 sensor
- 4 jumper wires (male-to-female)
- USB cable for ESP8266

#### **Make Connections**
⚡ **IMPORTANT**: Do this with power OFF!

1. **Connect ESP8266 to PZEM**:
   ```
   ESP8266 Pin    →    PZEM Pin
   ============        =========
   5V or VIN      →    VCC
   GND            →    GND  
   GPIO13 (D7)*   →    TX
   GPIO15 (D8)*   →    RX
   ```
   *Pin labels may vary - check your board's pinout

2. **Connect PZEM to AC Power** (⚡ **Turn off circuit breaker first!**):
   - **PZEM-014**: Connect Live and Neutral wires through the device
   - **PZEM-016**: Connect L/N for voltage, install CT for current

3. **Double-check all connections** before powering on

### **Step 4: Configure & Upload Software**

#### **Open Project**
- **VS Code**: File → Open Folder → Select your project folder
- **CLI**: Navigate to project folder in terminal

#### **Upload Firmware**

**Using VS Code:**
1. Click PlatformIO icon in left sidebar
2. Under "PROJECT TASKS" → "nodemcuv3" → "General"
3. Click "Upload" (this compiles and uploads firmware)
4. Wait for "SUCCESS" message

**Using CLI:**
```bash
platformio run --target upload
```

#### **Upload Web Interface Files**

**Using VS Code:**
1. Same location as above
2. Click "Upload Filesystem Image"
3. Wait for completion

**Using CLI:**
```bash
platformio run --target uploadfs
```

### **Step 5: First Boot & WiFi Setup**

#### **What to Expect**
1. **ESP8266 starts** and creates its own WiFi network
2. **Network name**: `ESP8266-Sensor` (password: `password`)
3. **Connection process**:
   - Connect your phone/computer to ESP8266's WiFi
   - Open any webpage - it will redirect to setup page
   - Enter your home WiFi name and password
   - Device connects to your network and **displays IP address on the page**
  

#### **Find Your Device**
**Method 1 - Router Admin:**
- Log into your router (usually 192.168.1.1 or 192.168.0.1)
- Look for new device in connected devices list

**Method 2 - Serial Monitor:**
- Keep USB cable connected
- Open serial monitor in PlatformIO (115200 baud)
- Device will print its IP address

### **Step 6: Access Web Interface**

1. **Open browser** and go to: `http://YOUR_ESP_IP_ADDRESS`
2. **You should see** the energy monitoring dashboard
3. **If sensor is connected properly**, you'll see live voltage/current/power readings

### **🎉 Success! What Now?**

- **Dashboard**: View real-time power consumption
- **Settings**: Configure data intervals, timezones, export your energy data, etc.
- **Logs**: Monitor system operation

### **❌ Something Not Working?**

**No sensor data showing?**
- Check wiring connections
- Verify PZEM has power (AC connected)
- Try different GPIO pins if needed

**Can't connect to WiFi?**
- Reset ESP8266 (power cycle)
- Try connecting to AP again
- Check WiFi password

**Upload failed?**
- Check USB cable connection
- Try different USB port
- Make sure ESP8266 drivers are installed

**Still stuck?** Check the [Troubleshooting](#-troubleshooting) section below!

