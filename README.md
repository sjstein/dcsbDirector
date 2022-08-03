# dcsbDirector
A utility to consume DCSBios UDP streams, parse and manipulate, and send to a serial device (i.e. arduino)


**NOTE**: Currently, this tool is configured to scrape the DCS-BIOS stream for data relevant to the A-10 CDU display only. 
It keeps a "memory map" of the display state and will send any changes in that state (per iteration) over the serial 
port in one of two forms. Either an entire line, or a single character (for efficiency). The destination of these 
messages is intended to be a USB attached Arduino which is handling the interaction with the hardware display. 

In the repo sub-directory "Serial-display-line" is an Arduino sketch which handles the hardware interface to the
display and handshakes with dcsbDirector over the (virtual) serial port (USB).

#### Usage:

  **python dcsbDirector --ipaddr --port --serial --debug**

    where:  ipaddr = multicast IP# to listen for (defaults to standard DCS-BIOS MC addr)
            port = multicast port to listen for
            serial = serial port to write to
            debug = integer specifying a level of debugging output (0 - 3)


##### Comments: 

Included in this repo is the tool stest.py which was used for debugging / analysis of the 
serial handshaking between the PC and the Arduino. It is **NOT** needed for operation.

##### Issues with SYS -> EGI display
As of July 2022, the SYS -> EGI display does not function properly. This is (apparently) a bug within the DCS 
base distribution. See EGI_bug.md within this repo for explanation.

##### Missing W in initial startup screen 
At startup of DCS-BIOS and the A-10 CDU, the top line should read:
`WAYPT     0   D5/B1`
however, as of July 2022, this will initially show up as:
` AYPT     0   D5/B1`

Upon investigating the raw stream coming from DCS-BIOS, this appears to be a latent bug somewhere
within either DCS-BIOS or the underlying message routine from DCS. The 'W' is missing. 

This can be handled either within dcsbDirector with a brute-force insertion of the character
(ugly), or perhaps some more digging into the DCS-BIOS data stream. 

Either way, as it stands, the 'W' will be missing until a screen referesh of the CDU as 
dcsbDirector sees no need to update the value when it is static.
