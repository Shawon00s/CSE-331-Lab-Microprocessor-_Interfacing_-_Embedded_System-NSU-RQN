# DHT11 with OLED and STM32 LED Control

## Hardware Lab Final Question
The question of the Hardware Lab Final was to read temperature and humidity from a DHT11 sensor, show the values on an OLED display, and control the STM32 LED according to the temperature.

## What the System Does
The STM32 continuously reads the DHT11 sensor and updates the OLED display with the current temperature and humidity values.

The LED logic is:

- If the temperature is less than 28 degrees Celsius, the STM32 LED blinks
- If the temperature is 28 degrees Celsius or higher, the STM32 LED stays ON

## Main Parts Used
- STM32 microcontroller
- DHT11 temperature and humidity sensor
- OLED display
- Onboard STM32 LED

## Working Idea
This lab question combines sensor reading, display output, and simple decision-based control. The OLED gives a live view of the environment, while the LED provides a clear visual indication of whether the temperature is below or above the threshold.

## Short Summary
This lab final question demonstrates how an STM32 can read environmental data from a DHT11 sensor, display the measurements on an OLED, and change LED behavior based on a temperature limit.
