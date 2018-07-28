
For now PJON works using `wiringPi` that should be already installed in the default raspbian distribution.

If in your case it is missing, type `sudo apt-get install wiringPi`

To correctly receive data on Raspberry Pi it may be necessary to disable the serial console, accessing as root to `boot/cmdline.txt` and removing `console=ttyAMA0, 115200`  or `console=serial0, 115200` if present.

Connect the Serial GPIO TX and RX of your Raspberry Pi with an Arduino compatible device (crossing the channels) through a level shifter not to damage the 3v3 rpi serial port.

![image](http://www.pjon.org/assets/images/PJON-RPI-UNO-level-shifter.jpg)

To compile the program it is necessary to reach with the terminal the `PJON/examples/RPI/Local/ThroughSerial/BlinkWithResponse/` directory and type `make`. Once the program is compiled you should see a new file called `Transmitter`.

Typing `sudo ./Transmitter` the program is executed and some logging info should appear.

Now program the receiving Arduino compatible device, using `PJON/examples/ARDUINO/Local/ThroughSerial/BlinkWithResponse/Receiver/Receiver.ino` [receiver's sketch](https://github.com/gioblu/PJON/blob/master/examples/ARDUINO/Local/ThroughSerial/BlinkWithResponse/Receiver/Receiver.ino) and the Arduino IDE.

As soon as you type `sudo ./Transmitter` and hit enter the Arduino compatible device should start blinking, on the other side in the Raspberry pi's terminal should appear "BLINK" each time Arduino blinks.
