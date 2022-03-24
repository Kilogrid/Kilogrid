### Before you start programming

Required softwares and libraries:

- AVR-GCC : to compile Module and Kilobot controller programs, and core libraries
- Qt5 : to build GUI applications, KiloGUI and KilogridEditor
- libusb
- libftdi
- [TBC]

You have to copy or rename `Common/kilogui-dispatcher/serial_speed.h.template` to `Common/kilogui-dispatcher/serial_speed.h`.
Be careful that it is not `serial_packet.h`, it is `serial_speed.h`.
Edit a value `BAUD`, defined in the header file `serial_speed.h`, depending on your environment.

In IRIDIA, you can use:

- `#define BAUD 38400` for Small Arena
- `#define BAUD 256000` for Large Arena [default]
