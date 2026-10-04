# middleware

Services for the layers above that do not depend on the board: the RTOS, the
logger, the display renderer and the NPU runtime from ST. Middleware reaches
the hardware only through functions it is given or through the HAL, never
through the board code.

## Structure

| Object | Role |
|---|---|
| `threadx/` | The ThreadX RTOS and its configuration for this project. |
| [`logger/`](logger/README.md) | Thread-safe logger for a terminal. |
| [`lcd/`](lcd/README.md) | Drawing of text and images into a framebuffer. |
| `stedgeai/` | The ST Edge AI runtime that runs the model on the NPU. |
