# norm — receptive-field normalization

The second stage of `asr_engine`. It takes raw log-mel frames from the mel ring
and produces the int8 input of the network. Every 50 frames (0.5 s) it
normalizes the whole 600-frame window per band, with the mean and deviation of
up to the last 3000 frames (30 s), and quantizes it with the network's input
scale and zero point. The window is written straight into the NPU input tensor.

Per band: `mu = S1/N`, `sigma = max(sqrt(max(S2/N − mu², 0)), 1e-6)` in double;
code `q = clamp(round_half_even(((x − mu) / sigma) / scale + zero_point), −128, 127)`.

## Structure

| Object | Role |
|---|---|
| `norm_thread.c` | The stage's ThreadX thread, woken once per stored frame. Feeds the mel ring into `norm_push_frame()`; before the frame that closes a block it waits for the network to release the input tensor (`acquire_input`), after it hands the window over (`window_ready`). Lost frames are logged but do not reset anything. |
| `norm.c` | The streaming normalizer, plain C: field ring, per-block sums, statistics, normalization and quantization. It owns only the mean and deviation; the three big buffers are passed to `norm_init()`, which rejects overlapping ones. |
| `norm_cfg.h` | Derived geometry: 12 blocks per window, 60 statistics blocks, values per block and per field. |

| Buffer | Owner | Bytes |
|---|---|---|
| raw field, 600 × 64 float | `asr_engine.c`, placed by the host (PSRAM) | 153 600 |
| statistics, 60 blocks | `asr_engine.c`, placed by the host (PSRAM) | 61 440 |
| mean and deviation | `norm.c` | 1 024 |
| input window, 600 × 64 int8 | the network (NPU input tensor) | 38 400 |

At start the field holds digital silence, `log(2^-24)`, which never enters the
statistics. The first window is therefore ready after the first block (0.5 s),
and the statistics grow with the stream until they reach 30 s, then slide.

## Interface

Used by `asr_engine.c`:

- `uint32_t norm_thread_create(void)` — create the thread and its semaphore,
  suspended, before the scheduler.
- `uint32_t norm_thread_start(const norm_thread_output_t *output)` — bind the
  buffers and hooks, start a new stream, resume the thread. `output` holds
  `buffers` (`input`, `field`, `stats`), `acquire_input()` and
  `window_ready()`; all are mandatory.
- `void norm_thread_frames_ready(void)` — the mel stage's frames hook.

The plain-C core, free of RTOS and hardware:

- `norm_status_t norm_init(const norm_buffers_t *buffers)`
- `norm_status_t norm_push_frame(const float *bands)` — `NORM_OK`,
  `NORM_WINDOW_READY` (the input holds a new window) or `NORM_ERROR`.
- `int norm_next_frame_writes_window(void)` — non-zero if the next frame will
  overwrite the input.
- `void norm_reset(void)` — deliberate restart: silence, empty statistics.

## Notes

- Every window is renormalized as a whole, so a frame appears in 12 windows
  with different codes each time and nothing is cached between windows. The
  cost is 600 × 64 double divisions every 0.5 s.
- **Do not "optimize" the arithmetic:** exact agreement with the model's
  reference needs double sums and statistics, a real division by `sigma` (not
  a multiplication by its reciprocal), round half to even (`lrintf`) and
  `-ffp-contract=off` (set in `CMakeLists.txt`) so GCC does not fuse
  `S2/N − mu²` into one FMA.
- A gap in the feature stream does not reset the stage; a reset would throw
  away 30 s of statistics.
- `ASR_MODEL_NORM_STATS_FRAMES` is the only value here that may change without
  reconverting the model. It must be a whole number of blocks and at least the
  window.
