## LocalFile

**Medium:** Hard disc

With the `LocalFile` PJON strategy hardware agnostic testing can be performed. This strategy is for developing and testing your business logic while not on a microcontroller but on PC.

### Why PJON over LocalFile
When developing often components that should communicate with each other reside on one hardware (Windows, Mac or Linux System).

When testing the roundtrip to the actual microcontroller is cumbersome. This strategy can be used
to test and develop your business logic or protocols without having to actually deploy sending and receiving components to actual hardware.

### How to use LocalFile
Pass the `LocalFile` type as PJON template parameter to instantiate a PJON object ready to communicate through this strategy.
```cpp  
  // Use LocalFile strategy with PJON device id 44
  PJON<LocalFile> bus(44);
```
Often you want to use conditional compiling and exchange this strategy with the actual one on your targeted hardware.

### Known issues
- Quite slow
- Will write files `pjon_content` and `pjon_semaphore` to the current working directory. These
files can be deleted when your client is not running.

### Also read
Readme.md in the tests for examples.
