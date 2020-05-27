In this example is described how to connect a Windows x86 machine with an Arduino compatible device using the `ThroughSerial` strategy. Connect the USB port of the windows computer/laptop to the Arduino compatible device used as a receiver.

Program the Arduino with the `PJON\examples\ARDUINO\Local\ThroughSerial\BlinkWithResponse\Receiver.cpp` sketch and remember the device COM port assigned to that particular Arduino board (the COM selected to program the device in the Arduino IDE) because will be useful later.

Visual Studio 2017 Community is used to compile the program on the Windows x86 machine:

- Click on `file` top menu option, `open` and then `folder...`
- Select the `PJON\examples\WINX86\Local\ThroughSerial\BlinkWithResponse\Transmitter` directory
- Double click on `Transmitter.cpp` in the solution explorer (panel on right)
- Edit the COM port number (at line 23) inserting the one assigned to the receiver device and save the file
- Click on `Build` top menu option and then click `Build Solution`  
- If the building process ended positively, you should find the executable in `PJON\examples\WINX86\Local\ThroughSerial\BlinkWithResponse\Transmitter\Debug`, execute `Transmitter.exe` double clicking on it

You should see the Arduino blinking every second and at the same time see a `B` logged in the console (that is the response coming from the Arduino).
