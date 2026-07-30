# STM32_C031C6_LEDMATRIX_MAX7219_Driver

A simple, bare-metal SPI driver to control the **MAX7219** IC, demonstrated on an 8x8 LED Matrix Module. 

This project includes a fully functional example application that drives the display with two distinct patterns: a cyclic display of numbers, the alphabet, and symbols, as well as an interactive "moving dot" pattern.

All peripheral drivers—including SPI, UART (for tracing), and BSP (Board Support Package)—are written entirely from scratch in C (bare-metal register level). STM32Cube was only used to generate the base device headers, startup code, and linker scripts.

---

## 🛠️ Hardware & Toolchain

### Hardware Used
*   **MCU:** STM32C031C6 (typically on a Nucleo board featuring User Button B1 and Reset Button B2).
*   **Display Module:** MAX7219 Dot Matrix Module V02 (by Elegoo). 
    *   *Note: While this example targets an 8x8 matrix, the driver is flexible and can be adapted to drive 7-segment displays or custom LED arrangements powered by the MAX7219.*

### Software & Toolchain
*   **IDE:** Keil uVision 5 (Non-Commercial Edition)
*   **Compiler:** ARM Compiler 6 (armclang)
*   **Headers/Startup:** STM32CubeMX

---

## 🚀 Quick Start Guide

1. **Clone the repository** and open the Keil uVision project file (`.uvprojx`).
   
   *(Add screenshot of Keil uVision project explorer here)*
   `![Keil uVision Setup](docs/images/keil_setup.png)`

2. **Configure your Debugger/Flasher** (e.g., ST-LINK) in the Keil project options.
3. **Build the target** (F7) and **Load/Flash** the code (F8) to the STM32 board.
4. **Reset the board** using the **B2** push button.

### Operating the Demo
Upon resetting the board, the display will immediately begin cycling through numbers, letters, and symbols. 
*   **Press User Button B1** to switch to the "Moving Dot" pattern. In this mode, a single LED advances sequentially across the 8x8 matrix grid.
*   **Press B1 again** to seamlessly toggle back to the alphanumeric pattern.

---

## 🐞 Debugging & Extras

### 1. SPI Protocol Decoding (Logic Analyzer)
For hardware debugging and verifying the driver, you can use a Logic Analyzer (such as a Saleae clone) connected to the SPI bus:
*   **Pins:** Connect to `CS`, `GND`, `MOSI`, and `CLK`.
*   **Software:** [PulseView](https://sigrok.org/wiki/PulseView) by sigrok. 
*   PulseView features a built-in **MAX7219 protocol decoder** that translates the raw SPI bitstream directly into MAX7219 register commands!

   *(Add screenshot of PulseView decoder here)*
   `![PulseView Decoder](docs/images/pulseview_decoder.png)`

### 2. DMA-UART Tracing (Virtual COM)
The project includes a robust, non-intrusive UART driver utilizing a dedicated DMA channel for debug tracing. The USART2 peripheral is internally routed to the USB Virtual COM port on the STM32 Nucleo board.

**To use the API in your code:**
```c
UART2_PrintDma("LED Green ON\n\r");