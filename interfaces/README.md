
#### What is an interface?
PJON uses interfaces to agnostically reach low level methods, abstracting all architecture/hardware related stuff and enhancing portability. An interface is simply a file containing a set of macros pointing to the necessary methods. This part of the PJON framework enables users to easily get PJON running on virtually every device able to execute a compiled C++ program.

#### Input-Output interfaces
- `PJON_IO_ANALOG_READ` like `analogRead`, it receives 1 parameter, the pin to read
- `PJON_IO_WRITE` like `digitalWrite` receives 2 parameters, the pin to set and the value (0 or 1)
- `PJON_IO_READ` like `digitalRead` receives 1 parameter, the pin to read
- `PJON_IO_MODE` like `pinMode` receives 2 parameters, the pin, and its mode (`INPUT`, `OUTPUT` or `INPUT_PULLUP`)
- `PJON_IO_PULL_DOWN` it receives 1 parameter, the pin to set input low state

#### Random number generator interfaces
- `PJON_RANDOM` receives 1 parameter, the maximum value, returns a random number
- `PJON_RANDOM_SEED` receives 1 parameter, sets the random seed

#### Serial interfaces
- `PJON_SERIAL_AVAILABLE` returns `true` if data was received, `false` if not
- `PJON_SERIAL_WRITE` sends a byte over Serial, receives 1 parameter, the byte to send
- `PJON_SERIAL_READ` returns `true` if data was received, `false` if not
- `PJON_SERIAL_FLUSH` waits for dispatched data to be transmitted

#### Timing interfaces
- `PJON_DELAY_MICROSECONDS` receives 1 parameter, the number of microseconds to delay
- `PJON_MICROS` returns the number of microseconds since startup
