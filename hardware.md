
HARDWARE

## 1. Flight Controller

| Component         | Model                               | Qty | Status      |
| ----------------- | ----------------------------------- | --- | ----------- |
| Flight Controller | **TI EK-TM4C123GXL (TM4C123GH6PM)** | 1   | **Primary** |

---

## 2. Flight Sensors

| Component     | Model      | Qty | Status      |
| ------------- | ---------- | --- | ----------- |
| Sensor Module | **GY-87**  | 1   | **Primary** |
| MPU6050       | Integrated | —   | Primary     |
| HMC5883L      | Integrated | —   | Primary     |
| BMP180        | Integrated | —   | Primary     |

---

## 3. Navigation

| Component | Model              | Qty | Status  |
| --------- | ------------------ | --- | ------- |
| GPS       | **u-blox NEO-M8N** | 1   | Primary |

---

# 4. Wireless Communication

### Primary

| Component | Model         | Qty |
| --------- | ------------- | --- |
| XBee S2C  | Digi XBee S2C | 2   |

---

### Backup

| Component                                | Model        | Qty | Purpose                    |
| ---------------------------------------- | ------------ | --- | -------------------------- |
| **HC-12 433 MHz Wireless Serial Module** | SI4463 Based | 2   | Backup Telemetry / RC Link |

The firmware architecture will abstract the communication interface, allowing either XBee or HC-12 to be selected without changing the control logic.

---

# 5. Electronic Speed Controllers

### Primary

| Component  | Model | Qty |
| ---------- | ----- | --- |
| SimonK ESC | 30A   | 4   |

---

### Backup / Upgrade

| Component                                              | Model | Qty | Status                  |
| ------------------------------------------------------ | ----- | --- | ----------------------- |
| **JHEMCU EM-40A 2S–6S BLHeli_S 4-in-1 ESC (DShot600)** | 40A   | 1   | Backup / Future Upgrade |

### Notes

* Supports PWM, OneShot, MultiShot, and **DShot600** (depending on firmware and controller support).
* Can simplify wiring compared to four individual ESCs.
* Your current firmware targets **standard PWM (400 Hz)**. Using DShot600 would require a future digital motor-output driver.

---

# 6. Motors

| Component       | Model        | Qty |
| --------------- | ------------ | --- |
| Brushless Motor | A2212 1000KV | 4   |

---

# 7. Propellers

| Component | Qty |
| --------- | --- |
| 1045 CW   | 2   |
| 1045 CCW  | 2   |

---

# 8. Frame

| Component                | Model            |
| ------------------------ | ---------------- |
| Frame                    | F450 Glass Fiber |
| Landing Gear             | F450 Compatible  |
| Power Distribution Board | XT60 Compatible  |

---

# 9. Power

| Component      | Specification       |
| -------------- | ------------------- |
| LiPo Battery   | 3S/4S 2200–5200 mAh |
| XT60 Connector | Yes                 |
| Battery Strap  | Yes                 |

---

# 10. Ground Station

| Component        |
| ---------------- |
| Laptop           |
| Xbox Controller  |
| XBee USB Adapter |

---

# 11. Development Tools

* Code Composer Studio (CCS)
* TivaWare
* Git & GitHub
* XCTU (XBee)
* u-center (GPS)
* USB-UART Adapter
* Logic Analyzer (Recommended)
* Oscilloscope (Optional)

---

# Communication Architecture

```text
                 Ground Station

              Xbox Controller
                     │
              Laptop Software
                     │
        ┌────────────┴────────────┐
        │                         │
     XBee S2C                HC-12
      (Primary)             (Backup)
        │                         │
        └────────────┬────────────┘
                     │
                  UART1
                     │
                 TM4C123
```

---

# Motor Control Architecture

```text
                Mixer
                  │
                  ▼
             PWM Driver
                  │
        ┌─────────┴─────────┐
        │                   │
   SimonK ESC ×4      JHEMCU 4-in-1 ESC
    (Primary)           (Backup)
```

---

# Important Design Decision

I suggest we **keep the firmware based on standard PWM (1000–2000 µs at 400 Hz)** for Phase 1 because:

* It works with the SimonK ESCs.
* The JHEMCU EM-40A also supports PWM input (assuming it's running compatible BLHeli_S firmware), so it can be used as a backup without changing the motor-control code.
* If you later decide to use **DShot600**, we'll add a new `DShotDriver` module instead of modifying the existing PWM driver. That keeps the code modular and avoids breaking a working implementation.

This approach gives you a stable flight controller now while leaving a clear upgrade path to digital ESC protocols in the future.
