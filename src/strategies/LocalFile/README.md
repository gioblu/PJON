## LocalFile

**Medium:** Hard disk

`LocalFile` uses a local file present on the hard drive, it is generally useful for developing, simulating and testing your network on a real time operative system without having to deploy physical hardware.

### How to use LocalFile
Pass the `LocalFile` type as PJON template parameter to instantiate a PJON object ready to communicate through this strategy.
```cpp  
  // Use LocalFile strategy with PJON device id 44
  PJON<LocalFile> bus(44);
```
After testing or simulation you may want to use conditional compiling and exchange this strategy with the actual one on your target hardware. The [test](test/README.md) directory contains some examples and tests.  

### Known issues
- Quite slow
- Will write files `pjon_content` and `pjon_semaphore` to the current working directory. These
files can be deleted when your client is not running.
