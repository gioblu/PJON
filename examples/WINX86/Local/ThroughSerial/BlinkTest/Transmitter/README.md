In this example is described how to connect a Windows x86 machine with an Arduino compatible device using the `ThroughSerial` strategy, the USB port of both devices and a USB cable; the Arduino compatible device is used as a receiver and will blink repeatedly every second if all works fine.

Program the Arduino with the `PJON\examples\ARDUINO\Local\ThroughSerial\BlinkTest\Receiver.cpp` sketch and remember the device COM port assigned to that particular Arduino board (the COM selected to program the device in the Arduino IDE) because will be useful later.

Visual Studio 2017 Community is used to compile the program on the Windows x86 machine:

- Click on `file` top menu option, `open` and then `folder...`
- Select the `PJON\examples\WINX86\Local\ThroughSerial\BlinkTest\Transmitter` directory
- Double click on `Transmitter.cpp` in the solution explorer (panel on right)
- Edit the COM port number (at line 40) inserting the one assigned to the receiver device and save the file
- Click on `Build` top menu option and then click `Build Solution`  
- If the building process ended positively, you should find the executable in `PJON\examples\WINX86\Local\ThroughSerial\BlinkTest\Transmitter\Debug`, execute `Transmitter.exe` double clicking on it

You should see the Arduino blinking every second and a related log coming out the console.
