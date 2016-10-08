#ifndef Structs_h
#define Structs_h

#include <WString.h>

#define TIMERSCOUNT 10
#define TIMESCOUNT 10

typedef struct  {
  byte Time; //start time in 10min.
  byte duration;//in minuets
}structTimes;

typedef struct  {
  byte frequancy; //in days, on every n days
  byte daysOn;// byte each bit corrosponds to a day  s,f,t,w,t,m,s,0    // 0,s,m,t,w,t,f,s
  structTimes Times[TIMESCOUNT];
  byte nextTime;
  int ranTodayCount;
  byte SOLINOIDENumber; // the pin the solenoid is on
}structTimerWeekly;

structTimerWeekly TimerWeekly[TIMERSCOUNT];
structTimerWeekly *runningTimer = &TimerWeekly[0];

void ShuffleElements(structTimes* arrayPtr , byte Length, byte insertAt, byte dir) {

     Serial.print("ShuffleElements  - ");
  if (dir == 0) {                          //shuffle down
    for (int i = Length - 1; i > insertAt ; i--) {
   //   arrayPtr[i] =  arrayPtr[i - 1];
      arrayPtr[i].Time = arrayPtr[i-1].Time;
      arrayPtr[i].duration =arrayPtr[i-1].duration;
 //     Serial.println(i);
    }
    //arrayPtr[insertAt].Time = 0;
    //arrayPtr[insertAt].duration = 0;
  } else {                                  //shuffle up
    for (int i = insertAt  ; i < Length - 1; i++) { 
     // arrayPtr[i] =  arrayPtr[i + 1];
      arrayPtr[i].Time = arrayPtr[i+1].Time;
      arrayPtr[i].duration =arrayPtr[i+1].duration;
 //      Serial.println(i);
    }
    //arrayPtr[Length].Time = 0;
    //arrayPtr[Length].duration = 0;
  }
       Serial.print(" end");
}


#endif
