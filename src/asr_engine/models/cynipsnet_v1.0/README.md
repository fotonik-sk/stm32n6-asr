# CynipsNet v1.0

CynipsNet v1.0 is a quantized speech recognition model developed for the
constraints of the NPU in the STM32N6 microcontroller. It is built so that
every part of the neural network is computed on the NPU, with no layer left to
the CPU: a layer that falls back to the CPU slows the whole network down
several times over.

The model was trained in three stages:

1. **Baseline** \*. Training on the LibriSpeech dataset [2] only, to allow a
   comparison with other models.
2. **Fine-tuned** \*\*. Further training with additional datasets: noise, voice-assistant
   commands and 248 hours of real recordings made through the board's
   microphone.
3. **Quantized** \*\*\*. The model is quantized to reduce its size and to make the
   mapping onto the NPU possible.

This is the first version of such a model, and it has shortcomings:

- It recognizes speech with a strong accent poorly.
- Recognition at a distance from the board's microphone is poor, even though
  the model was trained on recordings from that microphone.
- It is tuned to the specific microphone mounted on the discovery board.
  Another microphone will most likely reduce the accuracy.

## Accuracy

Word error rate (WER) in percent, lower is better. Every figure is for the
network alone, decoded greedily and without a language model.

| Model | Parameters | `dev_clean` | `dev_other` | `test-clean` | `test-other` | `dev_other_50cm` | `dev_other_250cm` |
|---|---:|---:|---:|---:|---:|---:|---:|
| QuartzNet 5x5 [1] | 6.7 M | 5.39 | 15.69 | — | — | — | — |
| QuartzNet 10x5 [1] | 12.8 M | 4.14 | 12.33 | — | — | — | — |
| CynipsNet v1.0 baseline \* | 7.2 M | 5.22 | 14.73 | 5.32 | 14.84 | 19.92 | 44.21 |
| CynipsNet v1.0 fine-tuned \*\* | 7.2 M | 5.65 | 15.01 | 5.77 | 15.65 | 16.70 | 25.54 |
| **CynipsNet v1.0 quantized \*\*\*** | 7.2 M | **5.79** | **15.31** | **5.89** | **16.06** | **17.04** | **26.02** |

\*, \*\*, \*\*\* are the three training stages described at the
[top of this page](#cynipsnet-v10).

The datasets:

- `dev_clean`, `dev_other`, `test-clean` and `test-other` are the standard
  LibriSpeech [2] sets dev-clean, dev-other, test-clean and test-other.
- `dev_other_50cm` and `dev_other_250cm` are dev-other recorded through this
  board's microphone from 50 and 250 cm. During the recording the loudness at
  the source was 55 to 60 dB.

How the figures were obtained:

- **CynipsNet v1.0** was evaluated on a PC in the training framework. The
  accuracy was then rechecked on the microcontroller's NPU: the difference is
  within the noise. A validation dataset was also run through the
  microcontroller, and there too the differences are minimal.
- **QuartzNet** figures are taken from the publication [1] (arXiv:1910.10261).

The datasets used for training are listed in [Datasets](#datasets).

## Properties

| | |
|---|---|
| Language | English |
| Input | int8, 600 frames × 64 log-mel bands: 6 s at 100 frames per second. |
| Output | int8, 300 steps × 29 classes, one step per 20 ms. |
| Classes | CTC blank, space, `A`–`Z`, apostrophe |
| Decoding | greedy CTC, no dictionary, no language model |
| Parameters | 7,199,645, stored as 8-bit integers |
| Weights | 7,315,121 bytes (7.3 MB) in external NOR at `0x71500000` |
| Activations | 4,154,752 bytes (4.2 MB) in internal SRAM and PSRAM |
| Operations per run | 2.58 × 10⁹ multiply-accumulates |
| Computed on | the NPU: 268 of 268 epochs are pure hardware |
| Run time | about 220 ms per window; not optimized yet |
| Latency, first characters | 0.2 to 0.7 s (tentative text) |
| Latency, final characters | 1.5 to 2.0 s (confirmed text) |
| Converter | ST Edge AI Core 4.0, LL_ATON 1.1.3-262 |

## Memory

After conversion the model occupies the following memory regions:

| Memory | Pools in the log | Range | Occupied |
|---|---|---|---|
| Internal SRAM | `cpuRAM1`, `cpuRAM2`, `npuRAM3`–`npuRAM6` | `0x34064000`–`0x343C0000` | 3007 KiB of 3440 KiB, activations |
| PSRAM | `hyperRAM` | `0x90500000`–`0x92000000` | 1050 KiB of 27 MiB, activations |
| NOR flash | `octoFlash` | `0x71500000`–`0x78000000` | 6.98 MiB of 107 MiB, weights |

For more detail, see the converter log: [converter_log.txt](converter_log.txt).

## Datasets

The datasets the model was trained on.

| Dataset | Used as | Source | License |
|---|---|---|---|
| LibriSpeech | speech | <https://www.openslr.org/12/> | CC BY 4.0 |
| VoiceAssistant-400K | speech, voice-assistant commands | <https://huggingface.co/datasets/gpt-omni/VoiceAssistant-400K> | Apache-2.0 |
| DNS Challenge (noise from Freesound) | noise | <https://github.com/microsoft/DNS-Challenge> | CC BY 4.0 |
| FSD50K (selection) | noise | <https://zenodo.org/record/4060432> | CC0 / CC BY per clip |
| Recordings through the board's microphone, 248 hours | speech as the board hears it | made for this project | — |

Full citations and attribution are in [../LICENSE.md](../LICENSE.md).

## License

The weights and the conversion report are CC BY-NC-SA 4.0 (non-commercial),
the archive and its two headers remain under ST's SLA0104, the rest is
Apache-2.0. Details: [../LICENSE.md](../LICENSE.md).

## References

[1] S. Kriman, S. Beliaev, B. Ginsburg, J. Huang, O. Kuchaiev, V. Lavrukhin,
R. Leary, J. Li and Y. Zhang, "QuartzNet: Deep Automatic Speech Recognition
with 1D Time-Channel Separable Convolutions", 2019,
<https://arxiv.org/abs/1910.10261>.

[2] V. Panayotov, G. Chen, D. Povey and S. Khudanpur, "LibriSpeech: an ASR
corpus based on public domain audio books", ICASSP 2015,
<https://www.openslr.org/12/>.
