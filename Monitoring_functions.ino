#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include <Adafruit_AM2315.h>
Adafruit_AM2315 am2315;
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
byte computer_bytes_received = 0;    //We need to know how many characters bytes have been received
byte sensor_bytes_received = 0;      //We need to know how many characters bytes have been received
int s1 = 16;                         //Arduino pin 5 to control pin S1
int s2 = 17;                         //Arduino pin 6 to control pin S2
int s3 = 18;                         //Arduino pin 7 to control pin S3
int port = 0;                       //what port to open
char strng[20];               //A 20 byte character array to hold incoming data from a pc/mac/other
char sensordata[30];                 //A 30 byte character array to hold incoming data from the sensors
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
int sroftank = 0;
int snfftank = 0;
int spotnf= 0;
int srorejectp = 0;
int swwtank = 0;
int rospot= 0;
int smfp = 0;
int smfftank= 0;
int swastetank = 0;

//valves
const int mfa=27;
const int mfao=26;
const int mfac=25;
char mfastatus[4];
const int mfb=24;
const int mfbo=23;
const int mfbc=22;
char mfbstatus[4];
const int mfc=30;
const int mfco=29;
const int mfcc=28;
char mfcstatus[4];
const int mfd=36;
const int mfdo=35;
const int mfdc=34;
char mfdstatus[4];
const int roa=49;
const int roac=47;
const int roao=48;
char roastatus[4];
const int rob=33;
const int robc=31;
const int robo=32;
char robstatus[4];
const int nfa=39;
const int nfao=38;
const int nfac=37;
char nfastatus[4];
const int nfb=2;
const int nfbo=3;
const int nfbc=4;
char nfbstatus[4];
const int waste=42;
const int wastec=40;
const int wasteo=41;
char wastestatus[4];
const int wwrinse=46;
const int wwrinsec=44;
const int wwrinseo=45;
char wwrinsestatus[4];

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
  
  pinMode(s1, OUTPUT);              //Set the digital pin as output
  pinMode(s2, OUTPUT);              //Set the digital pin as output
  pinMode(s3, OUTPUT);               //Set the digital pin as output
  pinMode(b, OUTPUT); ///////////////
  
  Serial.begin(9600);               //Set the hardware serial port to 9600
  Serial3.begin(9600);            //Set the soft serial port to 9600
}
void open_channel() {                             //This function controls what UART port is opened.
  port = atoi(channel);                           //Convert the ASCII char value of the port to be opened into an int    
  if (port < 1 || port > 8)port = 1;              //If the value of the port is within range (1-8) then open that port. If it’s not in range set it port 1  
  port -= 1;                                      //So, this device knows its ports as 0-1 but we have them labeled 1-8 by subtracting one from the port to be opened we correct for this.    
  digitalWrite(s1, bitRead(port, 0));             //Here we have two commands combined into one.
  digitalWrite(s2, bitRead(port, 1));             //The digitalWrite command sets a pin to 1/0 (high or low)
  digitalWrite(s3, bitRead(port, 2));             //The bitRead command tells us what the bit value is for a specific bit location of a number 
  delay(2);                                       //this is needed to make sure the channel switching event has completed
  return;                                         //go back
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
void uvdinisfect(int cmd){
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
    mfastatus=="clos";}
  else if (clos==0 && ope ==1){
    mfastatus=="open";}
  else {    mfastatus=="move";}
  clos=digitalRead(mfbc);
  ope=digitalRead(mfbo);
  if (clos==1 && ope==0){
    mfbstatus=="clos";}
  else if (clos==0 && ope ==1){
    mfbstatus=="open";}
  else {    mfbstatus=="move";  }
  clos=digitalRead(mfcc);
  ope=digitalRead(mfco);
  if (clos==1 && ope==0){
    mfcstatus=="clos";}
  else if (clos==0 && ope ==1){
    mfcstatus=="open";}
  else {    mfcstatus=="move";  }
  clos=digitalRead(mfdc);
  ope=digitalRead(mfdo);
  if (clos==1 && ope==0){
    mfdstatus=="clos";}
  else if (clos==0 && ope ==1){
    mfdstatus=="open";}
  else {mfdstatus=="move"; }
  clos=digitalRead(roac);
  ope=digitalRead(roao);
  if (clos==1 && ope==0){
    roastatus=="clos";}
  else if (clos==0 && ope ==1){
    roastatus=="open";}
  else {roastatus=="move"; }
  clos=digitalRead(robc);
  ope=digitalRead(robo);
  if (clos==1 && ope==0){
    robstatus=="clos";}
  else if (clos==0 && ope ==1){
    robstatus=="open";}
  else {robstatus=="move"; }
  clos=digitalRead(nfac);
  ope=digitalRead(nfao);
  if (clos==1 && ope==0){
    nfastatus=="clos";}
  else if (clos==0 && ope ==1){
    nfastatus=="open";}
  else {nfastatus=="move"; }
  clos=digitalRead(nfbc);
  ope=digitalRead(nfbo);
  if (clos==1 && ope==0){
    nfbstatus=="clos";}
  else if (clos==0 && ope ==1){
    nfbstatus=="open";}
  else {nfbstatus=="move"; }
  clos=digitalRead(wastec);
  ope=digitalRead(wasteo);
  if (clos==1 && ope==0){
    wastestatus=="clos";}
  else if (clos==0 && ope ==1){
    wastestatus=="open";}
  else {wastestatus=="move"; }
  clos=digitalRead(wwrinsec);
  ope=digitalRead(wwrinseo);
  if (clos==1 && ope==0){
    wwrinsestatus=="clos";}
  else if (clos==0 && ope ==1){
    wwrinsestatus=="open";}
  else {wwrinsestatus=="move"; }
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
  if (strng != 0) { 
    for (int i = 1; i < 6; i++){//If a command has been sent
      strng[0]= i;
      channel = strtok(strng, ":");          //Let's parse the string at each colon
      cmd = strtok(NULL, ":");   //Let's parse the string at each colon
      open_channel();  
    if (cmd != 0) {                               //If a command has been sent
      Serial3.print(cmd);                       //Send the command from the computer to the Atlas Scientific device using the softserial port
      Serial3.print("\r");                      //After we send the command we send a carriage return <CR>
    }
      delay(100);
    if (Serial3.available() > 0) {                 //If data has been transmitted from an Atlas Scientific device
    sensor_bytes_received = Serial3.readBytesUntil(13, sensordata, 30); //we read the data sent from the Atlas Scientific device until we see a <CR>. We also count how many character have been received
    sensordata[sensor_bytes_received] = 0;         //we add a 0 to the spot in the array just after the last character we received. This will stop us from transmitting incorrect data that may have been left in the buffer
    flotot = strtok(sensordata, ",");          //Let's parse the string at each colon
    floinst = strtok(NULL, "\n");   //Let's parse the string at each colon
    //Serial.print("instant flow> ");Serial.print(floinst);
    //Serial.print("   totalflow> ");Serial.println(flotot);
    float f=atof(flotot);float fl=atof(floinst);
    flw[2*i-2]=f;flw[2*i-1]=fl;
  }}strng=="";
  //Serial.print(flw); cannot serial print a float array
 }}
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

void tanklevel(){
  int junk = analogRead(2);  
  int roftank = (analogRead(2)*.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(5);
  int nfftank = (analogRead(5)*.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(4);
  int mfftank = (analogRead(4)*.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(6);
  int wwtank = (analogRead(6)*.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(3);
  int wastetank = (analogRead(3)*.14351-28.702)*24*18*0.004329;
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
  Serial.print("  wwtank> "); Serial.println(swwtank);//full at 259ish
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
  Serial.print("DC Temp: "); Serial.println(dctemp);
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

void temperature(){
  if (!tempsensor.begin(0x19)) {
    Serial.println("Couldn't find MCP9808 DC!");
    while (1);
  }
  tempsensor.shutdown_wake(0);
  dctemp = tempsensor.readTempC();
  tempsensor.shutdown_wake(1);
  if (!tempsensor.begin(0x18)) {
    Serial.println("Couldn't find MCP9808 AC!");
    while (1);
  }
  tempsensor.shutdown_wake(0);
  actemp = tempsensor.readTempC();
  tempsensor.shutdown_wake(1);
  if (! am2315.begin()) {
     Serial.println("Outside Sensor not found, check wiring & pullups!");
     while (1);
  }
  outtemp= am2315.readTemperature();
}

void loop() {
  t= millis();
  delay(10);
  if (t-oldt > 5000){
    strng =="1:R";
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
  }
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


