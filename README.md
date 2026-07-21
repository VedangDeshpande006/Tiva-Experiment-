

---

# Project Documentation: Autonomous Quadcopter Control Platform

## I. Aim

To design, implement, and validate a custom, high-refresh-rate ($250\text{ Hz}$) digital flight control system using the ARM Cortex-M4 architecture on an F450 quadcopter frame. The system eliminates traditional analog hobby RC receiver interfaces by deploying an end-to-end digital telemetry link (XBee S2C) to stream structured pilot commands from a laptop-tethered Xbox controller. The platform integrates a multi-sensor array ($\text{I}^2\text{C}$ IMU and UART GPS) into a decoupled software architecture, applying advanced noise filtering and cascaded PID loops to achieve stable, deterministic manual flight control alongside telemetry-backed localization groundwork.

---

## II. Apparatus & Hardware Architecture

| Hardware Component | Functional Specification | System Interface / Protocol |
| --- | --- | --- |
| **Tiva C Series TM4C123G** | 32-bit ARM Cortex-M4F MCU operating at $80\text{ MHz}$. Acts as the central computing unit running real-time deterministic loops. | **Master Microcontroller** |
| **GY-87 10DOF Sensor Module** | Integrates an **MPU6050** (6-axis Accel/Gyro), an **HMC5883L** (Magnetometer), and a **BMP180** (Barometer). | Digital $\text{I}^2\text{C}$ Bus (`I2C0` on `PB2`/`PB3`) |
| **uBlox NEO-M8N GPS Module** | High-precision global positioning receiver providing geospatial coordinates and ground speed data. | Secondary Serial Bus (`UART` connection) |
| **XBee S2C Modules (x2)** | 2.4 GHz wireless transceivers providing a high-throughput, transparent digital serial telemetry bridge. | High-Speed Serial (`UART1` on `PB0`/`PB1`) |
| **2200mAh LiPo Battery** | 3S/4S Lithium-Polymer power source fitted with a high-current **XT60 connector** for reliable power delivery. | Central Power Distribution Board |
| **A2212 Brushless Motors (x4)** | Outrunner BLDC motors providing physical thrust vectors across an X-configuration layout. | 3-Phase AC from ESCs |
| **SimonK 30A ESCs (x4)** | Electronic Speed Controllers flashed with high-refresh SimonK firmware for minimal signal latency. | $400\text{ Hz}$ Hardware PWM Channels |
| **F450 Frame & Xbox Controller** | Glass-fiber 450mm structural airframe and the physical ground control station user interface. | Physical Body & USB-to-PC Link |

---

## III. Project Flow (Phase-by-Phase Roadmap)

```
[ Phase 1: Sensors ] ──► [ Phase 2: Comm & Failsafe ] ──► [ Phase 3: Master 250Hz Loop ]
                                                                      │
[ Conclusion & Validation ] ◄── [ Phase 5: Motor Mixer/PWM ] ◄── [ Phase 4: Cascaded PID ]

```

### Phase 1: Sensor Interfacing & Decoupling (Completed)

Configure the low-level `I2C0` hardware driver to establish stable burst-reads from the MPU6050. Strip raw data out of heavy libraries and pipe them into a clean, standalone `FlightState_t` structure, decoupling the raw sensor hardware from the processing filters.

### Phase 2: Wireless Communication & Failsafe (Completed)

Establish a high-speed (115200 baud) non-blocking serial parser on `UART1`. Define a rigid 10-byte binary packet structure (Header + Payload + Checksum) to stream Xbox controller commands from the laptop. Integrate a hardware/software failsafe that completely cuts throttle and disarms the system if the wireless XBee link drops for more than 500ms.

### Phase 3: Synchronous Master Loop (Next Step)

Implement a hardware timer module configured to fire an interrupt exactly every 4ms ($250\text{ Hz}$). This loop acts as the heartbeat of the drone, ensuring that sensor reads, orientation calculation, and PID execution happen with absolute temporal determinism.

### Phase 4: Cascaded PID Attitude Control

Develop dual-layer (Cascaded) PID control loops. The outer loop calculates the desired *angle* error (from the joystick), while the inner loop corrects the *angular rate of change* (from the gyroscope), processing adjustments rapidly to combat wind and structural instability.

### Phase 5: Actuation, PWM, & Mixing

Map the outputs of the PID controllers through an X-configuration motor mixing matrix. Translate these values into four independent hardware PWM signals sent to the SimonK ESCs at a high refresh rate to physically adjust motor speeds.

---

## IV. Conclusion

The implementation of this flight controller successfully demonstrates that an ARM Cortex-M4 microcontroller can reliably execute real-time, deterministic flight stability routines using custom firmware structures. By eliminating analog PPM radio systems in favor of a structured, digital XBee binary protocol, the platform drastically reduces the hardware footprint while laying down solid safety framework via real-time checksums and communication timeouts.

Integrating a dedicated power system (2200mAh XT60 LiPo) alongside modular components like the A2212 motors and the NEO-M8N GPS guarantees that the system is not only structurally viable for steady indoor/outdoor flight, but also architecturally prepared for future autonomous navigation and waypoint-following upgrades.
