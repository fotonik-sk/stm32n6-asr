# logger

Thread-safe logger for a terminal. It formats log records with a time stamp
and a colored level, or plain text, and hands each complete record to an
output; on this board the output is the console UART. A mutex keeps records
from different threads from mixing. The logger does not know the UART: the
output functions are attached at initialization.

## Structure

| Object | Role |
|---|---|
| `logger.h` | The public interface: initialization, the log macros and plain text output. |
| `logger.c` | Formatting of the records, the mutex and the hand-over to the output. |
