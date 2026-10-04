# bsp

Support of the STM32N6570-DK board. This is the only code that knows the
board: which pins, peripherals, memories and interrupt priorities it uses. It
holds the board description, brings the board up before the RTOS starts, and
passes the board's wiring to each driver. 

