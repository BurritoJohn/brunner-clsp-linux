# CLS-P Joystick

## About

This project is an attempt to bring native linux compatibility for the BRUNNER CLS-P Joystick, using either libusb or a DKMS kernel patch.

## Dynamic Kernel Module

A modified kernel module is available in `/kernel_patch` to use with built-in `FF_EVENTS`.
The kernel patch does not need the `udev` rule to be set in order to function.

## libusb

A udev rule needs to be set to access the device in RW mode using libusb.
To do so, simply copy the resource `73-usb-brunner.rules` in `/etc/udev/rules.d`.

### Building

-`cmake -B build`

-`cmake --build build`

## Running

The Brunner CLS-P Joystick must be in DirectX Mode (check with `lsusb`).
If it is not the case, please upgrade the device firmware and switch it to DirectX mode using the [Brunner USB config tool](https://forum.brunner-innovation.swiss/).
Both can be achieved on a windows machine without admin rights.

## Firmware upgrade

The firmware of the joystick is often improved, and can easily be upgraded if the changelog concerns the CLS-P joystick, using the following link:
[Firmware upgrade](https://forum.brunner-innovation.swiss/forums/topic/firmware-yoke-joystick-rudder/)
