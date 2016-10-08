#ifndef Timers_h
#define Timers_h

#include "structs.h";

unsigned int startTime = 0;
byte timerindex = 0;


// include queue library header.
//#include <QueueList.h>
//QueueList <structTimerWeekly> queue;

#include"Timers.h"

#include "structs.h";
int waterRunnning = false;




//AddTimer(SOLINOIDE2 , 2 , 0 , 6*6 ,15 )   // runs a timer every seccond day at 6am for 15 min
//AddTimer(SOLINOIDE3 , 0 , b100000 , 7*6 ,30 )   // runs a timer every monday at 7am for 30 min
//AddTimer(SOLINOIDE4 , 0 , b100010 , 6*12 ,30 )   // runs a timer every monday and friday at 12pm for 30 min (12pm-6am)*60min /10


void AddTimer(byte pinNum, byte frequancy , byte daysOn , byte startTime, byte duration);// adds a new timer , saves to eeprom.




//Call EEPROM.begin(4096); in setup()

void eeprom_read_block(uint8_t * data, uint32_t address, size_t len);


void eeprom_write_block(uint8_t * data, uint32_t address, size_t len);

void saveConfig(); //to eeprom

void loadConfig();// from eeprom

//AddTimer(SOLINOIDE2 , 2 , 0 , 0 ,15 )   // runs a timer every seccond day at 6am for 15 min
//AddTime(0, 11*6,15) // now has solinoide2 running at 6 am and 5pm for 15min every second day


void AddTime(byte index , byte startTime, byte duration); // adds another time for this schedule, sorted

//AddTimer(SOLINOIDE2 , 2 , 0 , 6*6 ,15 );   // runs a timer every seccond day at 6am for 15 min
//AddTime(0, 11*6,15); // now has solinoide2 running at 6 am and 5pm for 15min every second day
//RemoveTime(0,0,15); // now has solinoide2 only running at 5pm for 15min every second day

void RemoveTime(byte index , byte startTime, byte duration);//removes a trigger time from a timer.

void RemoveTimer(byte timerIndex); //removes an indivudal timer ans saves chanes to eeprom.

void ClearAllTimers();//deletes all timers ans saves the state to eeprom

  void ResetCounters() ;// resets daily how many time the time has run.

  void UpdateDayOfWeek(); //updates which days frequancy based timers run.


  void CheckTimers(); //checks and turns on which timers need to run. only alowys one timer to run at a time.


  String PrintTimersHTML() ;// converts the timers to a html table
  String daysOntoStr(byte days); //convertes the byte days on in the format s,f,t,w,t,m,s,0 = 11001100 to a string sf--tm-
  void PrintTimers();//converts the timers to the serial port
 
#endif

