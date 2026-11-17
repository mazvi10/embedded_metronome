# ENGS 28 Final Project : Embedded Metronome

Authors: Mazvita Nhidza and Amelia Smith

Date: 11.17.26

## Project Description

## Features
- Python script to calculate the BPM of any supplied noise
- Potentiometer control of the BPM when bluetooth is disconnected
- Two LEDs that alternate at the selected BPM
- Servo that taps at the half frequency of the BPM
- Sonar sensor that detects nearby objects and activates the servo
- Seven-segment display with three display modes
  - BPM Mode: Displays the current beats per minute
  - Beat Mode: Displays "bEAt" in sync with the LEDs
  - Speed Mode: Displays the current speed setting (FAST or SLO)

## Hardware Requirements / BOM
- STM32 Nucleo-C031C6
- Two potentiometers
- Two (or more) LEDS
- HC-SR04 sonar sensor
- Tower Pro SG92R Micro Servo
- Seven-segment display
- Adafruit Bluefruit LE UART Friend
- Barrel-jack adapter
- 2 resistors (11kΩ)
- Breadboard
- Jumper wires

## Software Requirements
- STM32CubeIDE
- Embedded C

## Hardware Connections

| Component | STM32 Pin | Configuration |
| --- | --- | --- |
| Potentiometer 1 | PA0 | INPUT |
| Potentiometer 2 | PA1 | INPUT |
| LED 1 | PA5 | OUTPUT |
| LED 2 | PA6 | OUTPUT |
| Bluetooth RX | PA9 | ALTERNATE FUNCTION |
| Bluetooth TX | PA10 | ALTERNATE FUNCTION |
| Bluetooth RTS | PA11 | ALTERNATE FUNCTION |
| Bluetooth CTS | PA12 | ALTERNATE FUNCTION |
| Sonar Sensor Trigger | PB4 | OUTPUT |
|Sonar Sensor Echo | PB5 | INPUT |
|Seven-Segment Display | PB8 | OUTPUT |
|Seven-Segment Display | PB9 | OUTPUT |

## File Descriptions

| File | Description |
|------|-------------|
| `FinalProject.c` | Main program that coordinates the project components, including Bluetooth communication, potentiometer readings, LED control, servo operation, and seven-segment display updates. |
| `SonarSensor.h` | Contains the initialization and measurement functions for the ultrasonic sonar sensor. |
| `SevenSegPartial.h` | Contains functions for initializing and controlling the seven-segment display. |
| `ADC.h` | Contains functions for configuring the ADC and reading analog inputs from the potentiometers. |
| `tim.h` | Contains timer initialization, interrupt configuration, and PWM functions used to control timing and the servo motor. |
| `uart.h` | Contains functions for initializing UART communication and transmitting and receiving Bluetooth data. |
| `i2c.h` | Contains functions for initializing and managing I2C communication. |
| `ES28.h` | Contains board-specific definitions, register configurations, and utility functions used throughout the project. |



  
