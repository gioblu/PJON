## LocalFile tests

To verify the `LocalFile` PJON strategy, you find in this directory the TestClient and TestSender cpp files. To compile and start you can simply call for example
```  
  ./compileAndRunTestClient.bat 2 3 1
```
Parameters for the client are:
- The BusId this client should use
- The number of request this client waits for until the client terminates
- A boolean that indicates if an answer to the sender should be sent (0: No answer, 1 echo)

For sending to one or more clients
```  
  ./compileAndRunTestSender.bat 2 3
```
Parameters for sending are:
- Number of clients to send to
- Number of Requests being sent

**Please note:** The sender will be having busId 1, therefore sent to busIds counting from 2 on, so be aware that you do not start a client with busId 1.

For loadtesting there is the
```
   ./stresstest.bat 10 20 1
```
Job.

Parameters:
- Number of requests to be sent
- Number of clients to fork (Max 255)
- Boolean indicating whether an echo should be performed


## Tests passed so far
- Tested under windows with up to 255 clients. 
