# STM32_C031C6_LEDMATRIX_MAX7219_Driver

A simple, bare-metal SPI driver to control the **MAX7219** IC, demonstrated on an 8x8 LED Matrix Module. 

This project includes a fully functional example application that drives the display with two distinct patterns: a cyclic display of numbers, the alphabet, and symbols, as well as an interactive "moving dot" pattern.

All peripheral drivers—including SPI, UART (for tracing), and BSP (Board Support Package)—are written entirely from scratch in C (bare-metal register level). STM32Cube was only used to generate the base device headers, startup code, and linker scripts.

---

## 🛠️ Hardware & Toolchain

### Hardware Used
*   **MCU:** STM32C031C6 (typically on a Nucleo board featuring User Button B1 and Reset Button B2).
<img width="660" height="420" alt="image" src="https://github.com/user-attachments/assets/e43d5b70-9e2d-4fbd-abf1-c752cfa9eb58" />

*   **Display Module:** MAX7219 Dot Matrix Module V02 (by Elegoo). 
    *   *Note: While this example targets an 8x8 matrix, the driver is flexible and can be adapted to drive 7-segment displays or custom LED arrangements powered by the MAX7219.*
<img width="442" height="313" alt="image" src="https://github.com/user-attachments/assets/ed2ff3ce-66b8-4cd3-9274-0903db022e80" />

* **Connection:**
  * The board is connected to a PC via USB (The onboard ST-LINK acts as a Virtual COM port wired to USART2).
  * The MAX7219 moudle is connected as follows:
  * GND->pin20CN7
  * VCC->pin18CN7
  * SCLK->PA1
  * CS->PA4
  * MOSI->PA7
<img width="980" height="780" alt="image" src="https://github.com/user-attachments/assets/8f480ee9-1b8f-490a-a3ae-4895ffd42c27" />

### Software & Toolchain
*   **IDE:** Keil uVision 5 (Non-Commercial Edition)
*   **Compiler:** ARM Compiler 6 (armclang)
*   **Headers/Startup:** STM32CubeMX
<img width="661" height="138" alt="image" src="https://github.com/user-attachments/assets/631f09a1-b007-4b04-8673-2b8f82c2d492" />

---

## 🚀 Quick Start Guide

1. **Clone the repository** and open the Keil uVision project file (`.uvprojx`).
<img width="1117" height="440" alt="image" src="https://github.com/user-attachments/assets/00a360a1-a30c-4a78-9ed2-4b733633f32b" />



2. **Configure your Debugger/Flasher** (e.g., ST-LINK) in the Keil project options.
<img width="618" height="466" alt="image" src="https://github.com/user-attachments/assets/34217ead-2477-4be0-a840-9459f979d34e" />

3. **Build the target** (F7) and **Load/Flash** the code (F8) to the STM32 board.
4. **Reset the board** using the **B2** push button.

### Operating the Demo
Upon resetting the board, the display will  begin cycling through numbers, letters, and symbols. 

*   **Press User Button B1** to switch to the "Moving Dot" pattern. In this mode, a single LED advances across the 8x8 matrix grid.
<br>
<img width="300" alt="Moving Dot Pattern" src="https://github.com/user-attachments/assets/bd5fb5ab-fc03-460b-ad91-e8542eaa5b63" />

*   **Press B1 again** to toggle back to the alphanumeric pattern.
<br>
<img width="300" alt="Alphanumeric Pattern" src="https://github.com/user-attachments/assets/0157116c-0a3f-4ac7-a6f3-8df7af08872c" />
---

## 🐞 Debugging & Extras

### 1. SPI Protocol Decoding (Logic Analyzer)
For hardware debugging and verifying the driver, you can use a Logic Analyzer (such as a Saleae clone) connected to the SPI bus:
*   **Pins:** Connect to `CS`, `GND`, `MOSI`, and `CLK`.
*   **Software:** [PulseView](https://sigrok.org/wiki/PulseView) by sigrok. 
*   PulseView features a built-in **MAX7219 protocol decoder** that translates the raw SPI bitstream directly into MAX7219 register commands!
<img width="1892" height="584" alt="image" src="https://github.com/user-attachments/assets/84dca6b9-6e76-4c8c-babc-f97a4c9085f4" />


### 2. DMA-UART Tracing (Virtual COM)
The project includes a non-intrusive UART driver utilizing a dedicated DMA channel for debug tracing or future uses. The USART2 peripheral is internally routed to the USB Virtual COM port on the STM32 Nucleo board.
<img width="626" height="231" alt="image" src="https://github.com/user-attachments/assets/bb3979e6-7ccf-457f-ad4c-318a992e5fdb" />


**To use the API in your code:**
```c
UART2_PrintDma("LED Green ON\n\r");
```
---

## 📄 License & Usage
This project is open-source and intended for educational, hobbyist, and non-commercial purposes. Feel free to fork, modify, and use this code as a learning reference for bare-metal STM32 programming!

