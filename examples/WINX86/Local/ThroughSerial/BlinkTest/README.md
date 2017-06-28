In this example is described how to connect a Windows x86 machine with an Arduino compatible device using the `ThroughSerial` strategy, the USB port of both devices and a USB cable; the Arduino compatible device is used as a receiver and will blink repeatedly every second if all works fine.

Program the Arduino with the `PJON\examples\ARDUINO\Local\ThroughSerial\BlinkTest\Receiver.cpp` sketch and remember the device COM port assigned to that particular Arduino board (the COM selected to program the device in the Arduino IDE) because will be useful later.

Visual Studio 2017 Community is used to compile the program on the Windows x86 machine:

- Create a new project and select the `Win32 Console Application` application type and continue
- In the application wizard, click next, be sure to deactivate the `Precomplied header` option and press finish
- When the project is created, locate its directory, by default should be in `Documents\Visual Studio 2017\Projects\Transmitter\Transmitter`, and clone/copy there the PJON directory
- Locate the `Solution explorer` right sidebar, right click on `Source files`, then `add` -> `existing item`, search for and select  `Serial.cpp` in `PJON\interfaces\WINX86\Serial\`  
- Copy the content of `PJON\examples\WINX86\Local\ThroughSerial\BlinkTest\Transmitter.cpp` example
 in `Documents\Visual Studio 2017\Projects\Transmitter\Transmitter\Transmitter.cpp`, edit the COM port number (at line 38) according to the one assigned to the receiver device and save the file
- Press `Build` in the top bar, then select `Batch build` and click the checkboxes of the two release configurations to generate an executable file
- If the building process ended positively, navigate using a console to `Documents\Visual Studio 2017\Projects\Transmitter\Release` and execute `Transmitter.exe`

You should see the Arduino blinking every second and a related log coming out the console.
