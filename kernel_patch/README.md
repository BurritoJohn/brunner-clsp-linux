# brunner-ff

Brunner Force Feedback Joystick patch for the linux kernel

## Requirements

- Make
- GCC
- `linux-kbuild` and `linux-headers`

## Build and install

### DKMS

This installs the module to the system `src` directory and allows the module to
be rebuilt any time the kernel updates.

You need to have `dkms` installed.
On Ubuntu, you can use the following:

```bash
sudo apt update && sudo apt install dkms
```

Run the command in the root of this repo:

```bash
sudo dkms install .
```

If the module is already existing in the dkms tree, you can run:

```bash
sudo dkms status
```

to see the exact module name and version, then:

```bash
sudo dkms remove <modulename>/<version> --all
```

Note:
If the device is still plugged in, run:

```bash
sudo modprobe -r hid_brunnerff
```

### Test if the driver is loaded

Plug the joystick, then run:

```bash
sudo dmesg | grep "brunner-ff"
```

## Modinfo

Use modinfo to see available options :

```bash
modinfo hid_brunnerff
```
