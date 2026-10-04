# Changelog

## 0.1.0 - 2026-10-04

Initial release. Speech recognition that runs entirely on the STM32N6
microcontroller: on the STM32N6570-DK board the firmware listens to the
on-board microphone and prints English text on the display as you speak, with
no network connection and no server. The CPU turns the microphone signal into
log-mel features and the NPU runs CynipsNet v1.0, a model designed for the
STM32N6 that recognizes any words rather than a fixed list of commands. The
repository holds the model weights and the complete firmware that runs them.
