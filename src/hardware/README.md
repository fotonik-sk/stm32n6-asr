# hardware

Everything that touches the silicon. The code is split into four parts by what
each one knows, and dependencies point only downward: `bsp` knows the board,
`drivers` know a peripheral of the microcontroller, `components` know an
external chip, `mcu` knows the microcontroller itself.

## Structure

| Object | Role |
|---|---|
| [`bsp/`](bsp/README.md) | The board: its initialization, the wiring of each device and the interrupt vectors. |
| `drivers/uart/` | Console UART. |
| `drivers/ltdc/` | Display controller. |
| [`drivers/pdm/`](drivers/pdm/README.md) | Microphone capture. |
| `drivers/npu/` | Power, clocks and memory of the NPU. |
| `components/` | ST drivers of the external NOR and PSRAM chips. |
| [`mcu/`](mcu/README.md) | The microcontroller: startup code, linker script, CMSIS, HAL, caches and clocks. |
