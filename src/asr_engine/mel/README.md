# mel — log-mel front end

The first stage of `asr_engine`. It turns 16 kHz PCM16 into one vector of 64
log-mel values every 10 ms, in float32, and stores the vectors in a ring for the
normalization stage. The chain is: scale by 1/32768 → pre-emphasis (0.97) →
320-sample frames every 160 samples → symmetric Hann window → zero-pad to 512
→ power spectrum → Slaney mel bank (0–8 kHz) → `log(x + 2^-24)`. There is no
normalization here; that is the next stage.

## Structure

| Object | Role |
|---|---|
| `mel_thread.c` | The stage's ThreadX thread. Woken once per capture chunk, it drains the audio port (at most count−1 chunks per read), converts, stores every frame and calls the frames hook. Lost audio is logged and resets the converter. Also implements `asr_engine_audio_chunk_ready()`, since it owns the semaphore. |
| `mel.c` | Streaming converter. Carries the frame overlap and the previous sample for pre-emphasis across calls, so any split of the stream into blocks yields exactly the frames of one offline pass. |
| `mel_fft.c` | Radix-2 float32 FFT returning the power spectrum of one frame (257 bins). |
| `mel_tables.c` | Hann window and Slaney filter bank with area normalization, built at init in double precision from `ASR_MODEL_MEL_*` and stored as float spans (498 non-zero weights). |
| `mel_ring.c` | Single-producer, single-consumer ring of 128 frames (1.28 s, 32 KB). When full it drops the oldest frame and counts it. A read copies first and then checks that the producer did not lap it. |
| `mel_cfg.h` | Values derived from the model parameters (frame overlap, spectrum bins) and the weight-pool size. |

## Interface

Used by `asr_engine.c`:

- `uint32_t mel_thread_create(void)` — create the thread and its semaphore,
  suspended, before the scheduler.
- `uint32_t mel_thread_start(const asr_audio_port_t *port, mel_thread_frames_hook_t on_frames)`
  — build the tables, empty the ring, resume the thread. The port must be
  complete; `on_frames` may be `NULL`.
- `typedef void (*mel_thread_frames_hook_t)(void)` — called on the mel thread
  after each stored frame; must not block.

Used by the normalization stage (the consumer side of the ring):

- `uint32_t mel_ring_available(void)` — frames waiting.
- `mel_ring_status_t mel_ring_read(float *destination, uint32_t destination_values, uint32_t frames)`
  — copy and release the oldest frames; `OK`, `ERROR`, `UNDERRUN`, or
  `OVERRUN` (data discarded, reading resumes at the newest frames).
- `mel_ring_status_t mel_ring_skip(uint32_t frames)` — release without copying.

The converter itself (`mel_init()`, `mel_process()`, `mel_reset()`,
`mel_flush()`) is plain C, free of RTOS and hardware.

## Notes

- `mel.c` and `mel_fft.c` use private scratch buffers: only one thread may own
  the conversion path.
- After lost audio the filter state would span the gap, so the thread calls
  `mel_reset()` whenever the port reports dropped chunks or an overrun.
- A full ring drops the oldest frames instead of stalling capture; the count is
  logged. 1.28 s is how late normalization may run.
- Cost: one 512-point FFT and 498 multiply-accumulates per frame at 100 frames
  per second; about 40 KB of static memory, mostly the ring.
- Changing a value in `asr_model_params.h` needs no regenerated tables: they
  are built at start-up. `mel_tables_init()` fails if the bank no longer fits
  its 640-weight pool.
