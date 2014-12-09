PJON 
====

PJON **(Padded Jittering Operative network)** is a multi-master, single wire, addressed and checked communication protocol for Arduino. Every connected device has an ID (one byte address) and can communicate with any other device on the network with package correctness acknowledge. Every package sent is checked by a one byte Cyclic Redundancy Check and is addresses to a particular device, for this reason is practically impossible to miss a package (if the receiver will not receive the sender will know) or to receive uncorrect data.



.??+++==~                               
 Z$?$??+==..                            
7I777III?+=                             
. .$7777II?+.                           
   :$O$$7OI?                            
   Z$$777$I?I                           
 . Z$$ZDI7II$                           
 .Z$ZZZ7OIII7                           
 .$$$$$7777?I?.                         
 Z$$$$$77I?++=:, .                      
.$$$$7I?+=~,::,,,,                      
 7$7?+~::::,:,,,,...                    
.???+=~:::::::,,,,...                   
.???+===:=::::,,,,,..                   
 +??+==++~~~~=:::,,,...                 
  ??======~~~:::::,:,:,.7               
 .??+====~=~~~~~::?,ZZI+.$              
 . ??++======~~~~::OOOZOZ=7$.           
    II++=+====~~~88OZ~~OZ?+=$..         
     ,II?I+=++D888=:::OOO~,?+ZZ.        
      .I?I7I+?+===~~OO8Z::~Z$+OOZ.      
      . III??I?77OOO8O$$$7Z$$$I?888.    
        .+III????I77ZI7??I?I??+7$$OD    
        . ??? .7$.    ~Z$.++I7II++?78N. 
       . ,+ ..III$I.         ,?+?$$7I$. 
   .?:~++~=?..I?               ?7$ZOZZI 
     .I?.                         8OO$$.
     . .                          .. 7. 



