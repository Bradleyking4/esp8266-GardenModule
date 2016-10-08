/*Created By bradley King on the 15-5-2016
   web connectable gardering watersystem.
*/

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>


ESP8266WebServer server(80);
#include "Pinout.h"
#include "Timers.h"

//for LED status
#include <Ticker.h>
Ticker ticker;

#include "EEPROM.h"

//TIME
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "structs.h";
#include "TimeStuff.h"
unsigned long OTAtimer = 1000 * 60 * 5; //allow ota update for 5 min

unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServerIP;

const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void HandleAddTime() {

  //AddTime(byte index , byte startTime, byte duration)

  String content = "<html><body><form action='/add' method='GET'>To add Time <br>";
  content += "index:<input type='text' name='index' placeholder='index'><br>";
  content += "startTime:<input type='text' name='startTime' placeholder=''>:<input type='text' name='startTimeMin' placeholder=''><br>";
  content += "duration:<input type='text' name='duration' placeholder='duration'><br>";


  content += "<input type='submit' name='SUBMIT' value='Submit'></form> msg <br>";
  server.send(200, "text/html", content);
}

void HandleRemoveTime() {

  //AddTime(byte index , byte startTime, byte duration)

  String content = "<html><body><form action='/remove' method='GET'>To remove Time please enter index of timer, start and duration<br>";
  content += "index:<input type='text' name='index' placeholder='index'><br>";
  content += "startTime:<input type='text' name='startTime' placeholder=''>:<input type='text' name='startTimeMin' placeholder=''><br>";
  content += "duration:<input type='text' name='duration' placeholder='duration'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form> msg <br>";
  server.send(200, "text/html", content);
}

void HandleRemoveTimer() {

  //AddTime(byte index , byte startTime, byte duration)

  String content = "<html><body><form action='/remove' method='GET'>To remove Timer please enter index of timer<br>";
  content += "index:<input type='text' name='index' placeholder='index'><br>";
  content += "<input type='submit' name='SUBMIT' value='timer'></form> msg <br>";
  server.send(200, "text/html", content);
}

void HandleAddTimer() {

  //AddTimer(byte pinNum, byte frequancy , byte daysOn , byte startTime, byte duration)

  String content = "<html><body><form action='/add' method='GET'>To add Timer, please enter either Days on or Frequancey<br>";
  content += "SOLINOIDENumber:<input type='text' name='SOLINOIDENumber' placeholder='SOLINOIDE Number'><br>";
  content += "frequancy:<input type='text' name='frequancy' placeholder='frequancy'><br>";
  content += "daysOn:<input type='text' name='daysOn' placeholder='daysOn binary bit patten'><br>";
  content += "startTime:<input type='text' name='startTime' placeholder=''>:<input type='text' name='startTimeMin' placeholder=''><br>";
  content += "duration:<input type='text' name='duration' placeholder='duration'><br>";

  content += "<input type='submit' name='SUBMIT' value='Submit'></form> msg <br>";
  server.send(200, "text/html", content);
}


String form = "<form action='/led'><input type='radio' name='state' value='1' checked>On<input type='radio' name='state' value='0'>Off<input type='submit' value='Submit'></form>";

void OTAsetup() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("ANUbus Watering System");

  // No authentication by default
  //   ArduinoOTA.setPassword((const char *)"ANUbus");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  OTAtimer = millis() * 1000 * 60 * 5;
}

void printDigits(int digits) {
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  Serial.println();
}



//---------------------------------------------------------------------------------------------------------------------------------------------------------------------


void setup() {
  WiFiManager wifiManager;
  pinSetup();

  EEPROM.begin(4096);

  Serial.begin(115200);

  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around

  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration

  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected");
  ticker.detach();
  //turn LED off
  digitalWrite(BUILTIN_LED, HIGH);

  OTAsetup();


  Serial.println("Starting UDP");
  udp.begin(localPort);
  delay(1000);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());

  delay(1000);


  setSyncProvider(getNtpTime);



  server.on("/led", []() {
    String state = server.arg("state");
    if (state == "0") digitalWrite(BUILTIN_LED, HIGH);
    else if (state == "1") digitalWrite(BUILTIN_LED, LOW);
    server.send(200, "text/html", form);
  });// - See more at: http://www.esp8266.com/viewtopic.php?f=29&t=2153#sthash.kJDEndiX.dpuf

  server.on("/", []() {
    if (server.hasArg("submit")) {
      if (server.arg("submit") == "clear") {
        ClearAllTimers();
      }
    }
    server.send(200, "text/html",  PrintTimersHTML());
  });// - See more at: http://html.esp8266.com/viewtopic.php?f=29&t=2153#sthash.kJDEndiX.dpuf

  server.on("/addTimer", []() {
    HandleAddTimer();
  });// - See more at: http://html.esp8266.com/viewtopic.php?f=29&t=2153#sthash.kJDEndiX.dpuf

  server.on("/allowProgram", []() {
    OTAtimer += millis();
    long rssi =  WiFi.RSSI();
    String content = "Programming Mode Enabled for 5 min " + String(rssi) +"<br> <a href='/'>Return</a>";
    server.send(200, "text/html", content);


  });// - See more at: http://html.esp8266.com/viewtopic.php?f=29&t=2153#sthash.kJDEndiX.dpuf


  server.on("/addTime", []() {
    HandleAddTime();
  });// - See more at: http://html.esp8266.com/viewtopic.php?f=29&t=2153#sthash.kJDEndiX.dpuf

  server.on("/removeTime", []() {
    HandleRemoveTime();
  });// - See more at: http://html.esp8266.com/viewtopic.php?f=29&t=2153#sthash.kJDEndiX.dpuf

  server.on("/removeTimer", []() {
    HandleRemoveTimer();
  });// - See more at: http://html.esp8266.com/viewtopic.php?f=29&t=2153#sthash.kJDEndiX.dpuf

  server.on("/clear", []() {
    server.send(200, "text/html", "<form action='/' method='GET'> daysOn:<input type='text' name='daysOn' placeholder='daysOn binary bit patten'><br> Clear AllTimers ? <input type='submit' name='SUBMIT' value='clear'></form> <a href='/'>Return To Timers</a>");

  });// - See more at: http://html.esp8266.com/viewtopic.php?f=29&t=2153#sthash.kJDEndiX.dpuf

  server.on("/add", []() {
    String msg;
    if (server.hasArg("index") && server.hasArg("startTime") && server.hasArg("startTimeMin") && server.hasArg("duration")) {
      AddTime(server.arg("index").toInt(), server.arg("startTime").toInt() * 6 +  server.arg("startTimeMin").toInt() / 10 , server.arg("duration").toInt());

      msg =  "Time Added";
    }

    if (server.hasArg("SOLINOIDENumber") && server.hasArg("frequancy") && server.hasArg("daysOn") && server.hasArg("startTime") && server.hasArg("startTimeMin") && server.hasArg("duration")) {
      AddTimer(server.arg("SOLINOIDENumber").toInt(), server.arg("frequancy").toInt() , server.arg("daysOn").toInt() , server.arg("startTime").toInt() * 6 +  server.arg("startTimeMin").toInt() / 10, server.arg("duration").toInt());

      msg =  "Timer Added";
    }
    server.send(200, "text/html", msg + "<br> <a href='/'>Return To Timers</a>");
  });// - See more at: http://html.esp8266.com/viewtopic.php?f=29&t=2153#sthash.kJDEndiX.dpuf

  server.on("/remove", []() {
    String msg;
    if (server.hasArg("index") && server.hasArg("startTime") && server.hasArg("startTimeMin") && server.hasArg("duration")) {
      RemoveTime(server.arg("index").toInt(), server.arg("startTime").toInt() * 6 +  server.arg("startTimeMin").toInt() / 10 , server.arg("duration").toInt());

      msg =  "Time Removed";
    }
    if (server.hasArg("index") && server.hasArg("SUBMIT")) {
      if ( server.arg("SUBMIT") == "timer") {
        RemoveTimer(server.arg("index").toInt());
        msg =  "Timer Removed";
      }
    }
    server.send(200, "text/html", msg + "<br> <a href='/'>Return To Timers</a>");
  });// - See more at: http://html.esp8266.com/viewtopic.php?f=29&t=2153#sthash.kJDEndiX.dpuf

  if ( EEPROM.read(0) == sizeof(structTimerWeekly)) { //check to make sure the config settings havent changed
    loadConfig();

  } else {

    AddTimer(3 , 0 , B10000010 , 5 * 6 , 30 );
    AddTime(0, 5 * 6 + 3, 10);
    AddTime(0, 11 * 6, 10);

    AddTimer(2 , 2 , 0 , 7 * 6 , 20 ); // runs a timer every seccond day at 6am for 15 min
    AddTime(1, 11 * 6, 30); // now has solinoide2 running at 6 am and 5pm for 15min every second day
    //RemoveTime(0,0,15); // now has solinoide2 only running at 5pm for 15min every second day
  }
  Serial.println("0");
  UpdateDayOfWeek();


  server.begin();
  int shields =  analogRead(SHIELDSENSORS);
  Serial.println("1");
  if (shields == 1) // no shields internal pullup, 4 solenoid's
  {

  }
  if (shields == 0.4) // 4 additional solenoids
  {

  }
  if (shields == 0.2) // 2 additional solenoids + 2 opto in's
  {

  }
  if (shields == 0.1) // 2 additional solenoids + 1 opto in + 3 pin(flow)
  {

  }
  if (shields == 0) // additional mcu.
  {

  }
  PrintTimers();
  digitalClockDisplay();
}

time_t prevDisplay = 0; // when the digital clock was displayed



void Sleep() {

  delay(1000 * 60 * 60); //sleep for 3 hours
  delay(1000 * 60 * 60);
  delay(1000 * 60 * 60);
}

void errorMSG(int errorNUM) {
  Serial.print(errorNUM);
  switch (errorNUM) {
    case 1: Serial.println(" - water run at stop time"); break;
    case 2: Serial.println(" - no timers"); break;
    case 3: Serial.println(" - that time not found"); break;
    case 4: Serial.println(); break;
    case 5: Serial.println(" - timers Full"); break;

    default:  Serial.println(" - unknown error");
  }
}

void loop()
{
  server.handleClient();
  if (millis() < OTAtimer) {
    ArduinoOTA.handle();
  }

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_STA);
    ////
    //  WiFiManager wifiManager;
    //wifiManager.autoConnect();

    //
  }

  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //check only if time has changed
      prevDisplay = now();
      //digitalClockDisplay();
      if (hour() < 22) { //should not be watering the plants after 10pm
        CheckTimers() ;
      } else
      {
        ResetCounters();
        Sleep();
        UpdateDayOfWeek();
      }
    }
  } else
  {
    setSyncProvider(getNtpTime);
  }
}

