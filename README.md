# Aegis-EdgeOS
**A High-Performance AI-Integrated Distributed Operating System for Edge Computing**

Aegis-EdgeOS is a lightweight, C-based controller and communication framework designed to bridge high-level Artificial Intelligence with low-level embedded hardware. It enables a central host to process complex natural language intents and propagate commands to distributed hardware nodes (ESP32, Raspberry Pi, STM32) wirelessly over a local network.


## 🚀 Key Features

### 🧠 Core Intelligence
- **AI Intent Extraction:** Integrated with Groq Cloud (Llama 3) for real-time natural language processing.
- **Semantic Mapping:** Translates human language (e.g., "It's too dark in here") into structured hardware instructions (`LED_ON`).

### 🌐 Distributed Networking
- **Custom C HTTP Server:** A lightweight server built from scratch to host real-time telemetry.
- **JSON-Based Protocol:** Standardized communication using `ArduinoJson` and custom C-structs for hardware-agnostic compatibility.
- **Wireless Edge Nodes:** ESP32 implementation that polls the central kernel for autonomous execution.

### 💻 System & UI
- **Modular C Kernel:** Multi-threaded architecture designed for low-memory overhead.
- **Real-time Dashboard:** A Cyberpunk-themed Web UI for live monitoring of system states and AI logs.
- **Cross-Platform Host:** Fully compatible with Linux, macOS, and Windows (via WSL/MinGW).


## 🛠 Tech Stack
| Component | Technology |
| :--- | :--- |
| **Language** | C (Kernel), C++ (Hardware), JavaScript (Dashboard) |
| **AI Engine** | Groq Cloud API / Llama 3 70B |
| **Hardware** | ESP32 (Xtensa Dual-Core), Breadboard Circuitry |
| **Libraries** | Libcurl, ArduinoJson, POSIX Threads, WiFi.h |


## 📂 Project Structure
Aegis-EdgeOS/
├── src/                # Central C-Kernel source files
├── include/            # Header files and global definitions
├── public/             # Web Dashboard (HTML/JS/CSS)
├── hardware_nodes/     # Edge node implementations
│   └── esp32/          # ESP32 C++ (PlatformIO) project
├── build/              # Compiled binaries
└── node_config.json    # The "Live" communication bridge


## ⚙️ Installation & Deployment

### 1. Host Machine Setup (Linux / macOS / Windows)
Ensure you have a C compiler and `libcurl` installed.

# Clone the repository
git clone [https://github.com/YukiCodepth/Aegis-EdgeOS.git](https://github.com/YukiCodepth/Aegis-EdgeOS.git)
cd Aegis-EdgeOS

# Build the system
mkdir build && cd build
cmake ..
make

### 2. Edge Node Setup (ESP32)
1. Open the `hardware_nodes/esp32` folder in **VS Code** with the **PlatformIO** extension.
2. Update `main.cpp` with your Wi-Fi credentials and the Host IP Address.
3. Connect your ESP32 and click **Upload**.


## 🕹 Usage Instructions

1. **Initialize the OS:**

   # From the root directory
   source .env && ./build/aegis_controller

2. **Access the Dashboard:**
   Open `http://localhost:8080` in your web browser.

3. **Issue AI Commands:**
   Within the Aegis terminal prompt, type:
   `ai intent "Turn on the security lights"`

4. **Hardware Response:**
   The ESP32 will detect the payload update within 5 seconds and trigger the physical GPIO pin.


## 🗺 Roadmap
- [ ] **Phase 1:** Resolve physical GPIO current starvation on ESP32 Node.
- [ ] **Phase 2:** Implement WebSockets for <100ms real-time latency.
- [ ] **Phase 3:** Add mDNS for automatic service discovery (removing static IPs).
- [ ] **Phase 4:** Integrate AES-256 encryption for secure telemetry.

**Lead Architect:** Aman Kumar (YukiCodepth)  
**Affiliation:** Electronics & Communication Engineering, SRMIST
