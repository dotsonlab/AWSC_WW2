#include <Wire.h>
#include "RTClib.h"
#include "Adafruit_MCP9808.h"
#include <Adafruit_AM2315.h>
Adafruit_AM2315 am2315;
Adafruit_MCP9808 tempsensorac = Adafruit_MCP9808();
Adafruit_MCP9808 tempsensordc = Adafruit_MCP9808();
RTC_DS3231 rtc;
byte computer_bytes_received = 0;    //We need to know how many characters bytes have been received
byte sensor_bytes_received = 0;      //We need to know how many characters bytes have been received
int s1 = 16;                         //Arduino pin 5 to control pin S1
int s2 = 17;                         //Arduino pin 6 to control pin S2
int s3 = 18;                         //Arduino pin 7 to control pin S3
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
const int b=13;
int t=0;
int oldt =0;
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
//valves
const int sol=43;
const int mfa=27;
const int mfao=26;
const int mfac=25;
int mfastatus;
const int mfb=24;
const int mfbo=23;
const int mfbc=22;
int mfbstatus;
const int mfc=30;
const int mfco=29;
const int mfcc=28;
int mfcstatus;
const int mfd=36;
const int mfdo=35;
const int mfdc=34;
int mfdstatus;
const int roa=49;
const int roac=47;
const int roao=48;
int roastatus;
const int rob=33;
const int robc=31;
const int robo=32;
int robstatus;
const int nfa=39;
const int nfao=38;
const int nfac=37;
int nfastatus;
const int nfb=2;
const int nfbo=3;
const int nfbc=4;
int nfbstatus;
const int waste=42;
const int wastec=40;
const int wasteo=41;
int wastestatus;
const int wwrinse=46;
const int wwrinsec=44;
const int wwrinseo=45;
int wwrinsestatus;
const int rostpen=8;
const int nfstpen=11;
const int rodir = 10;
const int ros = 9;
const int nfs = 12;
const int nfdir = 13;
const int bubbleblast=53;
int bubbleblaststatus=0;
const int bubbler=51;
int bubblerstatus=0;
const int uv=52;
int uvstatus=0;
const int ozone=50;
int ozonestatus=0;
const int pump=7;
int pumpon=0;

void setup() {
  pinMode(bubbleblast, OUTPUT);
  pinMode(bubbler, OUTPUT);
  pinMode(uv, OUTPUT);
  pinMode(ozone, OUTPUT);
  pinMode(sol, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(mfao, INPUT);
  pinMode(mfac, INPUT);
  pinMode(mfa, OUTPUT);
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
  pinMode(nfdir, OUTPUT);
  pinMode(nfs, OUTPUT);
  pinMode(nfstpen, OUTPUT);
  pinMode(rodir, OUTPUT);
  pinMode(ros, OUTPUT);
  pinMode(rostpen, OUTPUT);
  pinMode(s1, OUTPUT);              //Set the digital pin as output
  pinMode(s2, OUTPUT);              //Set the digital pin as output
  pinMode(s3, OUTPUT);               //Set the digital pin as output
  pinMode(b, OUTPUT); ///////////////
  digitalWrite(rostpen, HIGH);
  digitalWrite(nfstpen, HIGH);
  Serial.begin(9600);               //Set the hardware serial port to 9600
  Serial3.begin(9600);            //Set the soft serial port to 9600
  int junk = analogRead(2);  
  sroftank = (analogRead(2)*.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(5);
  snfftank = (analogRead(5)*.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(4);
  smfftank = (analogRead(4)*.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(6);
  swwtank = (analogRead(6)*.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(3);
  swastetank = (analogRead(3)*.14351-28.702)*24*18*0.004329;
}
void open_channel(int i) {                             //This function controls what UART port is opened.
  port = i;//atoi(channel);                           //Convert the ASCII char value of the port to be opened into an int    
  if (port < 1 || port > 8)port = 1;              //If the value of the port is within range (1-8) then open that port. If it’s not in range set it port 1  
  port -= 1;                                      //So, this device knows its ports as 0-1 but we have them labeled 1-8 by subtracting one from the port to be opened we correct for this.    
  digitalWrite(s1, bitRead(port, 0));             //Here we have two commands combined into one.
  digitalWrite(s2, bitRead(port, 1));             //The digitalWrite command sets a pin to 1/0 (high or low)
  digitalWrite(s3, bitRead(port, 2));             //The bitRead command tells us what the bit value is for a specific bit location of a number 
  delay(2);                                       //this is needed to make sure the channel switching event has completed
  return;                                         //go back
}
void timenow(){
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  DateTime now = rtc.now();
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
    mfastatus=0;}
  else if (clos==0 && ope ==1){
    mfastatus=1;}
  else {    mfastatus=2;}
  clos=digitalRead(mfbc);
  ope=digitalRead(mfbo);
  if (clos==1 && ope==0){
    mfbstatus=0;}
  else if (clos==0 && ope ==1){
    mfbstatus=1;}
  else {    mfbstatus=2;  }
  clos=digitalRead(mfcc);
  ope=digitalRead(mfco);
  if (clos==1 && ope==0){
    mfcstatus=0;}
  else if (clos==0 && ope ==1){
    mfcstatus=1;}
  else {    mfcstatus=2;  }
  clos=digitalRead(mfdc);
  ope=digitalRead(mfdo);
  if (clos==1 && ope==0){
    mfdstatus=0;}
  else if (clos==0 && ope ==1){
    mfdstatus=1;}
  else {mfdstatus=2; }
  clos=digitalRead(roac);
  ope=digitalRead(roao);
  if (clos==1 && ope==0){
    roastatus=0;}
  else if (clos==0 && ope ==1){
    roastatus=1;}
  else {roastatus=2; }
  clos=digitalRead(robc);
  ope=digitalRead(robo);
  if (clos==1 && ope==0){
    robstatus=0;}
  else if (clos==0 && ope ==1){
    robstatus=1;}
  else {robstatus=2; }
  clos=digitalRead(nfac);
  ope=digitalRead(nfao);
  if (clos==1 && ope==0){
    nfastatus=0;}
  else if (clos==0 && ope ==1){
    nfastatus=1;}
  else {nfastatus=2; }
  clos=digitalRead(nfbc);
  ope=digitalRead(nfbo);
  if (clos==1 && ope==0){
    nfbstatus=0;}
  else if (clos==0 && ope ==1){
    nfbstatus=1;}
  else {nfbstatus=2; }
  clos=digitalRead(wastec);
  ope=digitalRead(wasteo);
  if (clos==1 && ope==0){
    wastestatus=0;}
  else if (clos==0 && ope ==1){
    wastestatus=1;}
  else {wastestatus=2; }
  clos=digitalRead(wwrinsec);
  ope=digitalRead(wwrinseo);
  if (clos==1 && ope==0){
    wwrinsestatus=0;}
  else if (clos==0 && ope ==1){
    wwrinsestatus=1;}
  else {wwrinsestatus=2; }
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
    
    Serial.print("port ");Serial.print(i); 
    Serial.print(" inst ");Serial.print(floinst);
    Serial.print(" tot ");Serial.println(flotot);
    }}
 }//}
/*
void serialEvent() {              //This interrupt will trigger when the data coming from the serial monitor(pc/mac/other) is received
  computer_bytes_received = Serial.readBytesUntil(13, computerdata, 20); //We read the data sent from the serial monitor(pc/mac/other) until we see a <CR>. We also count how many characters have been received
computerdata[computer_bytes_received] = 0; //We add a 0 to the spot in the array just after the last character we received.. This will stop us from transmitting incorrect data that may have been left in the buffer
}
*/
void pressures(){
  int junk = analogRead(9);
  int feedp = analogRead(9)*.60753-121.507;
   junk = analogRead(11);
  int nfrejectp = analogRead(11)*.60753-121.507;
   junk = analogRead(8);
  int rorejectp = analogRead(8)*.60753-121.507;
   junk = analogRead(7);
  int mfp = analogRead(7)*.24301-48.603;
  snfrejectp = (snfrejectp*9+nfrejectp)/10;// print out the value you read:
  sfeedp = (sfeedp*9+feedp)/10;
  srorejectp = (srorejectp*9+rorejectp)/10;// print out the value you read:
  smfp = (mfp+smfp*9)/10;// print out the value you read:
}  

void valvepos(){
  int junk = analogRead(1);
  int ropot= analogRead(1);
  junk = analogRead(0);
  int nfpot= analogRead(0);
  spotnf = (spotnf*9+nfpot)/10;
  rospot = (rospot*9+ropot)/10;     
}

void rovalvecloseupflow(int num){
  digitalWrite(rodir,HIGH);
        digitalWrite(rostpen, LOW);//make sure correct stepper
        for (int i=0; i=num; i++){
        digitalWrite(ros, HIGH);  // turn the LED on (HIGH is the voltage level)
        delay(5);              // wait for a second
        digitalWrite(ros, LOW);    // turn the LED off by making the voltage LOW
        delay(5);
        }
        digitalWrite(rostpen, HIGH);
}
void rovalveopenupflow(int num){
  digitalWrite(rodir,LOW);
        digitalWrite(rostpen, LOW);//make sure correct stepper
        for (int i=0; i=num; i++){
          digitalWrite(ros, HIGH);  // turn the LED on (HIGH is the voltage level)
        delay(5);              // wait for a second
        digitalWrite(ros, LOW);    // turn the LED off by making the voltage LOW
        delay(5);
        }
        digitalWrite(rostpen, HIGH);
}
void nfvalvecloseupflow(int num){
  digitalWrite(nfdir,HIGH);
        digitalWrite(nfstpen, LOW);//make sure correct stepper
        for (int i=0; i=num; i++){
        digitalWrite(nfs, HIGH);  // turn the LED on (HIGH is the voltage level)
        delay(5);              // wait for a second
        digitalWrite(nfs, LOW);    // turn the LED off by making the voltage LOW
        delay(5);
}
        digitalWrite(nfstpen, HIGH);
}
void nfvalveopenupflow(int num){
  digitalWrite(nfdir,LOW);
        digitalWrite(nfstpen, LOW);//make sure correct stepper
        for (int i=0; i=num; i++){
        digitalWrite(nfs, HIGH);  // turn the LED on (HIGH is the voltage level)
        delay(5);              // wait for a second
        digitalWrite(nfs, LOW);    // turn the LED off by making the voltage LOW
        delay(5);
}
        digitalWrite(nfstpen, HIGH);
}

void rocontrolopen(){
  valvepos();
  while (rospot<800){
        digitalWrite(rodir,LOW);
        digitalWrite(rostpen, LOW);//make sure correct stepper
        digitalWrite(ros, HIGH);  // turn the LED on (HIGH is the voltage level)
        delay(5);              // wait for a second
        digitalWrite(ros, LOW);    // turn the LED off by making the voltage LOW
        delay(5);
valvepos();
}
digitalWrite(rostpen, HIGH);
}
void nfcontrolopen(){
  valvepos();
  while (rospot<800){
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
  int junk = analogRead(2);  
  float roftank = (analogRead(2)*.177-35.714286)*36.5*24*0.004329;
   junk = analogRead(5);
  float nfftank = (analogRead(5)*.177165-35.433)*36.5*24*0.004329;
   junk = analogRead(4);
  float mfftank = (analogRead(4)*.155172-31.0345)*36.5*24*0.004329;
   junk = analogRead(6);
  float wwtank = (analogRead(6)*.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(3);
  float wastetank = (analogRead(3)*.14351-28.702)*24*18*0.004329;
  smfftank = (mfftank+smfftank*9)/10;
  sroftank = (sroftank*9+roftank)/10; //empty at 201-203
  swwtank = (swwtank*9+wwtank)/10;
  snfftank = (snfftank*9+nfftank)/10;
  swastetank = (swastetank*9+wastetank)/10;
}

void printdata(){
  Serial.print("  mfftank> ");Serial.print(smfftank); //max at glass 347-348 top of lip at 345
  Serial.print("  nfftank> "); Serial.print(snfftank);//full at 327
  Serial.print("  roftank> ");Serial.print(sroftank); //full at 326
  Serial.print("  wwtank> "); Serial.print(swwtank);//full at 259ish
  Serial.print("  wastetank> ");Serial.println(swastetank);
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
}

void power(){
  int junk=analogRead(12);
  int blkread= analogRead(12);
  junk=analogRead(13);
  int redread=analogRead(13);
  int blkv=blkread*1.8/.36;
  int redv=redread*1.8/.36;
  int blki=blkv/.5;
  int redi=redv/.25;
  blkpwr= 120*blki/1000/60;//+blkpwr
  redpwr= 120*redi/1000/60;//+redpwr
}

void temperature(){//doesnt work, only reads one value
  if (!tempsensordc.begin(0x18)) {
    Serial.println("Couldn't find MCP9808 DC!");
    while (1);
  }
  tempsensordc.shutdown_wake(0);
  dctemp = tempsensordc.readTempC();
  delay(250);
  tempsensordc.shutdown_wake(1);
  if (!tempsensorac.begin(0x19)) {
    Serial.println("Couldn't find MCP9808 AC!");
    while (1);
  }
  tempsensorac.shutdown_wake(0);
  actemp = tempsensorac.readTempC();
  delay(250);
  tempsensorac.shutdown_wake(1);
  if (! am2315.begin()) {
     Serial.println("Outside Sensor not found, check wiring & pullups!");
     while (1);
  }
  outtemp= am2315.readTemperature();
}

void loop() {
//waiting(1000);
//RO();
//flows();
waiting(1);
//delay(3000);
Serial.println("loop");
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
void solenoid(){
      digitalWrite(mfb,HIGH);
      digitalWrite(mfc,LOW);
      while(checkvalve == false){ //wait for drain and vent valves to be closed
      valvecheck();
      if (mfbstatus ==1){
        checkvalve = true;
      }
    }checkvalve = false; 
      while(checkvalve == false){ //wait for drain and vent valves to be closed
      valvecheck();
      if (mfcstatus ==0){
        checkvalve = true;
      }
    }checkvalve = false; 
      digitalWrite(sol,HIGH);
      delay(1000);
      digitalWrite(sol,LOW);
      delay(1000);
      digitalWrite(mfb,LOW);
      digitalWrite(mfc,HIGH);  
      while(checkvalve == false){ //wait for drain and vent valves to be closed
      valvecheck();
      if (mfbstatus ==0){
        checkvalve = true;
      }
    }   checkvalve = false;    
    while(checkvalve == false){ //wait for drain and vent valves to be closed
      valvecheck();
      if (mfcstatus ==1){
        checkvalve = true;
      }
    } checkvalve = false; 
      }
void RO(){
if (swwtank> 80 && sroftank< 5) {//if water needs to be treated
  return;}
checkvalve = false;
uvdisinfect(1);
int warmuptime =millis(); Serial.println("uvwarmup");
while (t-warmuptime< 30000){//900000){ //warmup uv 15 min
 waiting(10000);

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
  Serial.print("all valves closed"); 
  digitalWrite(roa, HIGH);
    while(checkvalve == false){ //wait for drain and vent valves to be closed
      valvecheck();
      if (roastatus ==1){
        checkvalve = true;
      }
    
    }Serial.print("first valve open ");
    checkvalve = false; 
    digitalWrite(rob, HIGH);
    while(checkvalve == false){ //wait for drain and vent valves to be closed
      valvecheck();
      if (robstatus ==1){
        checkvalve = true;
      }
     }Serial.print("2 valve open ");
    checkvalve = false; 
  rocontrolopen();
  Serial.print("opened");
 hppump(1);
 Serial.print("pumpon");
 pressures();
 
 while (swwtank< 80 && sroftank> 5){
  waiting(15000);
  if (sfeedp>240){
    hppump(0);
    return;
    }
 if (flw[7]==0){waiting(100);Serial.print("treating with no flow");} 
  else if (0.1<flw[7]<=1){
  rovalvecloseupflow(40);//close valve alot
 }
 else if (1<flw[7]<1.35){
  rovalvecloseupflow(20);
 }
 else if (1.35<=flw[7]<1.45){//close valve a little bit
  rovalvecloseupflow(1);
 }
 else if (1.55<flw[7]<=1.65){
  rovalveopenupflow(1);//open valve a little bit
 }
 else if (flw[7]>1.65){
  rovalveopenupflow(20);//open valve a lot
 }
 else {waiting(15000);}
 }
 
 hppump(0);
 rocontrolopen();
   digitalWrite(roa, LOW);
    while(checkvalve == false){ //wait for drain and vent valves to be closed
      valvecheck();
      if (roastatus ==0){
        checkvalve = true;
      }
    }
    checkvalve = false; 
      digitalWrite(wwrinse, HIGH);
    while(checkvalve == false){ //wait for drain and vent valves to be closed
      valvecheck();
      if (wwrinsestatus ==1){
        checkvalve = true;
      }
    }
    checkvalve = false; 
hppump(1);
waiting(1);
int rinsetime =millis();
while (t-rinsetime< 15000){ //rinse 15 sec
 waiting(5000);
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
          digitalWrite(rob, LOW);
    while(checkvalve == false){ //wait for drain and vent valves to be closed
      valvecheck();
      if (robstatus ==0){
        checkvalve = true;
      }
    }
    checkvalve = false; 
    waiting(1);
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

