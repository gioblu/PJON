This example consists of a Transmitter example that is supposed to work as a "RemoteWorker" in the ARDUINO/Local/EthernetTCP/SoftwareBitBangSurrogate setup, instead of the Arduino RemoteWorker present there.

This means that this Linux process can directly parttake in the SWBB bus via the surrogate. The Ethernet connection between RemoteWorker and Surrogate allows the two to be located far from each other. Also, the Surrogate "phoning home" to the RemoteWorker could open for scenarios like those used in most IoT setups today, but the place they are phoning home to can be a server controlled by you, not some third party server in the public cloud.

There is only a Makefile for this example, no code, as the Windows version can be built for Linux. Copy Transmitter.cpp from examples/WINX86/Local/EthernetTCP/RemoteWorker/Transmitter.
