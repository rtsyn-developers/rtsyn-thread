# RTSyn Thread

## Usage

### Update

Make sure you have last version of the dependencies:

```bash
xrepo update-repo
xmake require --upgrade
```

For development you may need to run:

```bash
xmake require --upgrade -fy <dependency_name>
```

### Compiling

For compiling:

```bash
xmake
```

### Linux CPU-idle latency QoS (PREEMPT_RT backend)

When built with `--thread_core=preempt_rt`, RTSyn requests a 0 µs maximum
CPU-idle exit latency through Linux PM QoS by default. The default is defined
in `rtsyn-defaults` as `RTSYN_ENGINE_DEFAULT_CPU_DMA_LATENCY_US`. Override it
by setting `RTSYN_CPU_DMA_LATENCY_US` in the daemon environment before launch:

```bash
RTSYN_CPU_DMA_LATENCY_US=100 ./build/linux/x86_64/release/rtsyn --no-gui daemon run
```

The value is in microseconds; `0` requests the lowest available idle-exit
latency, and a positive number sets that maximum. Set the variable to `off` to
disable the request. RTSyn keeps `/dev/cpu_dma_latency` open for the process
lifetime, so the kernel releases the request automatically on exit. If the
device cannot be opened or written, PREEMPT_RT thread initialization fails
with a diagnostic. Ensure the process can write to the device, or explicitly
opt out with `RTSYN_CPU_DMA_LATENCY_US=off`.

This setting is independent of scheduler policy/priority and only applies to
the Linux PREEMPT_RT backend.

### Tests

For running test:

```bash
xmake test
```

For enabling valgrind, before running tests:

```bash
xmake f --valgrind=y
```

For disabling valgrind, just replace `y` for `n`.

### Local development

If you want to test your changes locally from different parts of RTSyn, export the path where you have all the repos:

```bash
export RTSYN_WORKSPACE=<PATH>
```

> [!WARNING]
> This expects you also the `rtsyn-xmake-repo`.

### Cleaning

To remove all generated build artifacts:

```bash
xmake clean --all
```

To also reset cached configuration and tool detection:

```bash
xmake f -c
```
