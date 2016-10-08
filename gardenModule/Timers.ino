// include queue library header.
//#include <QueueList.h>
//QueueList <structTimerWeekly> queue;

#include"Timers.h"

#include "structs.h";

//AddTimer(SOLINOIDE2 , 2 , 0 , 6*6 ,15 )   // runs a timer every seccond day at 6am for 15 min
//AddTimer(SOLINOIDE3 , 0 , b100000 , 7*6 ,30 )   // runs a timer every monday at 7am for 30 min
//AddTimer(SOLINOIDE4 , 0 , b100010 , 6*12 ,30 )   // runs a timer every monday and friday at 12pm for 30 min (12pm-6am)*60min /10


void AddTimer(byte pinNum, byte frequancy , byte daysOn , byte startTime, byte duration) {
  byte tempindex = -1;
  do {
    tempindex++;
  } while (  TimerWeekly[tempindex].SOLINOIDENumber != 0 && tempindex < TIMERSCOUNT );


  if (tempindex == TIMERSCOUNT) {
    errorMSG(5);
  } else {

    TimerWeekly[tempindex].SOLINOIDENumber = pinNum;
    TimerWeekly[tempindex].frequancy = frequancy;
    TimerWeekly[tempindex].daysOn = daysOn ;
    if ( TimerWeekly[tempindex].frequancy != 0) {
      for ( int d = 0 ; d < 6 ; d++) {
        if (((day() + d) % TimerWeekly[tempindex].frequancy) == 0) {
          int DayOfweek = ((weekday() + d - 1) % 7) + 1 ;
          bitSet(TimerWeekly[tempindex].daysOn, DayOfweek );  //we want to play with bits 1-7
        }
      }
    }


    TimerWeekly[tempindex].Times[0].Time = startTime;
    TimerWeekly[tempindex].Times[0].duration = duration;

    TimerWeekly[tempindex].nextTime = 0;
  }
  
  saveConfig() ;
}




//Call EEPROM.begin(4096); in setup()

void eeprom_read_block(uint8_t * data, uint32_t address, size_t len){
  int i;
  for(i=0; i<len; i++){
    data[i] = EEPROM.read(address+i);
  }
}

void eeprom_write_block(uint8_t * data, uint32_t address, size_t len){
  int i;
  for(i=0; i<len; i++){
    EEPROM.write(address+i, data[i]);
  }
  EEPROM.commit();
}

void saveConfig() {
EEPROM.write(0, sizeof(structTimerWeekly));
//eeprom_write_block(TimerWeekly,1,sizeof(TimerWeekly));
//eeprom_write_block(TimerWeekly,2,sizeof(TimerWeekly));
 eeprom_write_block((uint8_t *)TimerWeekly,3,sizeof(TimerWeekly));
}

void loadConfig(){
 eeprom_read_block((uint8_t *)TimerWeekly,3,sizeof(TimerWeekly)); 
}


//AddTimer(SOLINOIDE2 , 2 , 0 , 0 ,15 )   // runs a timer every seccond day at 6am for 15 min
//AddTime(0, 11*6,15) // now has solinoide2 running at 6 am and 5pm for 15min every second day

void AddTime(byte index , byte startTime, byte duration) { // adds another time for this schedule, sorted
  if (TimerWeekly[index].SOLINOIDENumber == 0) {
    errorMSG(2);//check if there is a timer to add to.
  }

  Serial.print("AddTime ");

  if (TimerWeekly[index].Times[0].duration == 0) { //if there is no current times.
    TimerWeekly[index].Times[0].Time = startTime;
    TimerWeekly[index].Times[0].duration = duration;
  } else {

    if (startTime <= TimerWeekly[index].Times[0].Time) { // before first time

      ShuffleElements((structTimes*)TimerWeekly[index].Times , TIMESCOUNT  , 0 , 0);

      TimerWeekly[index].Times[0].Time = startTime;
      TimerWeekly[index].Times[0].duration = duration;

    } else {

      byte timesIndex = 0;
      do { // at some other point
        timesIndex++;
      }  while ((startTime >= TimerWeekly[index].Times[timesIndex].Time && TimerWeekly[index].Times[timesIndex].duration != 0 ) && timesIndex < TIMESCOUNT   );

      ShuffleElements((structTimes*)&TimerWeekly[index].Times[0], TIMESCOUNT , timesIndex , 0);

      Serial.println("- end");
      TimerWeekly[index].Times[ timesIndex].Time = startTime;
      TimerWeekly[index].Times[ timesIndex].duration = duration;
    }
  }
  saveConfig() ;
}


//AddTimer(SOLINOIDE2 , 2 , 0 , 6*6 ,15 );   // runs a timer every seccond day at 6am for 15 min
//AddTime(0, 11*6,15); // now has solinoide2 running at 6 am and 5pm for 15min every second day
//RemoveTime(0,0,15); // now has solinoide2 only running at 5pm for 15min every second day

void RemoveTime(byte index , byte startTime, byte duration) {
  byte tempindex = 0;
  while (TimerWeekly[index].Times[tempindex].Time != startTime && tempindex < TIMESCOUNT) {
    tempindex++;
  }
  ShuffleElements((structTimes*)TimerWeekly[index].Times, TIMESCOUNT , tempindex , 1);
  TimerWeekly[index].Times[TIMESCOUNT].Time = 0;
  TimerWeekly[index].Times[TIMESCOUNT].duration = 0;
  saveConfig() ;
}

void RemoveTimer(byte timerIndex){
  if (runningTimer == &TimerWeekly[timerindex]){
    waterRunnning = false;
  }
  TimerWeekly[timerIndex].nextTime = 0;
    digitalWrite(   solenoides[ TimerWeekly[timerIndex].SOLINOIDENumber], !PINON);
    TimerWeekly[timerIndex].daysOn = 0;
    TimerWeekly[timerIndex].ranTodayCount = 0;
    TimerWeekly[timerIndex].SOLINOIDENumber = 0;
    TimerWeekly[timerIndex].frequancy = 0;

    for (int times = 0; times < 10 ; times++) {
      TimerWeekly[timerIndex].Times[times].Time = 0;
      TimerWeekly[timerIndex].Times[times].duration = 0;
    }
    
    saveConfig() ;
  
}

void ClearAllTimers() {
  waterRunnning = false;
  for (int timers = 0; timers < TIMERSCOUNT ; timers++) {
    RemoveTimer(timers);
  }
  saveConfig();
}

  void ResetCounters() {
    for (int i = 0; i < TIMERSCOUNT ; i++) {
      TimerWeekly[i].nextTime = 0;
      TimerWeekly[i].ranTodayCount = 0;
      digitalWrite(   solenoides[ TimerWeekly[i].SOLINOIDENumber], !PINON); // should not be needed....
    }
    timerindex = 0;
    if ( waterRunnning ) {
      errorMSG(1); // timers should of stopped by now
      waterRunnning = false;
    }
  }

  void UpdateDayOfWeek() {
    Serial.println("UpdateDayOfWeek");
    for (int i = 0; i < TIMERSCOUNT ; i++) {
      if ( TimerWeekly[i].frequancy != 0) {
        TimerWeekly[i].daysOn= 0;
        for ( int d = 0 ; d < 6 ; d++) {
          if (((day() + d) % TimerWeekly[i].frequancy) == 0) {
            int DayOfweek = ((weekday() + d - 1) % 7) + 1 ;
            bitSet(TimerWeekly[i].daysOn, DayOfweek );  //we want to play with bits 1-7
          }
        }
      }
    }
  }


  //QueueList <structTimerWeekly> queueWeekly;
  //QueueList <structTimerDaily> queueDaily;

  void CheckTimers() {
    if (waterRunnning == false) {
      int DayOfWeek = 1 <<  weekday();
      Serial.println(weekday());
      //  Serial.println(daysOntoStr(TimerWeekly[timerindex].daysOn) , BIN);
      Serial.println(daysOntoStr(DayOfWeek));
      if ( TimerWeekly[timerindex].SOLINOIDENumber != 0 ) { //is there a timer?
        Serial.println("8.5");
        Serial.println(bitRead((TimerWeekly[timerindex].daysOn), weekday()));
        Serial.print("Time:");
        Serial.println(((hour()) * 6 + minute() / 10));
        Serial.println("8.7");
        if (bitRead((TimerWeekly[timerindex].daysOn), weekday()) ) { //do we run the timer today?
          Serial.println("9");
          if  ( TimerWeekly[timerindex].nextTime < TIMESCOUNT  ) { //is there a timer left to run
            Serial.println("10");
            if  ( TimerWeekly[timerindex].Times[TimerWeekly[timerindex].nextTime].Time < ((hour()) * 6 + minute() / 10)) { // is it time yet to run?


              Serial.println("11");
              TimerWeekly[timerindex].ranTodayCount++;
              waterRunnning = true;
              digitalWrite(BUILTIN_LED, LOW);
              runningTimer = &TimerWeekly[timerindex];
              startTime = millis();
              digitalWrite(    solenoides[ TimerWeekly[timerindex].SOLINOIDENumber], PINON);


            }
          }
        }
      }
      timerindex = (timerindex + 1) % TIMERSCOUNT;
    } else {
      if ((millis() - startTime) > (runningTimer->Times[runningTimer->nextTime].duration * 1000 * 60) ) {
        while  ( runningTimer->Times[runningTimer->nextTime].Time < ((hour()) * 6 + minute() / 10)) {
          runningTimer->nextTime++;
        }
        if ( runningTimer->nextTime == TIMESCOUNT || runningTimer->Times[runningTimer->nextTime].duration == 0 )
        {
          runningTimer->nextTime = TIMESCOUNT;
        }
        waterRunnning = false;
        digitalWrite(BUILTIN_LED, HIGH);
        digitalWrite( solenoides[runningTimer->SOLINOIDENumber], !PINON);


      }
    }
  }

  /*
  <table border='1' cellpadding='5' cellspacing='0'>
    <tr>
      <td>a1</td>
      <td>b1</td>
      <td>c1</td>
      <td>d1</td>
    </tr>
    <tr>
      <td>a2</td>
      <td>b2</td>
      <td>c2</td>
      <td>d2</td>
    </tr>
  </table>
  */


  String PrintTimersHTML() {
    byte tempindex = 0;
    String result = "<table border='1' cellpadding='5' cellspacing='0'>";
    result += "<tr>";

    result += "<td>";    result += "index";     result += "</td>";
    result += "<td>";    result += "frequancy";     result += "</td>";
    result += "<td>";    result += "daysOn";     result += "</td>";
    result += "<td>";    result += "ranTodayCount";     result += "</td>";
    result += "<td>";    result += "solenoide";     result += "</td>";
    result += "<td>";    result += "timers";     result += "</td>";
    result += "</tr>";
    // server.send(200, "text/html", result);
    // result ="";
    do {
      result += "<tr>";
      result += "<td>";    result += tempindex;     result += "</td>";

      result += "<td>";    result += TimerWeekly[tempindex].frequancy;     result += "</td>";
      result += "<td>";    result += daysOntoStr(TimerWeekly[tempindex].daysOn);     result += "</td>";

      result += "<td>";    result += TimerWeekly[tempindex].ranTodayCount;     result += "</td>";
      result += "<td>";    result += TimerWeekly[tempindex].SOLINOIDENumber;     result += "</td>"; //needs to be changeto get the sole number

      result += "<td>";
      if (TimerWeekly[tempindex].Times[0].duration == 0) { //if there is no current times.
        Serial.println("hmm");
      } else {
        byte TimesIndex = 0;
        result += "<table border='1' cellpadding='5' cellspacing='0'>";
        result += "<tr>";

        result += "<td>";    result += "Start Time";     result += "</td>";
        result += "<td>";    result += "duration";     result += "</td>";
        result += "</tr>";

        while ( TimerWeekly[tempindex].Times[TimesIndex].duration != 0 && (TimesIndex <  TIMESCOUNT)) {
          result += "<tr>";
          int hours = (TimerWeekly[tempindex].Times[TimesIndex].Time  / 6);
          int mins = (TimerWeekly[tempindex].Times[TimesIndex].Time  % 6 * 10);
          result += "<td>";    result += String(hours)  + ":" + String(mins) ;     result += "</td>";
          result += "<td>";    result += TimerWeekly[tempindex].Times[TimesIndex].duration ;    result += "</td>";
          result += "</tr>";
          TimesIndex++;
        }
        result += "</table>";
      }
      result += "</td>";
      result += "</tr>";
      tempindex++;




    } while (  solenoides[ TimerWeekly[tempindex].SOLINOIDENumber] != 0 );
    result += "</table>";

    result += "<a href='/addTimer'>Add a new Timer</a> <br>";

    result += "<a href='/addTime'>Add a new Time to a Timer</a><br>";

    result += "<a href='/removeTimer'>Remove a Timer</a> <br>";

    result += "<a href='/removeTime'>Remove a Time</a> <br>";

    result += "<a href='/clear'>Clear All Timers</a> <br>";
    
    result += "<a href='/allowProgram'>Allow Programming</a>";
    
    
  result += "<br><br> Current Time is "; 
  result += hour();
 result +=  ":" ;
 result += minute() ;
 result += " on " ;
  result += daysOntoStr(1 <<  weekday());
    return result;
  }

  String daysOntoStr(byte days) {
    // s,f,t,w,t,m,s,0
    String result;
    result += bitRead(days, 1) ? "S" : "-";
    result += bitRead(days, 2) ? "M" : "-";
    result += bitRead(days, 3) ? "T" : "-";
    result += bitRead(days, 4) ? "W" : "-";
    result += bitRead(days, 5) ? "T" : "-";
    result += bitRead(days, 6) ? "F" : "-";
    result += bitRead(days, 7) ? "S" : "-";
    return result;
  }

  void PrintTimers() {
    byte tempindex = 0;
    do {
      Serial.println();
      Serial.print("index :");
      Serial.print(tempindex);

      Serial.print(" frequancy :");
      Serial.print(TimerWeekly[tempindex].frequancy);

      Serial.print(" daysOn :");
      Serial.print(daysOntoStr(TimerWeekly[tempindex].daysOn));

      Serial.print(" ranTodayCount :");
      Serial.print(TimerWeekly[tempindex].ranTodayCount);

      Serial.print(" pin :");
      Serial.print(TimerWeekly[tempindex].SOLINOIDENumber);


      Serial.println();
      if (TimerWeekly[tempindex].Times[0].duration == 0) { //if there is no current times.
        Serial.println("hmm");
      } else {
        byte TimesIndex = 0;
        while ( TimerWeekly[tempindex].Times[TimesIndex].duration != 0 && TimesIndex <  TIMESCOUNT) {
          Serial.print(" Start Time ");
          Serial.print( TimerWeekly[tempindex].Times[TimesIndex].Time  / 6 );
          Serial.print(":");
          Serial.print( TimerWeekly[tempindex].Times[TimesIndex].Time  % 6 * 10);


          Serial.print(" duration :");
          Serial.print( TimerWeekly[tempindex].Times[TimesIndex].duration);

          Serial.print(" - ");
          TimesIndex++;
        }
        Serial.println();
      }
      tempindex++;
    } while ( solenoides[ TimerWeekly[tempindex].SOLINOIDENumber]  != 0 );
  }


