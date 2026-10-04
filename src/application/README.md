# application

The top layer of the firmware. It starts the board and every module in a fixed
order and connects the speech recognizer (`asr_engine`) to the board: the
microphone becomes its audio source and the display shows its text. It also
blinks the LED and shows the logo at start-up.

## Threads

The firmware runs on ThreadX. A lower priority number is more urgent. The
priorities and stack sizes of all threads, including the three threads of
`asr_engine`, are set in `config/app_threads.h`.

| Thread | Priority | What it does |
|---|---:|---|
| startup | 5 | Starts the modules one by one, then exits. |
| asr mel | 8 | Turns the microphone audio into log-mel features. |
| asr norm | 9 | Normalizes the features for the model. |
| led | 10 | Blinks the green LED to show that the firmware is alive. |
| asr net | 11 | Runs the model on the NPU and decodes its output into text. |
| display | 12 | Draws the recognized text on the display. |

The audio path comes first, because it must keep pace with the microphone. The
display is last: a late frame costs nothing.
