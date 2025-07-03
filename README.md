# Aks-SmartCar

## Overview
Aks-SmartCar is a smart car project based on the STM32F407 microcontroller. The project demonstrates embedded system design, motor control, GPIO management, and real-time operation using STM32 HAL libraries. It is suitable for educational purposes, robotics competitions, and as a foundation for more advanced autonomous vehicle projects.

## Features
- **Microcontroller:** STM32F407
- **Motor Control:** PWM-based speed and direction control
- **GPIO Management:** Input/output for sensors and actuators
- **Timer Configuration:** For precise control and periodic tasks
- **Modular Code Structure:** Easy to extend and maintain
- **CMake Build System:** Cross-platform and IDE-friendly

## Directory Structure
```
Aks-SmartCar.ioc           # STM32CubeMX project file
CMakeLists.txt             # Main CMake build script
Core/
  Inc/                     # Header files
  Src/                     # Source files
Drivers/                   # CMSIS and HAL drivers
startup_stm32f407xx.s      # Startup assembly code
STM32F407XX_FLASH.ld       # Linker script
README.md                  # Project documentation
```

## Getting Started
### Prerequisites
- STM32CubeMX
- STM32CubeIDE or CLion (with STM32 plugin)
- ARM GCC Toolchain (e.g., gcc-arm-none-eabi)
- CMake 3.15+
- ST-Link or compatible programmer/debugger

### Building the Project
1. **Clone the repository:**
   ```sh
   git clone <repo-url>
   cd Aks-SmartCar
   ```
2. **Configure the project with CMake:**
   ```sh
   cmake -Bcmake-build-debug-stm32 -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake
   ```
3. **Build the firmware:**
   ```sh
   cmake --build cmake-build-debug-stm32
   ```
4. **Flash to the board:**
   Use STM32CubeProgrammer or OpenOCD to flash `Aks-SmartCar.elf` to your STM32F407 board.

### Opening in STM32CubeIDE
- Open the `.ioc` file in STM32CubeIDE to modify pin configurations or regenerate code.

## Code Structure
- **Core/Inc/**: Header files for main application, peripherals, and user modules.
- **Core/Src/**: Source files implementing main logic, peripheral initialization, and user modules.
- **Drivers/**: CMSIS and STM32 HAL drivers.
- **cmake/**: CMake toolchain and STM32CubeMX integration scripts.

## Main Modules
- `main.c`: Entry point, system initialization, main loop.
- `motor.c/h`: Motor control logic (PWM, direction, speed).
- `gpio.c/h`: GPIO initialization and control.
- `tim.c/h`: Timer configuration for PWM and periodic interrupts.
- `stm32f4xx_it.c/h`: Interrupt handlers.

## Motor Control Example
```c
// Set motor speed and direction
Motor_SetSpeed(MOTOR_LEFT, 80);   // 80% speed
Motor_SetDirection(MOTOR_LEFT, FORWARD);
```

## Customization
- **Add Sensors:** Connect sensors to available GPIOs and add initialization code in `gpio.c`.
- **Modify Motor Logic:** Update `motor.c` for different motor drivers or control algorithms.
- **Expand Functionality:** Add new modules in `Core/Src` and declare interfaces in `Core/Inc`.

## Debugging
- Use ST-Link or J-Link for debugging via SWD.
- Set breakpoints in your IDE or use `printf` for UART debugging.

## Troubleshooting
- **Build Errors:** Ensure toolchain and CMake are correctly installed and configured.
- **Flashing Issues:** Check connections, power supply, and driver installation.
- **Runtime Errors:** Use debugger to step through code and inspect variables.

## License
This project uses open-source STM32 HAL and CMSIS drivers. See `Drivers/STM32F4xx_HAL_Driver/LICENSE.txt` and `Drivers/CMSIS/LICENSE.txt` for details.

## References
- [STM32F4 Reference Manual](https://www.st.com/resource/en/reference_manual/dm00031020.pdf)
- [STM32CubeMX Documentation](https://www.st.com/en/development-tools/stm32cubemx.html)
- [CMake Documentation](https://cmake.org/cmake/help/latest/)
- [ARM GCC Toolchain](https://developer.arm.com/downloads/-/gnu-rm)

## Contact
For questions or contributions, please open an issue or submit a pull request.
