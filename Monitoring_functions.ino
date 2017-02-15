//BEGIN ---- included libraries
#include <Wire.h>
#include <LiquidTWI.h>
  LiquidTWI lcd(0);
#include "RTClib.h"
#include "Adafruit_MCP9808.h"
#include <Adafruit_AM2315.h>
//END ---- included libraries

//BEGIN ---- library configuration
Adafruit_AM2315 am2315;
Adafruit_MCP9808 tempsensorac = Adafruit_MCP9808();
Adafruit_MCP9808 tempsensordc = Adafruit_MCP9808();
RTC_DS3231 rtc;
//END ---- library configuration

//BEGIN ---- defined variables
byte computer_bytes_received = 0;    //We need to know how many characters bytes have been received
byte sensor_bytes_received = 0;      //We need to know how many characters bytes have been received
int port = 0;                       //what port to open
char strng[20];               //A 20 byte character array to hold incoming data from a pc/mac/other
char sensordata[30];
char sensordatajunk[30];//A 30 byte character array to hold incoming data from the sensors
char *channel;                       //Char pointer used in string parsing
char *cmd;
float flw[9];
float dctemp;
float actemp;
float outtemp;
float blkpwr;
float redpwr;
int incomingByte;
unsigned long t=0;
unsigned long oldt =0;
unsigned long tt=0;
unsigned long bw =0;
char *flotot;
char *floinst;
long y;
int sfeedp = 0;
int snfrejectp = 0;
float sroftank = 0;
float snfftank = 0;
int spotnf= 0;
int srorejectp = 0;
float swwtank = 0;
int rospot= 0;
int smfp = 0;
float smfftank= 0;
float swastetank = 0;
boolean checkvalve = false;
int systemstate=0;
unsigned long timnow;
unsigned long treattimes[3];
//initial 12v actuated ball valve status
int mfastatus;
int mfbstatus;
int mfcstatus;
int mfdstatus;
int roastatus;
int robstatus;
int nfastatus;
int nfbstatus;
int wastestatus;
int wwrinsestatus;

//initial 120v relay status
int bubbleblaststatus=0;
int bubblerstatus=0;
int uvstatus=0;
int ozonestatus=0;
int pumpon=0;
//END ---- defined variables

//BEGIN ---- Pin definitions

const int s1 = 16;                         //pin 16 to control pin S1
const int s2 = 17;                         //pin 17 to control pin S2
const int s3 = 18;                         //pin 18 to control pin S3

//120v relays
const int bubbleblast=53;
const int bubbler=51;
const int uv=52;
const int ozone=50;
const int pump=7;

//12v solenoid valves
const int sol=43;

//12v actuated ball valves
const int mfa=27;
const int mfao=26;
const int mfac=25;

const int mfb=24;
const int mfbo=23;
const int mfbc=22;

const int mfc=30;
const int mfco=29;
const int mfcc=28;

const int mfd=36;
const int mfdo=35;
const int mfdc=34;

const int roa=49;
const int roac=47;
const int roao=48;

const int rob=33;
const int robc=31;
const int robo=32;

const int nfa=39;
const int nfao=38;
const int nfac=37;

const int nfb=2;
const int nfbo=3;
const int nfbc=4;

const int waste=42;
const int wastec=40;
const int wasteo=41;

const int wwrinse=46;
const int wwrinsec=44;
const int wwrinseo=45;

//12v stepper positioned plug valve
const int rostpen=8;
const int ros = 9;
const int rodir = 10;

const int nfstpen=11;
const int nfs = 12;
const int nfdir = 13;
//ANALOGPINS
const int feedppin=9;
const int nfrejectppin=11;
const int rorejectppin=8;
const int mfppin=7;

const int ropotpin=1;
const int nfpotpin=0;

const int roftankpin=2;
const int nfftankpin=5;
const int mfftankpin=4;
const int wwtankpin=6;
const int wastetankpin=3;

const int blkpowerpin = 12;
const int redpowerpin = 13;

//END ---- Pin definitions

//BEGIN ---- setup
void setup() {
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH);

  //BEGIN ---- pin configuration
  //ANALOGPINS
  pinMode(blkpowerpin, INPUT);
  pinMode(redpowerpin,INPUT);
  pinMode(roftankpin,INPUT);
  pinMode(nfftankpin,INPUT);
  pinMode(mfftankpin,INPUT);
  pinMode(wwtankpin,INPUT);
  pinMode(wastetankpin,INPUT);
  pinMode(ropotpin,INPUT);
  pinMode(nfpotpin,INPUT);
  pinMode(feedppin,INPUT);
  pinMode(nfrejectppin,INPUT);
  pinMode(rorejectppin,INPUT);
  pinMode(mfppin,INPUT);
  //120v relays
  pinMode(bubbleblast, OUTPUT);
  pinMode(bubbler, OUTPUT);
  pinMode(uv, OUTPUT);
  pinMode(ozone, OUTPUT);
  pinMode(pump, OUTPUT);

  //12v solenoid valves
  pinMode(sol, OUTPUT);

  //12v actuated ball valves
  pinMode(mfa, OUTPUT);
  pinMode(mfao, INPUT);
  pinMode(mfac, INPUT);

  pinMode(mfb, OUTPUT);
  pinMode(mfbo, INPUT);
  pinMode(mfbc, INPUT);

  pinMode(mfc, OUTPUT);
  pinMode(mfco, INPUT);
  pinMode(mfcc, INPUT);

  pinMode(mfd, OUTPUT);
  pinMode(mfdo, INPUT);
  pinMode(mfdc, INPUT);

  pinMode(roa, OUTPUT);
  pinMode(roao, INPUT);
  pinMode(roac, INPUT);

  pinMode(rob, OUTPUT);
  pinMode(robo, INPUT);
  pinMode(robc, INPUT);

  pinMode(nfa, OUTPUT);
  pinMode(nfao, INPUT);
  pinMode(nfac, INPUT);

  pinMode(nfb, OUTPUT);
  pinMode(nfbo, INPUT);
  pinMode(nfbc, INPUT);

  pinMode(waste, OUTPUT);
  pinMode(wasteo, INPUT);
  pinMode(wastec, INPUT);

  pinMode(wwrinse, OUTPUT);
  pinMode(wwrinseo, INPUT);
  pinMode(wwrinsec, INPUT);

  //12v stepper positioned plug valve
  pinMode(nfdir, OUTPUT);
  pinMode(nfs, OUTPUT);
  pinMode(nfstpen, OUTPUT);
  digitalWrite(nfstpen, HIGH); //initial configuration motor disabled

  pinMode(rodir, OUTPUT);
  pinMode(ros, OUTPUT);
  pinMode(rostpen, OUTPUT);
  digitalWrite(rostpen, HIGH); //initial configuration motor disabled

  //Atlas-Scientific Serial Channel Selectors
  pinMode(s1, OUTPUT);              //Set the digital pin as output
  pinMode(s2, OUTPUT);              //Set the digital pin as output
  pinMode(s3, OUTPUT);               //Set the digital pin as output


  Serial.begin(9600);               //Set the hardware serial port to 9600
  Serial3.begin(9600);            //Set the soft serial port to 9600

  //initial readings of tank levels
  int junk = analogRead(roftankpin);
  sroftank = (analogRead(roftankpin)*0.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(nfftankpin);
  snfftank = (analogRead(nfftankpin)*0.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(mfftankpin);
  smfftank = (analogRead(mfftankpin)*0.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(wwtankpin);
  swwtank = (analogRead(wwtankpin)*0.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(wastetankpin);
  swastetank = (analogRead(wastetankpin)*0.14351-28.702)*24*18*0.004329;
   junk = analogRead(ropotpin);
  rospot= analogRead(ropotpin);
  junk = analogRead(nfpotpin);
  spotnf= analogRead(nfpotpin);
}

//******     BEGIN FUNCTIONS     ******//

void open_channel(int i) {
  //This function controls what UART port is opened.
  port = i;
  //atoi(channel);
  //Convert the ASCII char value of the port to be opened into an int
  if (port < 1 || port > 8)port = 1;
  //If the value of the port is within range (1-8) then open that port. If it’s not in range set it port 1
  port -= 1;
  //So, this device knows its ports as 0-1 but we have them labeled 1-8 by subtracting one from the port to be opened we correct for this.
  digitalWrite(s1, bitRead(port, 0));
  //Here we have two commands combined into one.
  digitalWrite(s2, bitRead(port, 1));
  //The digitalWrite command sets a pin to 1/0 (high or low)
  digitalWrite(s3, bitRead(port, 2));
  //The bitRead command tells us what the bit value is for a specific bit location of a number
  delay(2); //required to make sure the channel switching event has completed
  return;
}

void timenow(){
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  DateTime now = rtc.now();
  timnow=now.unixtime();
  Serial.println(now.unixtime());
}

void bubbles(int cmd){
  if (cmd==0){
    digitalWrite(bubbler,LOW);
    bubblerstatus=0;
  }
  if (cmd==1){
    digitalWrite(bubbler,HIGH);
    bubblerstatus=1;
  }
}

void hppump(int cmd){
  if (cmd==0){
    digitalWrite(pump,LOW);
    pumpon=0;
  }
  if (cmd==1){
    delay(1000);  //delay to ensure all valves have completed moving
    digitalWrite(pump,HIGH);
    pumpon=1;
  }
}

void o3pump(int cmd){
  if (cmd==0){
    digitalWrite(bubbleblast,LOW);
    bubbleblaststatus=0;
  }
  if (cmd==1){
    digitalWrite(bubbleblast,HIGH);
    bubbleblaststatus=1;
  }
}

void o3(int cmd){
  if (cmd==0){
    digitalWrite(ozone,LOW);
    ozonestatus=0;
  }
  if (cmd==1){
    digitalWrite(ozone,HIGH);
    ozonestatus=1;
  }
}

void uvdisinfect(int cmd){
  if (cmd==0){
    digitalWrite(uv,LOW);
    uvstatus=0;
  }
  if (cmd==1){
    digitalWrite(uv,HIGH);
    uvstatus=1;
  }
}

void printrelays(){
  Serial.print("uv> ");Serial.print(uvstatus);
  Serial.print(" ozone> ");Serial.print(ozonestatus);
  Serial.print(" bubbleblaster> ");Serial.print(bubbleblaststatus);
  Serial.print(" bubbler> ");Serial.print(bubblerstatus);
  Serial.print(" pump> ");Serial.println(pumpon);
}

void valvecheck(){
  int clos=digitalRead(mfac);
  int ope=digitalRead(mfao);
  if (clos==1 && ope==0){
    mfastatus=0;
    } else if (clos==0 && ope ==1){
      mfastatus=1;
    } else {    mfastatus=2;
      }

  clos=digitalRead(mfbc);
  ope=digitalRead(mfbo);
  if (clos==1 && ope==0){
    mfbstatus=0;
    } else if (clos==0 && ope ==1){
      mfbstatus=1;
    } else {    mfbstatus=2;
      }

  clos=digitalRead(mfcc);
  ope=digitalRead(mfco);
  if (clos==1 && ope==0){
    mfcstatus=0;
    } else if (clos==0 && ope ==1){
    mfcstatus=1;
    } else {    mfcstatus=2;
      }

  clos=digitalRead(mfdc);
  ope=digitalRead(mfdo);
  if (clos==1 && ope==0){
    mfdstatus=0;
    } else if (clos==0 && ope ==1){
      mfdstatus=1;
    } else {mfdstatus=2;
      }

  clos=digitalRead(roac);
  ope=digitalRead(roao);
  if (clos==1 && ope==0){
    roastatus=0;
    } else if (clos==0 && ope ==1){
      roastatus=1;
    } else {roastatus=2;
      }

  clos=digitalRead(robc);
  ope=digitalRead(robo);
  if (clos==1 && ope==0){
    robstatus=0;
    } else if (clos==0 && ope ==1){
    robstatus=1;
    } else {robstatus=2;
      }

  clos=digitalRead(nfac);
  ope=digitalRead(nfao);
  if (clos==1 && ope==0){
    nfastatus=0;
    } else if (clos==0 && ope ==1){
      nfastatus=1;
    } else {nfastatus=2;
      }

  clos=digitalRead(nfbc);
  ope=digitalRead(nfbo);
  if (clos==1 && ope==0){
    nfbstatus=0;
    } else if (clos==0 && ope ==1){
      nfbstatus=1;
    } else {nfbstatus=2;
      }

  clos=digitalRead(wastec);
  ope=digitalRead(wasteo);
  if (clos==1 && ope==0){
    wastestatus=0;
    } else if (clos==0 && ope ==1){
      wastestatus=1;
    } else {wastestatus=2;
      }

  clos=digitalRead(wwrinsec);
  ope=digitalRead(wwrinseo);
  if (clos==1 && ope==0){
    wwrinsestatus=0;
    } else if (clos==0 && ope ==1){
    wwrinsestatus=1;
    } else {wwrinsestatus=2;
      }
}

void printvalves(){
  Serial.print("mfa> ");Serial.print(mfastatus);
  Serial.print(" mfb> ");Serial.print(mfbstatus);
  Serial.print(" mfc> ");Serial.print(mfcstatus);
  Serial.print(" mfd> ");Serial.println(mfdstatus);
  Serial.print(" nfa> ");Serial.print(nfastatus);
  Serial.print(" nfb> ");Serial.print(nfbstatus);
  Serial.print(" roa> ");Serial.print(roastatus);
  Serial.print(" rob> ");Serial.println(robstatus);
  Serial.print(" waste> ");Serial.print(wastestatus);
  Serial.print(" wwrinse> ");Serial.println(wwrinsestatus);
  }

void flows(){
  char strng[] ="1:R";

  //if (strng != 0) {
    for (int i = 1; i < 6; i++){//If a command has been sent

      channel = strtok(strng, ":");          //Let's parse the string at each colon
      cmd = "R";//strtok(NULL, ":");   //Let's parse the string at each colon
      open_channel(i);
    if (cmd != 0) {                               //If a command has been sent
      Serial3.print(cmd);                       //Send the command from the computer to the Atlas Scientific device using the softserial port
      Serial3.print("\r");                      //After we send the command we send a carriage return <CR>
    }
      delay(100);
    if (Serial3.available() > 0) {                 //If data has been transmitted from an Atlas Scientific device
    sensor_bytes_received = Serial3.readBytesUntil(13, sensordata, 30); //we read the data sent from the Atlas Scientific device until we see a <CR>. We also count how many character have been received
    sensordata[sensor_bytes_received] = 0;         //we add a 0 to the spot in the array just after the last character we received. This will stop us from transmitting incorrect data that may have been left in the buffer
    //Serial.println(sensordata);//Serial.println(sensordata);
    flotot = strtok(sensordata, ",");          //Let's parse the string at each colon
    floinst = strtok(NULL, "\n");   //Let's parse the string at each colon
    float f=atof(flotot);float fl=atof(floinst);
    flw[2*i-2]=f;flw[2*i-1]=fl;
    delay(10);
    if (Serial3.available() > 0) {                 //If data has been transmitted from an Atlas Scientific device
    int junk = Serial3.readBytesUntil(13, sensordatajunk, 30);}
    delay(10);
    }}
 }//}
/*
void serialEvent() {              //This interrupt will trigger when the data coming from the serial monitor(pc/mac/other) is received
  computer_bytes_received = Serial.readBytesUntil(13, computerdata, 20); //We read the data sent from the serial monitor(pc/mac/other) until we see a <CR>. We also count how many characters have been received
computerdata[computer_bytes_received] = 0; //We add a 0 to the spot in the array just after the last character we received.. This will stop us from transmitting incorrect data that may have been left in the buffer
}
*/

void pressures(){
  int junk = analogRead(feedppin);
  int feedp = analogRead(feedppin)*0.60753-121.507;
   junk = analogRead(nfrejectppin);
  int nfrejectp = analogRead(nfrejectppin)*0.60753-121.507;
   junk = analogRead(rorejectppin);
  int rorejectp = analogRead(rorejectppin)*0.60753-121.507;
   junk = analogRead(mfppin);
  int mfp = analogRead(mfppin)*0.24301-48.603;
  snfrejectp = (snfrejectp+nfrejectp)/2;// print out the value you read:
  sfeedp = (sfeedp+feedp)/2;
  srorejectp = (srorejectp+rorejectp)/2;
  smfp = (mfp+smfp)/2;
}

void valvepos(){
  int junk = analogRead(ropotpin);
  int ropot= analogRead(ropotpin);
  junk = analogRead(nfpotpin);
  int nfpot= analogRead(nfpotpin);
  spotnf = (spotnf*4+nfpot)/5;
  rospot = (rospot*4+ropot)/5;
}

void rovalvecloseupflow(int num){
  lcd.setCursor(0, 2);
  lcd.print("stepper closing  ");
  digitalWrite(rodir,HIGH);
  digitalWrite(rostpen, LOW);//make sure correct stepper

  for (int i=0; i<num; i++){
    digitalWrite(ros, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(25);              // wait for a second
    digitalWrite(ros, LOW);    // turn the LED off by making the voltage LOW
    delay(25);
  }

  digitalWrite(rostpen, HIGH);
  flows();
  valvepos();
}

void rovalveopenupflow(int num){
  lcd.setCursor(0, 2);
  lcd.print("stepper opening  ");
  digitalWrite(rodir,LOW);
  digitalWrite(rostpen, LOW);//make sure correct stepper

  for (int i=0; i<num; i++){
    digitalWrite(ros, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(25);              // wait for a second
    digitalWrite(ros, LOW);    // turn the LED off by making the voltage LOW
    delay(25);
  }

  digitalWrite(rostpen, HIGH);
  flows();
  valvepos();
}

void nfvalvecloseupflow(int num){
  lcd.setCursor(0, 2);
  lcd.print("stepper closing  ");
  digitalWrite(nfdir,HIGH);
  digitalWrite(nfstpen, LOW);//make sure correct stepper

  for (int i=0; i<num; i++){
    digitalWrite(nfs, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(25);              // wait for a second
    digitalWrite(nfs, LOW);    // turn the LED off by making the voltage LOW
    delay(25);
  }

  digitalWrite(nfstpen, HIGH);
  flows();
  valvepos();
}

void nfvalveopenupflow(int num){
  lcd.setCursor(0, 2);
  lcd.print("stepper opening  ");
  digitalWrite(nfdir,LOW);
  digitalWrite(nfstpen, LOW);//make sure correct stepper

  for (int i=0; i<num; i++){
    digitalWrite(nfs, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(25);              // wait for a second
    digitalWrite(nfs, LOW);    // turn the LED off by making the voltage LOW
    delay(25);
  }

  digitalWrite(nfstpen, HIGH);
  flows();
  valvepos();
}

void rocontrolopen(){
  lcd.setCursor(0, 2);
  lcd.print("stepper opening  ");valvepos();

  while (rospot<800){
    digitalWrite(rodir,LOW);
    digitalWrite(rostpen, LOW);//make sure correct stepper
    digitalWrite(ros, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(25);              // wait for a second
    digitalWrite(ros, LOW);    // turn the LED off by making the voltage LOW
    delay(25);
    valvepos();
  }

  digitalWrite(rostpen, HIGH);
}

void nfcontrolopen(){
  valvepos();

  while (spotnf<800){
    digitalWrite(nfdir,LOW);
    digitalWrite(nfstpen, LOW);//make sure correct stepper
    digitalWrite(nfs, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(25);              // wait for a second
    digitalWrite(nfs, LOW);    // turn the LED off by making the voltage LOW
    delay(25);
    valvepos();
  }

  digitalWrite(nfstpen, HIGH);
}

void tanklevel(){
  int junk = analogRead(roftankpin);
  float roftank = (analogRead(roftankpin)*0.177-35.714286)*36.5*24*0.004329;

  junk = analogRead(nfftankpin);
  float nfftank = (analogRead(nfftankpin)*0.177165-35.433)*36.5*24*0.004329;

  junk = analogRead(mfftankpin);
  float mfftank = (analogRead(mfftankpin)*0.155172-31.0345)*36.5*24*0.004329;

  junk = analogRead(wwtankpin);
  float wwtank = (analogRead(wwtankpin)*0.14351-28.702)*36.5*24*0.004329;

  junk = analogRead(wastetankpin);
  float wastetank = (analogRead(wastetankpin)*0.14351-28.702)*24*18*0.004329;

  smfftank = (mfftank+smfftank)/2;
  sroftank = (sroftank+roftank)/2; //empty at 201-203
  swwtank = (swwtank+wwtank)/2;
  snfftank = (snfftank+nfftank)/2;
  swastetank = (swastetank+wastetank)/2;
}

void printdata(){
  Serial.print("  mfftank> ");Serial.print(smfftank,0); //max at glass 347-348 top of lip at 345
  Serial.print("  nfftank> "); Serial.print(snfftank,0);//full at 327
  Serial.print("  roftank> ");Serial.print(sroftank,0); //full at 326
  Serial.print("  wwtank> "); Serial.print(swwtank,0);//full at 259ish
  Serial.print("  wastetank> ");Serial.println(swastetank,0);
  Serial.print(" nfpot position>  ");Serial.print(spotnf);
  Serial.print(" ropot position>  ");Serial.print(rospot);
  Serial.print("  feedpress> ");Serial.print(sfeedp);
  Serial.print("  MFpress> ");Serial.print(smfp);
  Serial.print("  NFpress> ");Serial.print(snfrejectp);
  Serial.print("  ROpress> ");Serial.println(srorejectp);
  Serial.print("flows1> "); Serial.print(flw[0]);Serial.print(" ");Serial.print(flw[1]);
  Serial.print("  flows2> "); Serial.print(flw[2]);Serial.print(" ");Serial.print(flw[3]);
  Serial.print("  flows3> "); Serial.print(flw[4]);Serial.print(" ");Serial.print(flw[5]);
  Serial.print("  flows4> "); Serial.print(flw[6]);Serial.print(" ");Serial.print(flw[7]);
  Serial.print("  flows5> "); Serial.print(flw[8]);Serial.print(" ");Serial.println(flw[9]);
  Serial.print(" Outside Temp: "); Serial.print(outtemp);
  Serial.print(" AC Temp: "); Serial.print(actemp);
  Serial.print(" DC Temp: "); Serial.println(dctemp);
  Serial.print("  blkpwr:  "); Serial.print(blkpwr);
  Serial.print("  redpwr:  "); Serial.println(redpwr);
}

void power(){
  int junk=analogRead(blkpowerpin);
  int blkread= analogRead(blkpowerpin);

  junk=analogRead(redpowerpin);
  int redread=analogRead(redpowerpin);

  float blkv=blkread*1.8/.36;
  float redv=redread*1.8/.36;
  float blki=blkv/.5;
  float redi=redv/.25;
  blkpwr= 120*blki/1000/60;//+blkpwr
  redpwr= 120*redi/1000/60;//+redpwr
}

void temperature(){
  //reads from 0x18 temperature sensor in DC panel
  if (!tempsensordc.begin(0x18)) {
    Serial.println("Couldn't find MCP9808 DC!");
    while (1);
  }

  tempsensordc.shutdown_wake(0);
  dctemp = tempsensordc.readTempC();

  delay(250);
  tempsensordc.shutdown_wake(1);

  //reads from 0x19 temperature sensor in AC panel
  if (!tempsensorac.begin(0x19)) {
    Serial.println("Couldn't find MCP9808 AC!");
    while (1);
  }

  tempsensorac.shutdown_wake(0);
  actemp = tempsensorac.readTempC();

  delay(250);
  tempsensorac.shutdown_wake(1);

  //reads from external temperature sensor
  if (! am2315.begin()) {
     Serial.println("Outside Sensor not found, check wiring & pullups!");
     while (1);
  }

  outtemp= am2315.readTemperature();
}

void waiting(int interval){
   t= millis();
  if (t-oldt > interval){
    oldt=t;
    tanklevel();
    flows();
    pressures();
    temperature();
    power();
    valvepos();
    printdata();
    valvecheck();
    printvalves();
    printrelays();
    timenow();
  }
}

void RO(int target, int rinsecycle){
  if (swwtank> target && sroftank< 5) {//if water needs to be treated
    return;}
    lcd.setCursor(0, 0);
    lcd.print("RO Treatment");
    checkvalve = false;
    uvdisinfect(1);
    unsigned long warmuptime =millis();
    lcd.setCursor(0, 3);
    lcd.print("UV Warmup");
    waiting(1);
    while (t-warmuptime< 30000){//900000){ //warmup uv 15 min
    waiting(5000);
  }

  if (mfastatus !=0) {
    return;}
  if (mfbstatus !=0) {
    return;}
  if (mfcstatus !=0) {
    return;}
  if (mfdstatus !=0) {
    return;}
  if (roastatus !=0) {
    return;}
  if (robstatus !=0) {
    return;}
  if (nfastatus !=0) {
    return;}
  if (nfbstatus !=0) {
    return;}
  if (wastestatus !=0) {
    return;}
  if (wwrinsestatus !=0) {
    return;}
  lcd.setCursor(0, 3);
  lcd.print("all valves closed");

  digitalWrite(roa, HIGH);
  while(checkvalve == false){ //wait for valves to turn
    valvecheck();
    if (roastatus ==1){
      checkvalve = true;
    }
  }

  checkvalve = false;
  digitalWrite(rob, HIGH);
  while(checkvalve == false){ //wait for valves to turn
    valvecheck();
    if (robstatus ==1){
      checkvalve = true;
    }
   }

  checkvalve = false;
  rocontrolopen();

  hppump(1);
  Serial.print("pumpon");
  pressures();

  while (swwtank< target && sroftank> 10){//(swwtank< 80 && sroftank> 5){
    waiting(10000);
    lcd.setCursor(0, 3);
    lcd.print("productflow: ");lcd.print(flw[7]);
    pressures();
    flows();

    if (sfeedp>240){
      hppump(0);
      return;
    }
    valvepos();

    if (rospot<100){
      return;
    }
    delay(1000);

    if (flw[7]>0.1 && flw[7]<=1.0 && rospot>100){
      rovalvecloseupflow(40);//close valve alot
      valvepos();
    }

    if (flw[7]>1.0 && flw[7]<1.35 && rospot>100){
      rovalvecloseupflow(20);
      valvepos();
    }

    if (flw[7]>1.35 && flw[7]<1.5 && rospot>100){//close valve a little bit
      rovalvecloseupflow(10);
      valvepos();
    }
    if (flw[7]>1.55 && flw[7]<=1.65 && rospot>100){
      rovalveopenupflow(10);//open valve a little bit
      valvepos();
    }
    if (flw[7]>1.65 && rospot>100){
      rovalveopenupflow(20);//open valve a lot
      valvepos();
    }
    if (flw[7]>=1.5 && flw[7]<=1.55){
      lcd.setCursor(0, 2);
      lcd.print("correct flow");
      valvepos();}
    }

   hppump(0);
   rocontrolopen();
     digitalWrite(roa, LOW);//tank
      while(checkvalve == false){ //wait for valves to turn
        valvecheck();
        if (roastatus ==0){
          checkvalve = true;
        }
      }
      checkvalve = false;
  if (rinsecycle==1){
        digitalWrite(wwrinse, HIGH);
      while(checkvalve == false){ //wait for valves to turn
        valvecheck();
        if (wwrinsestatus ==1){
          checkvalve = true;
        }
      }
      checkvalve = false;

  hppump(1);
  unsigned long rinsetime =millis();
  waiting(1);
  lcd.setCursor(0, 3);
  lcd.print("rinsing    ");
  while (t-rinsetime< 15000){ //rinse 15 sec
    waiting(3000);
    if (sfeedp>240){
    hppump(0);
    return;
    }
  }

  hppump(0);
  uvdisinfect(0);
  digitalWrite(wwrinse, LOW);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
  valvecheck();
  if (wwrinsestatus ==0){
    checkvalve = true;
    }
  }
  checkvalve = false;
}
  digitalWrite(rob, LOW);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (robstatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;

  waiting(1);
  systemstate=3;
  lcd.setCursor(0, 3);
  lcd.print("RO complete  ");
  }

  /*if (Serial.available() > 0) {
      incomingByte = Serial.read();
      if (incomingByte == 'H') {
        digitalWrite(b, HIGH);
      }
      if (incomingByte == 'L') {
        digitalWrite(b, LOW);
        }
  }*/

void NF(int target, int rinsecycle){
  if (sroftank> target && snfftank< 5) {//if water needs to be treated
    return;}

  lcd.setCursor(0, 0);
  lcd.print("NF Treatment");
  checkvalve = false;
  if (mfastatus !=0) {
    return;}
  if (mfbstatus !=0) {
    return;}
  if (mfcstatus !=0) {
    return;}
  if (mfdstatus !=0) {
    return;}
  if (roastatus !=0) {
    return;}
  if (robstatus !=0) {
    return;}
  if (nfastatus !=0) {
    return;}
  if (nfbstatus !=0) {
    return;}
  if (wastestatus !=0) {
    return;}
  if (wwrinsestatus !=0) {
    return;}

  Serial.print("all valves closed");
  digitalWrite(nfa, HIGH);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (nfastatus ==1){
      checkvalve = true;
    }
  }
  Serial.print("first valve open ");
  checkvalve = false;

  digitalWrite(nfb, HIGH);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (nfbstatus ==1){
      checkvalve = true;
    }
   }
   Serial.print("2 valve open ");
  checkvalve = false;

  nfcontrolopen();
  Serial.print("opened");
  hppump(1);
  Serial.print("pumpon");
  pressures();

  while (sroftank< target && snfftank> 10){//(swwtank< 80 && sroftank> 5){
    waiting(10000);
    lcd.setCursor(0, 3);
    lcd.print("productflow: ");lcd.print(flw[9]);
    pressures();
    flows();
    if (sfeedp>240){
      hppump(0);
      return;
      }
    valvepos();
    if (spotnf<100){
      return;
      }
      delay(1000);

    if (flw[9]>0.1 && flw[9]<=1.0 && spotnf>100){
      nfvalvecloseupflow(40);//close valve alot
      valvepos();
    }
    if (flw[9]>1.0 && flw[9]<1.35 && spotnf>100){
      nfvalvecloseupflow(20);
      valvepos();
    }
    if (flw[9]>1.35 && flw[9]<1.5 && spotnf>100){//close valve a little bit
      nfvalvecloseupflow(10);
      valvepos();
    }
    if (flw[9]>1.55 && flw[9]<=1.65 && spotnf>100){
      nfvalveopenupflow(10);//open valve a little bit
      valvepos();
    }
    if (flw[9]>1.65 && spotnf>100){
      nfvalveopenupflow(20);//open valve a lot
      valvepos();
    }
    if (flw[9]>=1.5 && flw[9]<=1.55){
      lcd.setCursor(0, 2);
      lcd.print("correct flow");
      valvepos();}
    }
  hppump(0);
  nfcontrolopen();
  digitalWrite(nfa, LOW);//tank
  while(checkvalve == false){
    valvecheck();
    if (nfastatus ==0){
    checkvalve = true;
    }
  }
  checkvalve = false;
  if (rinsecycle==1){
  digitalWrite(wwrinse, HIGH);
  while(checkvalve == false){
    valvecheck();
    if (wwrinsestatus ==1){
      checkvalve = true;
    }
  }
  checkvalve = false;

  hppump(1);
  lcd.setCursor(0, 3);
  lcd.print("rinsing    ");
  unsigned long rinsetime =millis();
  waiting(1);
  while (t-rinsetime< 15000){ //rinse 15 sec
    waiting(3000);
    if (sfeedp>240){
    hppump(0);
    return;
    }
  }

  hppump(0);
}//end rinse
  uvdisinfect(0);
  digitalWrite(wwrinse, LOW);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (wwrinsestatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;

  digitalWrite(nfb, LOW);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (nfbstatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;

  waiting(1);
  systemstate=2;
  lcd.setCursor(0, 3);
  lcd.print("NF complete  ");
}

void MF(int target, int rinsecycle){
  if (snfftank> target && smfftank< 5) {//if water needs to be treated
  return;}
  lcd.setCursor(0, 0);
  lcd.print("MF Treatment");
  checkvalve = false;
  if (mfastatus !=0) {
    return;}
  if (mfbstatus !=0) {
    return;}
  if (mfcstatus !=0) {
    return;}
  if (mfdstatus !=0) {
    return;}
  if (roastatus !=0) {
    return;}
  if (robstatus !=0) {
    return;}
  if (nfastatus !=0) {
    return;}
  if (nfbstatus !=0) {
    return;}
  if (wastestatus !=0) {
    return;}
  if (wwrinsestatus !=0) {
    return;}
  Serial.print("all valves closed");

  digitalWrite(mfa,HIGH);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (mfastatus ==1){
      checkvalve = true;
    }
  }
  checkvalve = false;

  digitalWrite(mfb,LOW);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (mfbstatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;

  digitalWrite(mfd,HIGH);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (mfdstatus ==1){
      checkvalve = true;
    }
  }
  checkvalve = false;

  digitalWrite(mfc,HIGH);
    while(checkvalve == false){ //wait for drain and vent valves to be closed
      valvecheck();
      if (mfcstatus ==1){
        checkvalve = true;
      }
    }
  checkvalve = false;
 delay(1000);
  hppump(1);
  Serial.print("pumpon");
  waiting(1);
  bw = millis();
  while (snfftank< target && smfftank> 10){//(swwtank< 80 && sroftank> 5){
    waiting(10000);
    float mfflows= 12.5-flw[5];
    lcd.setCursor(0, 3);
    lcd.print("productflow: ");lcd.print(mfflows);
    pressures();
    flows();
    tt=millis();
    delay(1000);
    if (tt-bw > 100000){ //every 2 min
      solenoid();
      waiting(5000);
      bw = tt;
      if (sfeedp>240){
        hppump(0);
        return;
      }
    }
  }

  hppump(0);

  //rinse
  checkvalve = false;
  digitalWrite(mfa, LOW);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (mfastatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;
  if (rinsecycle==1){
  digitalWrite(wwrinse, HIGH);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (wwrinsestatus ==1){
      checkvalve = true;
    }
  }
  checkvalve = false;

  unsigned long rinsetime =millis();
  hppump(1);
  lcd.setCursor(0, 3);
  lcd.print("rinsing    ");
  waiting(1);
  while (t-rinsetime < 6000){ //rinse 6 sec
    waiting(500);
    if (sfeedp>240){
    hppump(0);
    return;
    }
  }

  hppump(0);
  digitalWrite(wwrinse, LOW);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (wwrinsestatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;
}//endrinse
  digitalWrite(mfa, LOW);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
  valvecheck();
  if (mfastatus ==0){
    checkvalve = true;
  }
  }
  checkvalve = false;

  digitalWrite(mfb, LOW);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (mfbstatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;

  digitalWrite(mfc, LOW);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (mfcstatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;

  digitalWrite(mfd, LOW);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (mfdstatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;

  waiting(1);
  systemstate=1;
  lcd.setCursor(0, 3);
  lcd.print("MF complete  ");
}

void solenoid(){
  lcd.setCursor(0, 3);
  lcd.print("backpulse       ");
  digitalWrite(mfb,HIGH);
  digitalWrite(mfc,LOW);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (mfbstatus ==1){
      checkvalve = true;
    }
  }

  checkvalve = false;
    while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (mfcstatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;

  digitalWrite(sol,HIGH);
  delay(250);
  digitalWrite(sol,LOW);
  delay(250);
  digitalWrite(sol,HIGH);
  delay(250);
  digitalWrite(sol,LOW);
  delay(250);
  digitalWrite(sol,HIGH);
  delay(250);
  digitalWrite(sol,LOW);
  delay(250);
  digitalWrite(sol,HIGH);
  delay(250);
  digitalWrite(sol,LOW);
  delay(250);
  digitalWrite(mfb,LOW);
  digitalWrite(mfc,HIGH);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
  valvecheck();
    if (mfbstatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (mfcstatus ==1){
      checkvalve = true;
    }
  }
  checkvalve = false;

  lcd.setCursor(0, 3);
  lcd.print("backpulse done ");
}
void fixaverages(int number){
  for (int i=0; i<= number; i++){
  delay(10);
  waiting(1);}
  delay(3000);
}
//******     END FUNCTIONS     ******//
void regularday(){
  fixaverages(10);
  if (systemstate=0){
    treattimes[0]=timnow;
    RO(81,1);
    fixaverages(10);}
  if (systemstate =3){
    treattimes[1]=timnow;
    NF(81,1);
    fixaverages(10);}
  if (systemstate=2){
    treattimes[2]=timnow;
    MF(81,1);
    fixaverages(10);}
    treattimes[3]=timnow;
    lcd.setCursor(0, 0);
    lcd.print("Treatment complete  ");
    Serial.print("rostart time: "); Serial.println(treattimes[0]);
    Serial.print("nfstart time: "); Serial.println(treattimes[1]);
    Serial.print("mfstart time: "); Serial.println(treattimes[2]);
    Serial.print("end time: "); Serial.println(treattimes[3]);
    Serial.println("Water treated for the day");}
//******     BEGIN LOOP     ******//
void loop() {
  bubbles(1);
  waiting(1000);
  //RO(80,1);//target then 1 for rinse cycle (put 0 for filling sequence with no rinse)
  //NF(80,0);
  //MF(81,0);
  //flows();
  regularday();
  Serial.println("loop");
  while(1){};
}
//******     END LOOP     ******//
