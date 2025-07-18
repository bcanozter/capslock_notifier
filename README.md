# Caps Lock Notifier Kernel Module

This Linux kernel module is written for learning purposes. It registers an input handler to capture keyboard events and monitors the Caps Lock LED state. The current state is exposed via the proc file system for user-space access.

## Features

- Registers an input handler to listen for keyboard events
- Detects and reports the Caps Lock LED state (ON/OFF)
- Exposes the state via `/proc/capslock_state`

## Environment Details

- Linux kernel headers: `linux-headers-6.8.0-64-generic`
- GCC: `12.3.0-1ubuntu1~22.04`

## Build

```
make
```

## Load the Module

```
sudo insmod capslock_notifier.ko
```

## Usage

Read the current Caps Lock LED state:

```
cat /proc/capslock_state
```

- Output is `1` if Caps Lock is ON, `0` if OFF.

## Example Output

```bash
[12747.354555] Init capslock_notifier module
[12747.354563] Connected input device: AT Translated Set 2 keyboard
[12747.354564] (AT Translated Set 2 keyboard): Initial Caps Lock state: OFF 
[12747.354569] Connected input device: Corsair Corsair Gaming K95 RGB PLATINUM Keyboard
[12747.354571] (Corsair Corsair Gaming K95 RGB PLATINUM Keyboard): Initial Caps Lock state: OFF
[12748.564202] (Corsair Corsair Gaming K95 RGB PLATINUM Keyboard): Caps Lock LED state: ON
[12749.356137] (Corsair Corsair Gaming K95 RGB PLATINUM Keyboard): Caps Lock LED state: OFF
```

## Unload the Module

```
sudo rmmod capslock_notifier
```

---

**Author:** Burak Ozter
