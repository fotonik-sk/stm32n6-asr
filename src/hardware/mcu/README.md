# mcu

The lowest part of the hardware layer: what belongs to the STM32N657
microcontroller itself and nothing about the board. It holds the startup code
and the memory map, the CMSIS and HAL libraries from ST, the setup of the
caches and clocks, and the access rights of the NPU and the display controller
to memory.

## Structure

| Object | Role |
|---|---|
| `system/` | Startup code, the linker script with the memory map, CMSIS. |
| `hal/` | The ST HAL library and its configuration. |
| `core/` | Memory protection unit and caches. |
| `clock/` | The PLLs: the system clock and the audio clock. |
| `rif/` | Access rights of the NPU and the display controller to memory. |

The memory map is the `MEMORY` table of
`system/linker/STM32N657XX_XIP.ld`.
