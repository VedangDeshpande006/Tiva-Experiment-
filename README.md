

# **Autonomous Quadcopter Control Platform Using ARM Cortex-M4**

## I. Aim

The objective of this project is to design, develop, and validate a custom high-performance digital flight controller for an F450 quadcopter using the **Texas Instruments TM4C123G (ARM Cortex-M4F)** microcontroller. The controller operates at a deterministic refresh frequency of **250 Hz**, enabling real-time attitude stabilization through cascaded PID control.

Unlike conventional hobby drones that rely on analog PPM/SBUS radio receivers, this platform employs a completely digital telemetry architecture using **XBee S2C wireless modules**, allowing structured binary command packets generated from an Xbox controller connected to a laptop. The system integrates multiple onboard sensors—including an IMU, magnetometer, barometer, and GPS—to provide accurate state estimation and establish a scalable foundation for future autonomous navigation.

---

# II. Hardware Architecture

| Component                          | Specification                                                                | Interface                |
| ---------------------------------- | ---------------------------------------------------------------------------- | ------------------------ |
| **TM4C123GH6PM (Tiva C Series)**   | 32-bit ARM Cortex-M4F, 80 MHz, Floating Point Unit                           | Main Flight Controller   |
| **GY-87 Sensor Module**            | MPU6050 (Accelerometer + Gyroscope), HMC5883L Magnetometer, BMP180 Barometer | I²C0                     |
| **u-Blox NEO-M8N GPS**             | High Precision GNSS Receiver                                                 | UART                     |
| **XBee S2C (2 Units)**             | 2.4 GHz Wireless Serial Telemetry                                            | UART1                    |
| **2200 mAh Li-Po Battery (3S/4S)** | Primary Power Source with XT60 Connector                                     | Power Distribution Board |
| **A2212 Brushless Motors (×4)**    | Outrunner BLDC Motors                                                        | ESC Driven               |
| **SimonK 30A ESC (×4)**            | High Refresh Electronic Speed Controllers                                    | PWM (400 Hz)             |
| **F450 Quadcopter Frame**          | 450 mm Glass Fiber Frame                                                     | Mechanical Structure     |
| **Xbox Controller**                | Pilot Input Device                                                           | USB to Laptop            |

---

# III. System Architecture

```
          Xbox Controller
                 │
                 ▼
            Laptop Application
                 │
         Binary Control Packets
                 │
          XBee S2C Transmitter
                 │
          Wireless Telemetry
                 │
          XBee S2C Receiver
                 │
          TM4C123 Flight Controller
                 │
      ┌──────────┴───────────┐
      │                      │
      ▼                      ▼
 IMU + Sensors           GPS Module
      │                      │
      └──────────┬───────────┘
                 ▼
        State Estimation
                 │
          Cascaded PID
                 │
           Motor Mixer
                 │
          PWM Generation
                 │
        SimonK ESC (×4)
                 │
          Brushless Motors
```

---

# IV. Software Development Roadmap

## Phase 1 – Sensor Interfacing (Completed)

### Objective

Establish reliable communication with onboard sensors and isolate hardware drivers from flight algorithms.

### Tasks

* Configure I²C0 peripheral.
* Read raw MPU6050 accelerometer and gyroscope values.
* Interface BMP180 barometer.
* Interface HMC5883L magnetometer.
* Store sensor values inside a centralized FlightState structure.
* Separate hardware abstraction layer from application logic.

### Output

A clean software architecture where every sensor updates a common system state without tightly coupling application logic to hardware drivers.

---

## Phase 2 – Wireless Communication and Failsafe (Completed)

### Objective

Replace conventional RC receivers with a digital communication system.

### Features

* UART communication at **115200 baud**
* Transparent XBee wireless link
* Binary packet parser
* Xbox joystick command decoding
* Checksum verification
* Communication timeout detection

### Binary Packet

| Byte | Description    |
| ---- | -------------- |
| 0    | Header         |
| 1    | Roll           |
| 2    | Pitch          |
| 3    | Throttle       |
| 4    | Yaw            |
| 5–8  | Auxiliary Data |
| 9    | Checksum       |

### Failsafe Logic

If no valid packet is received for **500 milliseconds**:

* Immediately set throttle to zero.
* Disable motor outputs.
* Reset PID integrators.
* Wait until communication resumes.

---

## Phase 3 – Deterministic Flight Loop (In Progress)

### Objective

Implement a fixed-frequency real-time scheduler.

### Method

A hardware timer generates an interrupt every

[
T=\frac{1}{250}=4\text{ ms}
]

Each interrupt executes the complete flight control cycle.

### Execution Order

```
Timer Interrupt

↓

Read Sensors

↓

Estimate Orientation

↓

Receive Wireless Commands

↓

Execute PID

↓

Motor Mixing

↓

Update PWM Outputs
```

Using a hardware timer guarantees deterministic execution regardless of processing load.

---

## Phase 4 – Cascaded PID Attitude Control

### Objective

Maintain stable quadcopter orientation under disturbances.

### Outer Loop

Computes desired angle error.

```
Desired Angle

↓

Angle Error

↓

Desired Angular Velocity
```

### Inner Loop

Uses gyroscope feedback.

```
Desired Rate

↓

Rate Error

↓

PID Controller

↓

Motor Correction
```

### Advantages

* Faster response
* Better disturbance rejection
* Improved hover stability
* Reduced oscillations

---

# V. Sensor Fusion

The quadcopter combines information from multiple sensors.

### MPU6050

Provides

* Roll rate
* Pitch rate
* Linear acceleration

### HMC5883L

Provides

* Heading
* Compass direction

### BMP180

Provides

* Atmospheric pressure
* Relative altitude

### NEO-M8N

Provides

* Latitude
* Longitude
* Ground speed
* UTC time

These measurements are combined to estimate the vehicle's complete state.

---

# VI. Motor Mixing

For an X-configuration quadcopter:

```
Motor 1 = Throttle + Pitch + Roll − Yaw

Motor 2 = Throttle + Pitch − Roll + Yaw

Motor 3 = Throttle − Pitch − Roll − Yaw

Motor 4 = Throttle − Pitch + Roll + Yaw
```

The resulting motor commands are constrained within allowable PWM limits before transmission to the ESCs.

---

# VII. PWM Generation

Each SimonK ESC receives a hardware PWM signal.

Typical pulse widths:

| Command | Pulse Width  |
| ------- | ------------ |
| Minimum | 1000 µs      |
| Hover   | 1450–1550 µs |
| Maximum | 2000 µs      |

PWM signals are generated using the TM4C123 hardware PWM module to ensure low jitter and precise timing.

---

# VIII. Safety Features

The platform incorporates several protective mechanisms:

* Wireless communication timeout
* Packet checksum validation
* Motor disarm on signal loss
* PID integral wind-up prevention
* PWM output saturation
* Sensor initialization verification
* Safe startup sequence
* Controlled arming/disarming logic

These features improve operational reliability and reduce the risk of uncontrolled flight.

---

# IX. Expected Performance

| Parameter                   | Value                                    |
| --------------------------- | ---------------------------------------- |
| Flight Controller Frequency | 250 Hz                                   |
| PWM Refresh Rate            | 400 Hz                                   |
| MCU Clock                   | 80 MHz                                   |
| Wireless Baud Rate          | 115200 bps                               |
| Communication Timeout       | 500 ms                                   |
| Number of Motors            | 4                                        |
| Position Sensors            | GPS + Barometer                          |
| Attitude Sensors            | Accelerometer + Gyroscope + Magnetometer |

---

# X. Future Scope

The modular architecture allows straightforward expansion into autonomous flight capabilities, including:

* GPS waypoint navigation
* Autonomous takeoff and landing
* Return-to-home (RTH)
* Altitude hold
* Position hold
* Loiter mode
* Autonomous mission planning
* Obstacle detection and avoidance
* MAVLink telemetry integration
* Ground Control Station (GCS) support
* Companion computer integration (e.g., Raspberry Pi)
* Vision-based navigation using cameras and computer vision

---

# XI. Conclusion

This project demonstrates the successful development of a deterministic, high-frequency digital flight control platform based on the ARM Cortex-M4 architecture. By replacing traditional analog RC communication with a robust XBee-based binary telemetry protocol, the system achieves a simplified hardware architecture while maintaining reliable manual control and safety through checksum verification and communication failsafes.

The modular software design, deterministic 250 Hz control loop, cascaded PID stabilization, and integrated sensor suite establish a scalable foundation for advanced autonomous flight functions. With support for GPS-based localization, real-time telemetry, and future navigation algorithms, the platform provides a robust framework for research, academic experimentation, and further development toward fully autonomous quadcopter operations.
