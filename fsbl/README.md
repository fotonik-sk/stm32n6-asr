# fsbl — First Stage Boot Loader

The small program that runs before the application. The STM32N6 has no
internal flash: after reset the bootROM copies the FSBL from the start of the
external NOR into AXISRAM2 and runs it there. The FSBL prepares the chip, maps
the NOR into the address space and jumps to the application, which then
executes in place from the NOR.

It is a standalone CMake project with its own toolchain file, preset and build
directory, built separately from the application. It is based on ST's
`Template_FSBL_XIP`.

## Boot sequence

1. Apply the NRST workaround from the STM32N6 errata sheet: one corrective
   software reset per external boot.
2. Enable the instruction and data caches.
3. Switch on the red LED to show that the FSBL is alive.
4. Raise the CPU clock to 600 MHz from PLL1.
5. Initialize XSPI2 and map the MX66UW1G45G NOR through the External Memory
   Manager.
6. Jump to the application in the NOR.

If any step fails, the FSBL stops with the red LED left on.

## Structure

| Object | Role |
|---|---|
| `application/` | `main.c`: the boot sequence above. |
| `hardware/mcu_hal/` | Vendored STM32N6 HAL, its configuration and the XSPI2 pin and clock setup. |
| `hardware/system/` | Startup code, CMSIS and the linker script that places the FSBL in AXISRAM2. |
| `middleware/extmem/` | Vendored ST External Memory Manager and its configuration for the NOR. |
| `cmake/` | Toolchain file and the device definitions. |

## Notes

- The FSBL never writes OTP fuses. It is built with `NO_OTP_FUSE`, and the
  code of the ST template that programs the fuse is removed.
- The images are not encrypted and secure boot is not used.
- The clock setup is shared with the application: the FSBL compiles the same
  `mcu_clock.c` from `src/hardware/mcu/clock/`.
- No interrupts are used. HAL timeouts run on a polled SysTick counter.
- In the NOR the FSBL is at `0x70000000` and the application at `0x70100000`.
- Building, signing and flashing are described in
  [docs/setup.md](../docs/setup.md).
