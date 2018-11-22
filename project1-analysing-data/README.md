# Project 1: wifistats.c
The goal of this project is to analyse wifi transmission logs. 

## UNFORTUNATELY, FINAL SOLUTION WAS LOST AFTER SUBMISSION. CURRENT SOLUTION MAY CONTAINS BUGS.

The logs are kept in columns:  
- Time (seconds.microseconds)  
- Transmitter's MAC address  
- Receiver's MAC address  
- Packet byte length  

Time | transmitter | receiver | packet length
--- | --- | --- | ---
1503251374.551604|	00:8e:f2:c0:13:cc|	00:11:d9:20:aa:4e|	157
1503251374.195670|	00:8e:f2:c0:13:cc|	00:11:d9:20:aa:4e|	397
1503251374.205047|	00:8e:f2:c0:13:cc|	00:11:d9:20:aa:4e|	397

Transmission log sample:  
> sample-packets.txt

## Features
> prompt> ./wifistats *what* **packets**

*what* : 't' or 'r' requests statistics about WiFi transmitters or receivers respectively.  
**packets** : transmission logs  
  
Output: MAC Address, sum of packet byte length  
  
Example:  
\>> ./wifistats t sample-packets.txt
   
MAC Address | packet sum
 --- | ---
00:8e:f2:c0:13:cc|	951
84:1b:5e:a8:bf:7f|	929
84:1b:5e:a8:bf:7c|	259
74:e2:f5:17:96:89|	138

> prompt> ./wifistats what packets ***OUIfile***

***OUIfile*** : Names of the vendors 

Vendors can be identified using the MAC address (first half). 
e.g. 
if MAC Address is 84:1b:5e:a8:bf:7f
then the Vendor is NETGEAR (84-1B-5E)

Output: vendor OUI, vendor name, packet sum

Example:  
\>> ./wifistats t sample-packets.txt
   
MAC Address | packet sum sample-OUIfile-small
 --- | ---
84:1b:5e|	NETGEAR	1188
??:??:??|	UNKNOWN-VENDOR	951
74:e2:f5|	Apple	138


## To compile
\>> gcc -std=c99 -Wall -Werror -pedantic -o wifistats wifistats.c
