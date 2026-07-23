

# Autonomous Quadcopter Flight Controller

## Hardware Specification (LOCKED)

## 1. Main Flight Controller

| Component         | Specification                  |
| ----------------- | ------------------------------ |
| MCU               | Texas Instruments TM4C123GH6PM |
| Development Board | EK-TM4C123GXL Rev B            |
| Core              | ARM Cortex-M4F                 |
| Clock             | 80 MHz                         |
| FPU               | Enabled                        |
| Control Loop      | 250 Hz                         |
| Motor Update Rate | 400 Hz PWM                     |

---

# 2. Flight Sensors

## GY-87

Integrated Sensors

* MPU6050
* HMC5883L
* BMP180

Interface

**I²C0**

| GY-87 | TM4C |
| ----- | ---- |
| VCC   | 3.3V |
| GND   | GND  |
| SDA   | PB3  |
| SCL   | PB2  |

Reserved I²C Address Space

```
MPU6050
HMC5883L
BMP180
```

---

# 3. GPS

Module

```
u-blox NEO-M8N
```

Interface

UART3

| GPS | TM4C      |
| --- | --------- |
| TX  | PC6 (RX3) |
| RX  | PC7 (TX3) |
| VCC | 3.3V      |
| GND | GND       |

Baud

```
9600
```

(configurable later)

---

# 4. Wireless Telemetry / RC

## Primary

```
XBee S2C
```

UART1

| XBee | TM4C |
| ---- | ---- |
| DIN  | PC5  |
| DOUT | PC4  |
| VCC  | 3.3V |
| GND  | GND  |

Baud

```
115200
```

---

## Backup

```
HC-12
```

Uses the **same UART1 interface**.

Simply disconnect XBee and connect HC-12.

No firmware changes required.

---

# 5. Debug Interface

UART0

| Signal | TM4C |
| ------ | ---- |
| RX     | PA0  |
| TX     | PA1  |

Used for

* printf
* PID tuning
* Sensor debugging
* Flight logs

---

# 6. Motor Outputs

## Primary ESC

```
SimonK 30A ESC ×4
```

Input

```
PWM
1000–2000 µs
400 Hz
```

---

## Backup ESC

```
JHEMCU EM-40A
BLHeli_S
4-in-1 ESC
```

Input

```
PWM
```

Future

```
DShot600
```

will be added in Phase 3 without changing the architecture.

---

# Official Motor Layout

```
                FRONT

          M1(CCW)     M2(CW)

               ↑

          M4(CW)      M3(CCW)

                REAR
```

---

Motor Numbering

| Motor | Position    | Rotation |
| ----- | ----------- | -------- |
| M1    | Front Left  | CCW      |
| M2    | Front Right | CW       |
| M3    | Rear Right  | CCW      |
| M4    | Rear Left   | CW       |

This numbering is permanently frozen.

---

# Official Mixer

```
M1 = T + R - P - Y

M2 = T - R - P + Y

M3 = T - R + P - Y

M4 = T + R + P + Y
```

---

# PWM Outputs

After considering the TM4C123 PWM architecture, we'll dedicate these pins:

| Motor | PWM Output | TM4C Pin |
| ----- | ---------- | -------- |
| M1    | PWM0       | PB6      |
| M2    | PWM1       | PB7      |
| M3    | PWM2       | PB4      |
| M4    | PWM3       | PB5      |

PWM Frequency

```
400 Hz
```

Pulse Width

```
1000–2000 µs
```

---

# 7. LEDs

| LED   | Pin | Function       |
| ----- | --- | -------------- |
| Red   | PF1 | Failsafe/Error |
| Blue  | PF2 | Calibration    |
| Green | PF3 | Armed          |

---

# 8. Power System

Battery

```
3S/4S LiPo
```

↓

Power Distribution Board

↓

```
ESC1
ESC2
ESC3
ESC4
```

↓

```
5V BEC
```

↓

TM4C LaunchPad

Sensors use

```
3.3V
```

---

# 9. Reserved Interfaces

| Peripheral | Purpose                              |
| ---------- | ------------------------------------ |
| UART5      | Raspberry Pi Companion Computer      |
| UART6      | MAVLink Radio                        |
| SSI0 (SPI) | High-Speed Sensors                   |
| I²C1       | ToF / Optical Flow / Future Sensors  |
| GPIO       | Buzzer, Camera Trigger, Landing Gear |

---

# Complete Pin Layout

| TM4C Pin | Peripheral | Connected Device |
| -------- | ---------- | ---------------- |
| PA0      | UART0 RX   | Debug            |
| PA1      | UART0 TX   | Debug            |
| PB2      | I²C0 SCL   | GY-87            |
| PB3      | I²C0 SDA   | GY-87            |
| PB4      | PWM2       | ESC3 (M3)        |
| PB5      | PWM3       | ESC4 (M4)        |
| PB6      | PWM0       | ESC1 (M1)        |
| PB7      | PWM1       | ESC2 (M2)        |
| PC4      | UART1 RX   | XBee / HC-12     |
| PC5      | UART1 TX   | XBee / HC-12     |
| PC6      | UART3 RX   | GPS              |
| PC7      | UART3 TX   | GPS              |
| PF1      | GPIO       | Red LED          |
| PF2      | GPIO       | Blue LED         |
| PF3      | GPIO       | Green LED        |

---

# Complete System Architecture

```text
                    Xbox Controller
                           │
                    Laptop Application
                           │
                ┌──────────┴──────────┐
                │                     │
            XBee S2C              HC-12
            (Primary)            (Backup)
                │                     │
                └──────────┬──────────┘
                           │
                        UART1
                           │
                    TM4C123GH6PM
                           │
      ┌──────────────┬──────────────┐
      │              │              │
    GY-87          GPS          Debug UART
    I²C0          UART3          UART0
      │              │
      └──────┬───────┘
             ▼
      State Estimation
             ▼
      Kalman Filter
             ▼
      Cascaded PID
             ▼
           Mixer
             ▼
      Hardware PWM (400 Hz)
             ▼
      ┌──────────────┬──────────────┐
      │              │
 SimonK ESC ×4   JHEMCU EM-40A
  (Primary)        (Backup)
             ▼
      A2212 Motors ×4
             ▼
         F450 Frame
```

---

# Software Assumptions (Frozen)

* **250 Hz** deterministic control loop.
* **400 Hz** hardware PWM output.
* Standard PWM protocol for Phase 1.
* Communication abstraction allows **XBee** or **HC-12** on the same UART without code changes.
* ESC abstraction allows **SimonK** or **JHEMCU EM-40A** using PWM. A future `DShotDriver` can be added alongside the PWM driver without affecting the flight-control logic.
* Motor numbering, mixer equations, and communication interfaces are now **locked**.

