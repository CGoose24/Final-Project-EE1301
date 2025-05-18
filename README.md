# Final-Project-EE1301
# 'StepSnitch' Smart Motion Detection and Alert System

This project implements a smart motion detection and alert system using the Particle Photon 2 microcontroller, WS2812 RGB LEDs, a passive infrared (PIR) motion sensor, and a piezoelectric buzzer. It includes real-time motion tracking, visual and audio alerts, cloud connectivity, and interactive data visualization through a web-based dashboard. Created by Carson Gusaas and Jonas McClung as a final project for the class EE1301: Introduction to Computing Systems.

## Features
- **Motion Detection**: Detects motion via a PIR sensor and logs the number of events with timestamps.
- **RGB LED Feedback**: Displays system mode and activity using a 3-LED WS2812 strip.
- **Buzzer Alerts**: Emits a 1-second audio alert on motion detection when alarm mode is enabled.
- **Cloud Integration**: Remotely monitor system status and control operating modes using Particle Cloud functions and variables.
- **Dynamic Dashboard**: Visualizes motion history in real time using a JavaScript chart updated every 10 minutes.
- **Email Alerts via IFTTT**: Motion events trigger email notifications using Particle’s Webhooks and IFTTT integration.

## Hardware

- Particle Photon 2
- PIR motion sensor
- WS2812 RGB LED strip (3 LEDs)
- Piezoelectric buzzer
- Breadboard and jumper wires

## Firmware Overview

The firmware is written in C++ and includes the following functionality:
- Initializes and monitors the motion sensor on `D2`.
- Controls WS2812 LEDs over SPI to indicate system and alarm states.
- Triggers buzzer alerts on motion if the alarm is active.
- Tracks and stores motion event counts in 10-minute intervals using a 144-element array (24 hours of data).
- Publishes device status and history to the Particle Cloud for dashboard retrieval.
- Exposes cloud functions to toggle device mode and alarm mode.

##  Web Dashboard **(Mostly AI Generated)**
**Because HTML programming was outside the scope of this course, we were allowed to use generative AI for the webpage portion of this project**

The HTML dashboard connects to Particle Cloud to:
- Display total motion count
- Show device and alarm states
- Graph motion detection history with timestamps in 10-minute resolution
- Allow toggling of modes via buttons

Makes use of:
- JavaScript
- Chart.js for dynamic plotting
- Particle Cloud JS SDK

## Design Considerations
- **Non-blocking Logic**: All time-sensitive actions (e.g., buzzer beeping, timeline shifting) are handled using time and state variables instead of `delay()`, ensuring the main loop remains responsive.
- **Motion Debouncing**: Motion detection is managed with state tracking to prevent redundant log entries and flickering LEDs.
- **Efficient State Handling**: The system uses state flags (`currentMotion`, `motionLogged`, etc.) to coordinate sensor readings, alerts, and UI updates smoothly.
- **Real-time Feedback**: RGB LEDs and the buzzer reflect current system state in real time without blocking other operations.
- **Readable Timestamps**: Formatted timestamps make it easy to identify when motion events occurred.
- **Optimized Dashboard Updates**: API calls to the Particle Cloud are spaced appropriately to prevent rate-limiting and maintain performance.

## Future Improvements
- Design and 3D-print a protective shell to house the electronics and make the system deployable as a standalone product.
- Use persistent storage for motion history.
- Enhance security with authentication on dashboard access.

## Breadboard Wiring Diagram
![StepSnitch](https://github.com/user-attachments/assets/b2a80ca5-e9bd-4d22-8466-c7e1b9449c69)
