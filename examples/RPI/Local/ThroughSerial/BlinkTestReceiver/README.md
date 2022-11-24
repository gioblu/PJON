Thanks to the addition of the [interfaces](https://github.com/gioblu/PJON/tree/master/src/interfaces), it is finally possible to use PJON on a Raspberry Pi (or more generally on a Linux machine). A library providing the low level methods is necessary, for now PJON works using `wiringPi` that should be already installed in the default raspbian distribution.

If in your case it is missing, type `sudo apt-get install wiringPi`

Connect the Serial GPIO TX and RX of your Raspberry Pi with an Arduino compatible device (crossing the channels) through a level shifter not to damage the 3v3 rpi serial port.

To correctly receive data on Raspberry Pi it may be necessary to disable the serial console, accessing as root to `boot/cmdline.txt` and removing `console=ttyAMA0, 115200`  or `console=serial0, 115200` if present.

To compile the program it is necessary to reach in the terminal the `PJON/examples/RPI/Local/ThroughSerial/BlinkTestReceiver/` directory and type `make`. Once the program is compiled you should see a new file called `Receiver`. Typing `sudo ./Receiver` the program is executed and some logging info should appear.

Now program the transmitter Arduino compatible device, using `PJON/examples/ARDUINO/Local/ThroughSerial/BlinkTest/Transmitter/Transmitter.ino` [transmitter's sketch](https://github.com/gioblu/PJON/blob/master/examples/ARDUINO/Local/ThroughSerial/BlinkTest/Transmitter/Transmitter.ino) and the Arduino IDE.

You should see "BLINK" printed in the console every second; press `control` + `c` to interrupt the process.
