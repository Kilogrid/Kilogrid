# Kilogrid

A modular virtualization environment designed for the Kilobot robots.

Link: [Kilogrid - IRIDIA](http://iridia.ulb.ac.be/kilogrid)

### Before you start programming

Required softwares and libraries:

- AVR-GCC : to compile Module and Kilobot controller programs, and core libraries
- Qt5 : to build GUI applications, KiloGUI and KilogridEditor
- libusb
- libftdi

You have to edit the value `BAUD` in the file `communication/kilogui-dispatcher/serial_speed.h`, depending on your environment.
In IRIDIA, you can use:

- `#define BAUD 38400` for Small Arena
- `#define BAUD 256000` for Large Arena [default]

