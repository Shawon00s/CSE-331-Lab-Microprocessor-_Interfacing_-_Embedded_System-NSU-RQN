# C Codes

This folder contains only C source code files for the STM32 lab work:

- `blink_external.c`
- `built_in_led.c`
- `Hello World.c`
- `image on OLED.c`
- `scrolling_text.c`

These are just C codes. Before running them, you need to set up the pin configuration in STM32CubeMX, generate the project files, and then copy the code into Keil software to build and run the project.

## Basic Workflow

1. Open STM32CubeMX.
2. Configure the required pins, clock, and peripherals.
3. Generate the Keil project files.
4. Open the generated project in Keil.
5. Copy and paste the C code into the appropriate source file.
6. Build and flash the program to the board.

## Notes

- Make sure the pin configuration in CubeMX matches your hardware connection.
- If the code uses GPIO, UART, OLED, or other peripherals, configure those peripherals first in CubeMX.
- Update the code if needed to match your microcontroller and board setup.
