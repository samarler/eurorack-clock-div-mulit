
/*****************************************************************************
 Version 0.2.17.22
 This code is for a module that provides a clock, a clock divider and a clock
 multiplier for modular synthesis. The module has two operating modes, Internl
 (the module provides its own clock signal) and External (the module relies on
 an external clock running at 24 pules per quarter note), and divides and 
 multiplies by 2, 4 and 8.

 Code is written for the Adafruit ItsyBitsy 32u4 5V.
 
 Pin assignments:
   4 - Switch for internal/external clock source
   6 - External clock input
   8 - Internal clock output
   5 - /8
   7 - /4
   9 - /2
  10 - x2
  11 - x4
  12 - x8
  A0 - Connected to 10k pot for internal clock rate

 Written by Scott Marler. GNU General Public License v3.0, check license.txt
 for more information.
 

 **************************************************************************/
/**************************************************************************
 * 
 * Define varialbes used throughout the program
 * 
 * 
 */
unsigned long interval;
unsigned long lastUpdate;
int DpulsePins[4] = {8,9,7,5};
int MpulsePins[4] = {10,11,12};
int Mticks[8] = {1,4,7,10,13,16,19,22};
int ticks;
int DivStep = 1;
int MulStep = 1;
int extTick = 1;
int extState = 0;
int clkSwState = 0;

void setup() {
  Serial.begin(9600);
  delay(1000);
  pinMode(4,INPUT_PULLUP); //Switch
  pinMode(5,OUTPUT);       //Divide by 8
  pinMode(6,INPUT_PULLUP); //External clock input
  pinMode(7,OUTPUT);       //Divide by 4
  pinMode(8,OUTPUT);       //Internal clock output
  pinMode(9,OUTPUT);       //Divide by 2
  pinMode(10,OUTPUT);      //Multiply bsay 2
  pinMode(11,OUTPUT);      //Multiply by 4
  pinMode(12,OUTPUT);      //Multiply by 8
}

void loop() {
  int curSwSample = 0;
  for(int i=0;i<100;i++) {
    curSwSample = curSwSample + digitalRead(4);
  }
  int curSw = curSwSample/100;
  if(curSw != clkSwState) {
    clkSwState = curSw;
    DivStep = 0;
    MulStep = 0;
    extTick = 0;
    extState = 0;
  }

  if(clkSwState > 0) {
    extClkRead();
    if(extTick == 1) {DivStep++;clockDivider(DivStep);}
    if(DivStep >= 8) {DivStep = 0;}
    for(int i=0;i<8;i++) {
      if(extTick == Mticks[i]) {
        MulStep++;
        clockMultiplier(MulStep);
        if(MulStep >= 8) {MulStep = 0;}
        break;
      }
    }
    if(extTick >= 24) {extTick = 0;}
  } else {
    int bpm = bpmRead();
    int tickMillis = 60000/bpm/8;
    if( millis() - lastUpdate > tickMillis) {
      lastUpdate = millis();
      MulStep++;
      if(MulStep == 1) {DivStep++;clockDivider(DivStep);}
      clockMultiplier(MulStep);
      if(MulStep >= 8) {MulStep = 0;}
      if(DivStep >= 8) {DivStep = 0;}
    }
  }
}

/*
 * Read A0 10 times and use average to map to 40 to 180 bpm
 */
int bpmRead() {
  int bpmSample = 0;
  for(int i=0;i<10;i++) {
    bpmSample = bpmSample + analogRead(A0);
  }
  int bpm = map(bpmSample/10,0,1023,40,180);
  return bpm;
}

/*
 * Check incoming clock, if HIGH increment tick count, wait until goes LOW to
 * enable incrementing tick count again
 * when tick count reaches 24 reset to 0
 */
int extClkRead() {
  if(extState == 0) {
    if(digitalRead(6) > 0) {
      extTick++;
      extState = 1;
    }
  } else {
    if(digitalRead(6) == 0) {
      extState = 0;
    }
  }
}
/*
 * Divide function
 */
void clockDivider(int d) {
  if((d%8) == 0) {pulse(DpulsePins,4,4);}
  else if((d%4) == 0) {pulse(DpulsePins,4,3);}
  else if((d%2) == 0) {pulse(DpulsePins,4,2);}
  else {pulse(DpulsePins,4,1);} 
}

/*
 * Muliply function (external clock)
 */
void EclockMultiplier(int c){
    if(c == 4 || c == 10 || c == 16 || c == 22) {
      pulse(MpulsePins,3,1);
    }
    switch(c) {
      case 1:
        pulse(MpulsePins,3,3);
        break;
      case 7:
        pulse(MpulsePins,3,2);
        break;
      case 13:
        pulse(MpulsePins,3,3);
        break;
      case 19:
        pulse(MpulsePins,3,2);
        break;
      default:
        break;
    }

  }
/*
 * Multiply function (internal clock)
 */
  void clockMultiplier(int c){
    switch(c) {
      case 1:
        pulse(MpulsePins,3,3);
        break;
      case 3:
        pulse(MpulsePins,3,2);
        break;
      case 5:
        pulse(MpulsePins,3,3);
        break;
      case 7:
        pulse(MpulsePins,3,2);
        break;
      default:
        pulse(MpulsePins,3,1);
        break;
    }

  }

/*
 * Function for sending out trigger/gate signal
 * Array is pins to bring high for 20ms an then back to low
 */
void pulse(int pins[],int arraySize, int num) {
    unsigned long Stime = millis();
    for(int i=0;i<num;i++) {
      digitalWrite(pins[i],HIGH);
    }
    while(millis() - Stime < 20) {}
    for(int i=0;i<num;i++) {
      digitalWrite(pins[i],LOW);
    }
}
