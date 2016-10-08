#define SOLINOIDE0 13
#define SOLINOIDE1 12
#define SOLINOIDE2 14
#define SOLINOIDE3 16
#define SOLINOIDE4 10
#define SOLINOIDE5 15
#define SOLINOIDE6 4
#define SOLINOIDE7 5

static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;

byte solenoides[9] = {0, SOLINOIDE0, SOLINOIDE1, SOLINOIDE2, SOLINOIDE3,             
SOLINOIDE4, SOLINOIDE5, SOLINOIDE6, SOLINOIDE7};

#define RAINSENSOR 5
#define FLOWSENSOR 10

#define SHIELDSENSORS A0

#define PINON 1

void pinSetup() {
  for (int i = 1 ; i < 9 ; i++) {
    digitalWrite(solenoides[i], LOW);
    pinMode(solenoides[i], OUTPUT);
  }
  pinMode(A0, INPUT);
}
