# Flash a ready-made firmware

This guide is for programming an STM32N6570-DK with firmware files you
already have. The only tool required is STM32CubeProgrammer. Building the
firmware from source is covered in [setup.md](setup.md).

## What you need

- The STM32N6570-DK board and a USB-C cable.
- [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html)
  2.22.0 or newer.
- The firmware file `firmware.hex` from the [latest release][release]. It
  holds the boot loader, the application and the network weights together
  with their addresses in the board's flash memory.

## Install STM32CubeProgrammer

**Windows:** run the installer and let it install the **ST-LINK drivers**.

**Linux:** run the installer, then install the ST-LINK udev rules that come
with it and replug the board (adjust the path if you installed elsewhere):

```bash
sudo cp "$HOME/STMicroelectronics/STM32Cube/STM32CubeProgrammer/Drivers/rules/"*.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
```

## Prepare the board

- Connect the board's **ST-LINK_V3EC USB Type-C** port to the PC; it also
  powers the board. The power-source jumper **JP2** must be in position **1-2:
  5V_STLK**.
- The two boot switches: **BOOT0 (SW2)** stays at **L** (left) all the time;
  **BOOT1 (SW1)** selects the mode:

| BOOT1 (SW1) | Mode | Used for |
|---|---|---|
| **H** (right) | development: the bootROM waits for the debugger | flashing |
| **L** (left) | boot from external flash | normal operation |

After moving a switch press **RESET**; if the board does not start in the new
mode, unplug and replug the USB cable.

## Flash

1. Set BOOT1 = **H**, then reset or power-cycle the board.
2. Open a terminal in the folder with `firmware.hex` and run the command for
   your system. Adjust the first line if STM32CubeProgrammer is installed
   elsewhere.

   Linux:

   ```bash
   CUBE_PROGRAMMER_BIN="$HOME/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin"

   "$CUBE_PROGRAMMER_BIN/STM32_Programmer_CLI" \
     -c port=SWD freq=24000 ap=1 \
     -el "$CUBE_PROGRAMMER_BIN/ExternalLoader/MX66UW1G45G_STM32N6570-DK.stldr" \
     -d firmware.hex -v
   ```

   Windows (Command Prompt):

   ```bat
   set "CUBE_PROGRAMMER_BIN=C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"

   "%CUBE_PROGRAMMER_BIN%\STM32_Programmer_CLI.exe" ^
     -c port=SWD freq=24000 ap=1 ^
     -el "%CUBE_PROGRAMMER_BIN%\ExternalLoader\MX66UW1G45G_STM32N6570-DK.stldr" ^
     -d firmware.hex -v
   ```

   Programming must end with a "Download verified successfully" message.

3. Set BOOT1 = **L**, then reset or power-cycle the board.

## Run

The red LED lights briefly, the LCD shows the logo and the green LED blinks.
Speak English near the board: the text appears on the LCD.

The firmware also writes its log and the recognized text to a serial port,
which the ST-LINK USB connection exposes as a virtual COM port. Reading it is
optional; any serial terminal works with these settings:

| Parameter | Value |
|---|---|
| Baud rate | 921600 |
| Data bits | 8 |
| Parity | none |
| Stop bits | 1 |
| Flow control | none |

## Problems and fixes

| Symptom | Cause and fix |
|---|---|
| CubeProgrammer cannot connect to the target | Board not in development mode: BOOT1 = H, then RESET or replug. For an unstable link, change `freq=24000` to `freq=8000` in the flash command. |
| CubeProgrammer asks for an ST-LINK firmware upgrade | Upgrade the ST-LINK firmware from the STM32CubeProgrammer GUI (**Firmware upgrade** in the ST-LINK configuration panel), then retry. |
| Nothing happens after flashing | BOOT1 is still at H: set it to L, then reset or power-cycle the board. |

[release]: https://github.com/fotonik-sk/stm32n6-asr/releases/latest
