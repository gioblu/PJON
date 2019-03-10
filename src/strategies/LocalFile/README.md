## LocalFile

**Medium:** Hard disk

`LocalFile` uses a file present on the hard drive to let multiple processes communicate on the same machine. It can be used for inter-process communication and for developing, simulating and testing applications and networks on a real time operative system without having to deploy physical hardware.

### Configuration

Before including `PJON.h` it is possible to configure `LocalFile` using predefined constants:

| Constant           | Purpose                                      | Supported value                            |
| ------------------ |--------------------------------------------- | ------------------------------------------ |
| `LF_POLLDELAY`     | Poll interval                                | Duration in milliseconds (20 by default)   |
| `LF_FILENAME`      | Name and location of the file used as medium | Duration in microseconds (1500 by default) |
| `LF_QUEUESIZE`     | Size of the packets queue                    | > 0 (20 by default)                        |

Pass the `LocalFile` type as PJON template parameter to instantiate a PJON object ready to communicate through this strategy.
```cpp  
  // Use LocalFile strategy with PJON device id 44
  PJON<LocalFile> bus(44);
```
After testing or simulation you may want to use conditional compiling and exchange this strategy with the actual one on your target hardware.

The directory `examples/LINUX/Local/LocalFile/PingPong` contains examples. To build these on Linux, simply type "make". To build on Windows, open the solution file in Visual Studio 2017.

Reading messages is based on polling. The poll interval in milliseconds is defined by the pre-processor definition `LF_POLLDELAY` which is 10 by default. Decreasing this value will increase the communication speed but also use more CPU and cause more disk activity.

The strategy uses a file where messages are persisted. A queue of the last messages is kept there, with the queue size set by the pre-processor define `LF_QUEUESIZE` which is 20 by default.

### Known issues
- Will create the file `PJONLocalFile.dat` in the parent directory. This
file can be deleted when your client is not running. The name and location of the file can be specified using the pre-processor define `LF_FILENAME` which is `../PJONLocalFile.dat` by default.
