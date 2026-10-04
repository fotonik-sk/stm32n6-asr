# drv_pdm

Continuous capture of 16 kHz sound from a PDM microphone. The digital filter
of the microcontroller turns the microphone's bit stream into samples, the DMA
writes them into a ring of equal chunks, and a reader thread copies complete
chunks out. The driver knows the microcontroller, not the board: the pins and
the interrupt priority come from `bsp`.

## Structure

| Object | Role |
|---|---|
| `drv_pdm.c`, `drv_pdm.h` | The driver's public interface and its start-up sequence. |
| `mdf/` | The digital filter: its clock and filter settings. |
| `dma/` | The DMA transfer and the memory the samples are written to. |
| `ring/` | Bookkeeping of the chunks and copying them out to the reader. |
