
For now PJON works using `wiringPi` that should be already installed in the default raspbian distribution.

If in your case it is missing, type `sudo apt-get install wiringPi`

To correctly receive data on Raspberry Pi it may be necessary to disable the serial console, accessing as root to `boot/cmdline.txt` and removing `console=ttyAMA0, 115200`  or `console=serial0, 115200` if present.

To compile the program it is necessary to reach with the terminal the `PJON/examples/RPI/Local/ThroughSerial/RS485-BlinkWithResponse/` directory and type `make`. Once the program is compiled you should see a new file called `Transmitter`.

Typing `sudo ./Transmitter` the program is executed and some logging info should appear.

Now program the receiving Arduino compatible device, using `PJON/examples/ARDUINO/Local/ThroughSerial/RS485-BlinkWithResponse/Receiver/Receiver.ino` [receiver's sketch](https://github.com/gioblu/PJON/blob/master/examples/ARDUINO/Local/ThroughSerial/RS485-BlinkWithResponse/Receiver/Receiver.ino) and the Arduino IDE.

RPI circuit:
- Connect RPI Serial TX pin or GPIO14 to DI RS485 pin
- Connect RPI Serial RX pin or GPIO 15 to RO RS485 pin
- Connect together DE and RE RS485 pins
- Connect DE or RE to RPI pin 1 or GPIO 18
- Connect both RS485 modules together connecting A to A and B to B
- Connect the RPI 3v3 pin to RS485 VCC pin
- Connect the RPI GND pin to RS485 GND pin

Arduino circuit:
- Connect Arduino serial RX pin to RO RS485 pin
- Connect Arduino serial TX pin to DI RX485 pin
- Connect together DE and RE RS485 pins
- Connect Arduino pin 2 to DE or RE
- Connect the Arduino 5v pin to RS485 VCC pin
- Connect the Arduino GND pin to RS485 GND pin

As soon as you type `sudo ./Transmitter` and hit enter the Arduino compatible device should start blinking, on the other side in the Raspberry pi's terminal should appear "BLINK" each time Arduino blinks.
