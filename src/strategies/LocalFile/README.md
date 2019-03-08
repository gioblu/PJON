## LocalFile

**Medium:** Hard disk

`LocalFile` uses a local file present on the hard drive, it is generally useful for developing, simulating and testing your network on a real time operative system without having to deploy physical hardware.

It will let multiple processes on the same computer communicate together on a PJON bus via this file.

### How to use LocalFile
Pass the `LocalFile` type as PJON template parameter to instantiate a PJON object ready to communicate through this strategy.
```cpp  
  // Use LocalFile strategy with PJON device id 44
  PJON<LocalFile> bus(44);
```
After testing or simulation you may want to use conditional compiling and exchange this strategy with the actual one on your target hardware. 

The directory examples/LINUX/Local/LocalFile/PingPong contains examples. To build these on Linux, simply type "make". To build on Windows, open the solution file in Visual Studio 2017.

The strategy uses a file where messages are persisted. A queue of the last messages is kept there, with the queue size set by the preprocessor define PJON_LF_QUEUESIZE which is 20 by default.

Reading messages is based on polling. The poll interval in milliseconds is defined by the preprocessor definition PJON_LF_POLLDELAY which is 10 by default. Decreasing this value will increase the communication speed but also use more CPU and cause more disk activity.

### Known issues
- Will create the file `PJONLocalFile.dat` in the parent directory. This
file can be deleted when your client is not running. The name and location of the file can be specified using the preprocessor define PJON_LF_FILENAME which is "../PJONLocalFile.dat" by default.
