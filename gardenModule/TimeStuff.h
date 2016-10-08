#ifndef TimeStuff_h
#define TimeStuff_h
#include <TimeLib.h>
const int timeZone = 10;     // aus sydney

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address);
time_t getNtpTime();

#endif

