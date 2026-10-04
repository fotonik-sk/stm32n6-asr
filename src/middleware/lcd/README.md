# mw_lcd

Draws text and images into an RGB565 framebuffer and hands finished frames to
a display driver. It knows nothing about the display controller: the screen
size, the framebuffer and the function that shows a frame are passed at
initialization. On this board the application connects it to `drv_ltdc`.

`mw_lcd` does not own the framebuffers. There are two of them, 750 KB each:
one is on the screen while the other is being drawn. They are defined in
`src/hardware/bsp/bsp_lcd.c` and placed in the external PSRAM.

## Structure

| Object | Role |
|---|---|
| `mw_lcd.h` | The public interface: initialization, drawing of text and images, showing a frame. |
| `mw_lcd.c` | The text and image drawing. |
| `mw_lcd_font_sans_48.inc` | The font: glyph bitmaps of the printable ASCII characters, derived from Liberation Sans (SIL OFL 1.1). |
