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
//not working?? Adafruit_MCP9808 tempsensordc = Adafruit_MCP9808();
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
float dctemp=0;
float actemp;
float outtemp;
float blkpwr;
float redpwr;
String incomingByte;
unsigned long t=0;
unsigned long o3time=0;
unsigned long o3starttime=0;
unsigned long oldt =0;
unsigned long tt=0;
unsigned long bw =0;
unsigned long uvontime=0;
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
float sEQtank = 0;
float sSettletank = 0;
float ssbrtank= 0;
float swastetank = 0;
boolean checkvalve = false;
int systemstate=1;
unsigned long timnow;
unsigned long treattimes[3];
//initial 12v actuated ball valve status
int pretankstatus;
int prefiltstatus;
int roastatus;
int robstatus;
int nfastatus;
int nfbstatus;
int wastestatus;
int wwrinsestatus;
int sendbackstatus;
int EQ2Settlestatus;
int Settle2SBRstatus;
int maxwaste = 50;
int sbrairstatus=0;
float sbrairontime =0;
float sbrairofftime =0;
float sbraircyclestarttime=0;
//initial 120v relay status
int bubbleblaststatus=0;
int bubblerstatus=0;
int uvstatus=0;
int ozonestatus=0;
int pumpon=0;
int xx=1;
//END ---- defined variables

//BEGIN ---- Pin definitions

const int s1 = 16;                         //pin 16 to control pin S1
const int s2 = 17;                         //pin 17 to control pin S2
const int s3 = 18;                         //pin 18 to control pin S3

//120v relays
const int bubbleblast=53;
const int bubbler=51;//always on
const int uv=52;
const int ozone=50;
const int pump=7;

//12v actuated ball valves
//const int pretankvalve=27;
const int Settle2SBR=29;  //this os Settle2SBR
const int EQ2Settle=28;  // this is EQ2Settle

/* Valves removed with removal of Ceramic MF

const int mfb=24;//lower opening
const int mfbo=23;
const int mfbc=22;
*/
const int sendback=36;//upper opening
const int sendbacko=35;
const int sendbackc=34;


//const int prefiltvalve=30;
const int SBRDecant=26;//SBRDecant
const int SBRAir=25;//SBRAir

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

//ANALOG Sensor PINS
const int feedppin=9;
const int nfrejectppin=11;
const int rorejectppin=8;
const int settletankpin=7;
const int EQtankpin=14;

const int ropotpin=1;
const int nfpotpin=0;

const int roftankpin=2;
const int nfftankpin=5;
const int sbrtankpin=4;
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
  pinMode(sbrtankpin,INPUT);
  pinMode(wwtankpin,INPUT);
  pinMode(wastetankpin,INPUT);
  pinMode(ropotpin,INPUT);
  pinMode(nfpotpin,INPUT);
  pinMode(feedppin,INPUT);
  pinMode(nfrejectppin,INPUT);
  pinMode(rorejectppin,INPUT);
  pinMode(settletankpin,INPUT);
  pinMode(EQtankpin,INPUT);

  //120v relays
  pinMode(bubbleblast, OUTPUT);
  pinMode(bubbler, OUTPUT);//alwayson
  digitalWrite(bubbler, HIGH);
  pinMode(uv, OUTPUT);
  pinMode(ozone, OUTPUT);
  pinMode(pump, OUTPUT);

  //12v actuated ball valves
  //pinMode(pretankvalve, OUTPUT);
  pinMode(EQ2Settle, OUTPUT);
  pinMode(Settle2SBR, OUTPUT);

  //pinMode(prefiltvalve, OUTPUT);
  pinMode(SBRAir, OUTPUT);
  pinMode(SBRDecant, OUTPUT);

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

  pinMode(sendback, OUTPUT);
  pinMode(sendbacko, INPUT);
  pinMode(sendbackc, INPUT);

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
  o3starttime =t;//setting ozone timer
  //initial readings of tank levels
  int junk = analogRead(roftankpin);
  sroftank = (analogRead(roftankpin)*0.14351-28.702)*36.5*24*0.004329;//turn analog reading into gallons
   junk = analogRead(nfftankpin);
  snfftank = (analogRead(nfftankpin)*0.14351-28.702)*36.5*24*0.004329;
   junk = analogRead(sbrtankpin);
  ssbrtank = analogRead(sbrtankpin)*0.668388-146.32479;
   junk = analogRead(wwtankpin);
  swwtank = analogRead(wwtankpin)*0.65375348-134.42675;
   junk = analogRead(wastetankpin);
  swastetank = (analogRead(wastetankpin)*0.14351-28.702)*24*18*0.004329;
  junk = analogRead(EQtankpin);
  sroftank = (analogRead(EQtankpin)*0.14351-28.702)*36.5*24*0.004329;//turn analog reading into gallons
  junk = analogRead(settletankpin);
  sroftank = (analogRead(settletankpin)*0.14351-28.702)*36.5*24*0.004329;//turn analog reading into gallons
  junk = analogRead(ropotpin);
  rospot= analogRead(ropotpin);
  junk = analogRead(nfpotpin);
  spotnf= analogRead(nfpotpin);
  digitalWrite(pump,LOW); //ensure pin is not floating
  digitalWrite(bubbler,HIGH); //always on for outlet
  delay(5000);
  Serial.println("ready");//waiting(1);
}

//******     BEGIN FUNCTIONS     ******//

void open_channel(int i) {//This function controls what UART port is opened.
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
void timenow(){//gets current time for data in unixtime
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  DateTime now = rtc.now();
  timnow=now.unixtime();
  //Serial.println(now.unixtime());
}
void hppump(int cmd){//high pressure pump on/off
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
void EQ2Sett(int cmd){// pump on off
  if (cmd==0){
    digitalWrite(EQ2Settle,LOW);
    EQ2Settlestatus=0;
  }
  if (cmd==1){
    digitalWrite(EQ2Settle,HIGH);
    EQ2Settlestatus=1;
  }
}
void Sett2SBR(int cmd){// pump on off
  if (cmd==0){
    digitalWrite(Settle2SBR,LOW);
    Settle2SBRstatus=0;
  }
  if (cmd==1){
    digitalWrite(Settle2SBR,HIGH);
    Settle2SBRstatus=1;
  }
}
void SBRAironoff(int cmd){
    if (cmd==0){
      if (sbrairstatus==1){
        sbrairofftime=millis();}
      digitalWrite(SBRAir,LOW);
      sbrairstatus=0;
    }
    if (cmd==1){
      if (sbrairstatus ==0){
      sbrairontime= millis();}
      digitalWrite(SBRAir,HIGH);
      sbrairstatus=1;
    }
  }
void o3pump(int cmd){//ozone pump on off
  if (cmd==0){
    digitalWrite(bubbleblast,LOW);
    bubbleblaststatus=0;
  }
  if (cmd==1){
    digitalWrite(bubbleblast,HIGH);
    bubbleblaststatus=1;
  }
}
void o3(int cmd){//ozone generator on/off
  if (cmd==0){
    digitalWrite(ozone,LOW);
    ozonestatus=0;
  }
  if (cmd==1){
    digitalWrite(ozone,HIGH);
    ozonestatus=1;
  }
}
void uvdisinfect(int cmd){//uv disinfection on/off

  if (cmd==0){
    uvontime=0;
    digitalWrite(uv,LOW);
    uvstatus=0;

  }
  if (cmd==1){
    if (uvstatus ==0){
      uvontime=millis();
    }
    digitalWrite(uv,HIGH);
    uvstatus=1;
  }
}
void valvecheck(){//checks all valve limit switches, assigns status either moving, open or closed
  int clos=digitalRead(roac);
  int ope=digitalRead(roao);
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
  clos=digitalRead(sendbackc);
  ope=digitalRead(sendbacko);
  if (clos==1 && ope==0){
    sendbackstatus=0;
    } else if (clos==0 && ope ==1){
    sendbackstatus=1;
    } else {sendbackstatus=2;
      }
}
void closeallvalves(){//closes all valves, chekc they are closed. used before treatment sequence
  digitalWrite(roa, LOW);
  digitalWrite(rob, LOW);
  digitalWrite(nfa, LOW);
  digitalWrite(nfb, LOW);
  digitalWrite(waste, LOW);
  digitalWrite(wwrinse,LOW);
  digitalWrite(sendback,LOW);
  checkvalve = false;
  while(checkvalve == false){ //wait for valves to turn
    valvecheck();
    if (sendbackstatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;
  while(checkvalve == false){ //wait for valves to turn
    valvecheck();
    if (roastatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;
  while(checkvalve == false){ //wait for valves to turn
    valvecheck();
    if (robstatus ==0){
      checkvalve = true;
    }
   }
  while(checkvalve == false){ //wait for valves to turn
    valvecheck();
    if (nfastatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;
  while(checkvalve == false){ //wait for valves to turn
    valvecheck();
    if (nfbstatus ==0){
      checkvalve = true;
    }
   }
    checkvalve = false;
  while(checkvalve == false){ //wait for valves to turn
    valvecheck();
    if (wwrinsestatus ==0){
      checkvalve = true;
    }
   }
     checkvalve = false;
  while(checkvalve == false){ //wait for valves to turn
    valvecheck();
    if (wastestatus ==0){
      checkvalve = true;
    }
   }checkvalve = false;
}
void flows(){//reads all flows, both instantaneous and totals
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
void pressures(){//read and average pressure values
  int junk = analogRead(feedppin);
  int feedp = analogRead(feedppin)*0.60753-121.507;
  junk = analogRead(nfrejectppin);
  int nfrejectp = analogRead(nfrejectppin)*0.60753-121.507;
  junk = analogRead(rorejectppin);
  int rorejectp = analogRead(rorejectppin)*0.60753-121.507;
  junk = analogRead(EQtankpin);
  int prefiltp1 = analogRead(EQtankpin)*0.1215-24.3;
   junk = analogRead(settletankpin);
   int prefiltp2 = analogRead(settletankpin)*0.1215-24.3;
  snfrejectp = (snfrejectp+nfrejectp)/2;
  sfeedp = (sfeedp+feedp)/2;
  srorejectp = (srorejectp+rorejectp)/2;
  sEQtank = (prefiltp1+sEQtank)/2;
  sSettletank = (prefiltp2+sSettletank)/2;
}
void valvepos(){//find and average potentiometer values
  int junk = analogRead(ropotpin);
  int ropot= analogRead(ropotpin);
  junk = analogRead(nfpotpin);
  int nfpot= analogRead(nfpotpin);
  spotnf = (spotnf*4+nfpot)/5;
  rospot = (rospot*4+ropot)/5;
}
void rovalvecloseupflow(int num){ //closes plug valve a number of steps for flow control
  lcd.setCursor(0, 2);
  lcd.print("stepper closing  ");
  digitalWrite(rodir,HIGH);
  digitalWrite(rostpen, LOW); //enable motor turning
  for (int i=0; i<num; i++){  //make motor take a step
    digitalWrite(ros, HIGH);
    delay(25);
    digitalWrite(ros, LOW);
    delay(25);
    }
  digitalWrite(rostpen, HIGH);
  flows();
  valvepos();
}
void rovalveopenupflow(int num){ //opens plug valve a number of steps for flow control
  lcd.setCursor(0, 2);
  lcd.print("stepper opening  ");
  digitalWrite(rodir,LOW);
  digitalWrite(rostpen, LOW);
  for (int i=0; i<num; i++){ //make motor take a step
    digitalWrite(ros, HIGH);
    delay(25);
    digitalWrite(ros, LOW);
    delay(25);
  }
  digitalWrite(rostpen, HIGH);
  flows();
  valvepos();
}
void nfvalvecloseupflow(int num){//closes plug valve a number of steps for flow control
  lcd.setCursor(0, 2);
  lcd.print("stepper closing  ");
  digitalWrite(nfdir,HIGH);
  digitalWrite(nfstpen, LOW);
  for (int i=0; i<num; i++){ //make motor take a step
    digitalWrite(nfs, HIGH);
    delay(25);
    digitalWrite(nfs, LOW);
    delay(25);
  }
  digitalWrite(nfstpen, HIGH);
  flows();
  valvepos();
}
void nfvalveopenupflow(int num){//opens plug valve a number of steps for flow control
  lcd.setCursor(0, 2);
  lcd.print("stepper opening  ");
  digitalWrite(nfdir,LOW);
  digitalWrite(nfstpen, LOW);
  for (int i=0; i<num; i++){ //make motor take a step
    digitalWrite(nfs, HIGH);
    delay(25);
    digitalWrite(nfs, LOW);
    delay(25);
  }
  digitalWrite(nfstpen, HIGH);
  flows();
  valvepos();
}
void rocontrolopen(){//open valve until all the way open
  lcd.setCursor(0, 2);
  lcd.print("stepper opening  ");valvepos();
  while (rospot<800){
    digitalWrite(rodir,LOW);
    digitalWrite(rostpen, LOW);
    digitalWrite(ros, HIGH); //make motor take a step
    delay(25);
    digitalWrite(ros, LOW);
    delay(25);
    valvepos();
  }
  digitalWrite(rostpen, HIGH);
}
void nfcontrolopen(){//open valve until all the way open
  valvepos();//check position
  while (spotnf<800){
    digitalWrite(nfdir,LOW);
    digitalWrite(nfstpen, LOW);
    digitalWrite(nfs, HIGH);  //make motor take a step
    delay(25);
    digitalWrite(nfs, LOW);
    delay(25);
    valvepos();
  }
  digitalWrite(nfstpen, HIGH);//disable valve turn
}
void tanklevel(){//read and average tank levels for all 5 tanks
  int junk = analogRead(roftankpin);
  float roftank1 = (analogRead(roftankpin)*0.177-35.714286)*36.5*24*0.004329;
  float roftank2 = (analogRead(roftankpin)*0.177-35.714286)*36.5*24*0.004329;
  float roftank3 = (analogRead(roftankpin)*0.177-35.714286)*36.5*24*0.004329;
  float roftank4 = (analogRead(roftankpin)*0.177-35.714286)*36.5*24*0.004329;
  float roftank5 = (analogRead(roftankpin)*0.177-35.714286)*36.5*24*0.004329;
  float roftank6 = (analogRead(roftankpin)*0.177-35.714286)*36.5*24*0.004329;
  float roftank7 = (analogRead(roftankpin)*0.177-35.714286)*36.5*24*0.004329;
  float roftank8 = (analogRead(roftankpin)*0.177-35.714286)*36.5*24*0.004329;
  float roftank = (roftank1 + roftank2 + roftank3 + roftank4 + roftank5 + roftank6 + roftank7 + roftank8)/8;

  junk = analogRead(nfftankpin);
  float nfftank1 = (analogRead(nfftankpin)*0.177165-35.433)*36.5*24*0.004329;
  float nfftank2 = (analogRead(nfftankpin)*0.177165-35.433)*36.5*24*0.004329;
  float nfftank3 = (analogRead(nfftankpin)*0.177165-35.433)*36.5*24*0.004329;
  float nfftank4 = (analogRead(nfftankpin)*0.177165-35.433)*36.5*24*0.004329;
  float nfftank5 = (analogRead(nfftankpin)*0.177165-35.433)*36.5*24*0.004329;
  float nfftank6 = (analogRead(nfftankpin)*0.177165-35.433)*36.5*24*0.004329;
  float nfftank7 = (analogRead(nfftankpin)*0.177165-35.433)*36.5*24*0.004329;
  float nfftank8 = (analogRead(nfftankpin)*0.177165-35.433)*36.5*24*0.004329;
  float nfftank = (nfftank1 + nfftank2 + nfftank3 + nfftank4 + nfftank5 + nfftank6 + nfftank7 + nfftank8)/8;

  junk = analogRead(sbrtankpin);
  float pretank1 = analogRead(sbrtankpin)*0.668388-146.32479;
  float pretank2 = analogRead(sbrtankpin)*0.668388-146.32479;
  float pretank3 = analogRead(sbrtankpin)*0.668388-146.32479;
  float pretank4 = analogRead(sbrtankpin)*0.668388-146.32479;
  float pretank5 = analogRead(sbrtankpin)*0.668388-146.32479;
  float pretank6 = analogRead(sbrtankpin)*0.668388-146.32479;
  float pretank7 = analogRead(sbrtankpin)*0.668388-146.32479;
  float pretank8 = analogRead(sbrtankpin)*0.668388-146.32479;
  float pretank = (pretank1 + pretank2 + pretank3 + pretank4 + pretank5 + pretank6 + pretank7 + pretank8)/8;

  junk = analogRead(wwtankpin);
  float wwtank1 = analogRead(wwtankpin)*0.65375348-134.42675;
  float wwtank2 = analogRead(wwtankpin)*0.65375348-134.42675;
  float wwtank3 = analogRead(wwtankpin)*0.65375348-134.42675;
  float wwtank4 = analogRead(wwtankpin)*0.65375348-134.42675;
  float wwtank5 = analogRead(wwtankpin)*0.65375348-134.42675;
  float wwtank6 = analogRead(wwtankpin)*0.65375348-134.42675;
  float wwtank7 = analogRead(wwtankpin)*0.65375348-134.42675;
  float wwtank8 = analogRead(wwtankpin)*0.65375348-134.42675;
  float wwtank = (wwtank1 + wwtank2 + wwtank3 + wwtank4 + wwtank5 + wwtank6 + wwtank7 + wwtank8)/8;

  junk = analogRead(wastetankpin);
  float wastetank1 = (analogRead(wastetankpin)*0.14351-28.702)*24*18*0.004329;
  float wastetank2 = (analogRead(wastetankpin)*0.14351-28.702)*24*18*0.004329;
  float wastetank3 = (analogRead(wastetankpin)*0.14351-28.702)*24*18*0.004329;
  float wastetank4 = (analogRead(wastetankpin)*0.14351-28.702)*24*18*0.004329;
  float wastetank5 = (analogRead(wastetankpin)*0.14351-28.702)*24*18*0.004329;
  float wastetank6 = (analogRead(wastetankpin)*0.14351-28.702)*24*18*0.004329;
  float wastetank7 = (analogRead(wastetankpin)*0.14351-28.702)*24*18*0.004329;
  float wastetank8 = (analogRead(wastetankpin)*0.14351-28.702)*24*18*0.004329;
  float wastetank = (wastetank1 + wastetank2 + wastetank3 + wastetank4 + wastetank5 + wastetank6 + wastetank7 + wastetank8)/8;

  ssbrtank = (pretank+ssbrtank)/2;
  sroftank = (sroftank+roftank)/2;
  swwtank = (swwtank+wwtank)/2;
  snfftank = (snfftank+nfftank)/2;
  swastetank = (swastetank+wastetank)/2;
}
void printdata(){//prints all data to serial port for data collection by raspberry pi
  Serial.print("TANKS:\t"); Serial.print("WW\t");Serial.print("ROF\t");Serial.print("NFF\t");Serial.print("GW\t");Serial.print("WASTE\t");Serial.println("time");
  Serial.print("TANKD:\t"); Serial.print(swwtank,0);Serial.print("\t");Serial.print(sroftank,0);Serial.print("\t");Serial.print(snfftank,0);Serial.print("\t");

  Serial.print(ssbrtank,0);Serial.print("\t");Serial.print(swastetank,0);Serial.print("\t");Serial.println(timnow);
  Serial.print("PRESS:\t"); Serial.print("F\t");Serial.print("C1\t");Serial.print("C2\t");Serial.print("NFR\t");Serial.print("ROR\t");Serial.println("time");

  Serial.print("PRESSD:\t"); Serial.print(sfeedp);Serial.print("\t");Serial.print(sEQtank);Serial.print("\t");Serial.print(sSettletank);Serial.print("\t");
  Serial.print(snfrejectp);Serial.print("\t");Serial.print(srorejectp);Serial.print("\t");Serial.println(timnow);

  Serial.print("IFLOW:\t"); Serial.print("C\t");Serial.print("NFP\t");Serial.print("NFR\t");Serial.print("ROP\t");Serial.print("ROR\t");Serial.println("time");
  Serial.print("IFLOWD:\t"); Serial.print(flw[5]);Serial.print("\t");Serial.print(flw[9]);Serial.print("\t");Serial.print(flw[1]);Serial.print("\t");
  Serial.print(flw[7]);Serial.print("\t");Serial.print(flw[3]);Serial.print("\t");Serial.println(timnow);

  Serial.print("TFLOW:\t"); Serial.print("C\t");Serial.print("NFP\t");Serial.print("NFR\t");Serial.print("ROP\t");Serial.print("ROR\t");Serial.println("time");
  Serial.print("TFLOWD:\t"); Serial.print(flw[4],0);Serial.print("\t");Serial.print(flw[8],0);Serial.print("\t");Serial.print(flw[0],0);Serial.print("\t");
  Serial.print(flw[6],0);Serial.print("\t");Serial.print(flw[2],0);Serial.print("\t");Serial.println(timnow);

  Serial.print("TandP\t");Serial.print("OUT\t");Serial.print("AC\t");Serial.print("DC\t");Serial.print("PWRR\t");Serial.print("PWRB\t");Serial.println("time");
  Serial.print("TandPD\t");Serial.print(outtemp,0);Serial.print("\t");Serial.print(actemp,0);Serial.print("\t");Serial.print(dctemp,0);Serial.print("\t");Serial.print(redpwr,1);Serial.print("\t");Serial.print(blkpwr,1);Serial.print("\t");Serial.println(timnow);

  Serial.print("Relays\t");Serial.print("P\t");Serial.print("BUB\t");Serial.print("O3\t");Serial.print("O3pump\t");Serial.print("UV\t");Serial.println("time");
  Serial.print("RelayD\t");Serial.print(pumpon);Serial.print("\t");Serial.print(sbrairstatus);Serial.print("\t");Serial.print(ozonestatus);Serial.print("\t");Serial.print(bubbleblaststatus);Serial.print("\t");Serial.print(uvstatus);Serial.print("\t");Serial.println(timnow);

  Serial.print("1valves\t");Serial.print("NFPOT\t");Serial.print("NFF\t");Serial.print("NFFT\t");Serial.print("GW\t");Serial.print("CFF\t");Serial.println("time");
  Serial.print("1valveD\t");Serial.print(spotnf);Serial.print("\t");Serial.print(nfbstatus);Serial.print("\t");Serial.print(nfastatus);Serial.print("\t");Serial.print(pretankstatus);Serial.print("\t");Serial.print(prefiltstatus);Serial.print("\t");Serial.println(timnow);

  Serial.print("2valves\t");Serial.print("ROPOT\t");Serial.print("ROF\t");Serial.print("ROFT\t");Serial.print("WWT\t");Serial.print("WASTE\t");Serial.println("time");
  Serial.print("2valveD\t");Serial.print(rospot);Serial.print("\t");Serial.print(robstatus);Serial.print("\t");Serial.print(roastatus);Serial.print("\t");Serial.print(wwrinsestatus);Serial.print("\t");Serial.print(wastestatus);Serial.print("\t");Serial.println(timnow);

  Serial.println("");
}
void power(){//power readings in amps?--needs more calculation probably
  int junk=analogRead(blkpowerpin);
  int blkread= analogRead(blkpowerpin);

  junk=analogRead(redpowerpin);
  int redread=analogRead(redpowerpin);

  float blkv=blkread*0.00977517;
  float redv=redread*0.01955;
  blkpwr= blkv;
  redpwr= redv;
}
void temperature(){//reads temperature in both panels and the room
  //reads from 0x18 temperature sensor in DC panel
  /*if (!tempsensordc.begin(0x18)) {
    Serial.println("Couldn't find MCP9808 DC!");
    while (1);
  }
  tempsensordc.shutdown_wake(0);
  dctemp = tempsensordc.readTempC();//dc temp read
  delay(250);
  tempsensordc.shutdown_wake(1);*/
  //reads from 0x19 temperature sensor in AC panel
  if (!tempsensorac.begin(0x19)) {
    Serial.println("Couldn't find MCP9808 AC!");
    while (1);
  }
  tempsensorac.shutdown_wake(0);
  actemp = tempsensorac.readTempC();//ac temp read
  delay(250);
  tempsensorac.shutdown_wake(1);
  //reads from external temperature sensor
  if (! am2315.begin()) {
     Serial.println("Outside Sensor not found, check wiring & pullups!");
     while (1);
  }
  outtemp= am2315.readTemperature();//outside temp read
}
void o3calc(){//decides if ozone will be turned on or off or neither. based on timers and tank level
  if (systemstate ==1){
    if (t- o3time -o3starttime > 7200000 && ozonestatus ==0 && swwtank>20){
    o3time = swwtank *2 *60000/6.66;
    o3pump(1);
    delay(10000);
    o3(1);
    o3starttime=t;
   }

    if (t - o3starttime >= o3time && ozonestatus ==1){
    o3(0);
    delay(20000);
    delay(20000);
    delay(20000);
    o3pump(0);
  }
  }
  else if (systemstate ==3 && ozonestatus ==0){
    o3pump(1);
    delay(10000);
    o3(1);
    o3starttime=t;
    o3time = 600000;
  }
  else if (systemstate ==3 && ozonestatus ==1){}
  else if (systemstate ==2 && ozonestatus ==1){
    timenow();
    if (timnow-treattimes[2] > 600){
    o3(0);
    delay(20000);
    o3pump(0);}
  }
  else if (systemstate!=1 && ozonestatus==1){
    o3(0);
    delay(20000);
    delay(20000);
    delay(20000);//delay 1min
    o3pump(0);
  }}
void aircalc(){//decides if ozone will be turned on or off or neither. based on timers and tank level
    float w= millis();
    if (systemstate ==1){
      if (w- sbrairofftime > 3300000 && sbrairstatus ==0){
      SBRAironoff(1);
      delay(10000);
    }
      if (w - sbrairontime >= 300000 && sbrairstatus ==1){
        SBRAironoff(0);
        sbrairofftime=millis();
      delay(10000);
    }}
    else {
      SBRAironoff(0);
      sbrairofftime=millis();
      delay(10000);
    }
    }
void waiting(unsigned long interval){//function to read and report everything at given intervals
  t= millis();
  if (t-oldt > interval){
    oldt=t;
    tanklevel();
    flows();
    pressures();
    temperature();
    power();
    valvepos();
    valvecheck();
    timenow();
    printdata();
    o3calc();//also check on ozone timer to see if it needs to be turned on or not
  }
}
void RO(int target, int rinsecycle, int wastecycle, int uviswarm){//wastecycle is sendback
  SBRAironoff(1);
  if (swwtank >= target && sroftank< 5) {//if water needs to be treated
    return;}
    lcd.setCursor(0, 0);
    lcd.print("RO Treatment");
    closeallvalves();
    checkvalve = false;
    uvdisinfect(1);//turn on uvs or make sure they are on
    lcd.setCursor(0, 3);
    lcd.print("UV Warmup");
    waiting(1);
    if (uviswarm==0){
      while (t-uvontime< 900000){//warmup uv 15 min
        waiting(10000);}
    }
    lcd.setCursor(0, 3);
    lcd.print("UV Ready     ");
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

  //lcd.setCursor(0, 3);
  //lcd.print("all valves closed");

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
  sbraircyclestarttime = millis();
  hppump(1);
  pressures();
  float targetflow=0.75;
  while (swwtank< target && sroftank> 7){//treat as long as tank not too full or empty
    waiting(10000);
    lcd.setCursor(0, 3);
    lcd.print("productflow: ");lcd.print(flw[7]);
    pressures();
    flows();

    if (sfeedp>240){//pressure too high, quit function
      hppump(0);
      return;
    }
    valvepos();

    if (rospot<100){//means plug valve is closed, return
      return;
    }
    delay(1000);
    if (flw[7]>0.1 && flw[7]<=.5*targetflow && rospot>100 && pumpon==1 && sfeedp < 200){
      rovalvecloseupflow(40);//close valve alot
      valvepos();
    }

    if (flw[7]>.5*targetflow && flw[7]<targetflow-0.15 && rospot>100 && pumpon==1 && sfeedp < 200){
      rovalvecloseupflow(20);//close valve alot
      valvepos();
    }

    if (flw[7]>targetflow-0.15 && flw[7]<targetflow && rospot>100 && pumpon==1 && sfeedp < 200){//close valve a little bit
      rovalvecloseupflow(10);
      valvepos();
    }
    if (flw[7]>targetflow+.05 && flw[7]<=targetflow+0.15 && rospot<800 && pumpon==1){
      rovalveopenupflow(10);//open valve a little bit
      valvepos();
    }
    if (flw[7]>targetflow+0.15 && rospot>100 && rospot<800 && pumpon==1){
      rovalveopenupflow(20);//open valve a lot if necessary
      valvepos();
    }
    if (flw[7]>=targetflow && flw[7]<=targetflow+.05){//ideal flow
      lcd.setCursor(0, 2);
      lcd.print("correct flow");
      valvepos();}
    }
    hppump(0);
    rocontrolopen();//open plug valve for low pressure rinse
    SBRAironoff(0);
    checkvalve = false;
    digitalWrite(roa, LOW);//tank
    while(checkvalve == false){ //wait for valves to turn
      valvecheck();
      if (roastatus ==0){
        checkvalve = true;
      }
    }
    checkvalve = false;
    digitalWrite(rob, HIGH); //open membrane
        checkvalve = false;
  while(checkvalve == false){
    valvecheck();
    if (robstatus ==1){
    checkvalve = true;
    }}
  if (rinsecycle==1){
    waiting(1);
    digitalWrite(wwrinse, HIGH);
    checkvalve = false;
    while(checkvalve == false){ //wait for valves to turn
      valvecheck();
      if (wwrinsestatus ==1){
        checkvalve = true;
      }
    }
    checkvalve = false;
    hppump(1);
    unsigned long rinsetime =millis();//set initial rinse time for timer
    waiting(1);
    lcd.setCursor(0, 3);
    lcd.print("rinsing    ");
    while (t-rinsetime< 15000){ //rinse 15 sec
      waiting(3000);//report data
      if (sfeedp>240){
      hppump(0);//if pressure is too high shut down pump
      return;
      }
    }

    hppump(0);
    uvdisinfect(0);
    waiting(1);
    digitalWrite(wwrinse, LOW);
    while(checkvalve == false){ //wait for  valve to be closed
      valvecheck();
      if (wwrinsestatus ==0){
        checkvalve = true;
      }
    }
  checkvalve = false;
  }
  if (wastecycle ==1){//decide to run waste cycle. if gw tank is full donot run
    waiting(1);
    digitalWrite(roa, HIGH);//open tank for waste cycle
        checkvalve = false;
  while(checkvalve == false){
    valvecheck();
    if (roastatus ==1){
    checkvalve = true;
    }}
    digitalWrite(rob, LOW); //close membrane
        checkvalve = false;
  while(checkvalve == false){
    valvecheck();
    if (robstatus ==0){
    checkvalve = true;
    }}
    digitalWrite(waste, HIGH);//open sendback
        checkvalve = false;
  while(checkvalve == false){
    valvecheck();
    if (wastestatus ==1){
    checkvalve = true;
    } }
  waiting(1);
      lcd.setCursor(0, 3);
    lcd.print("wa RO   ");
  unsigned long wastetime = sroftank *4.8*1000;//mseconds to run pump
  hppump(1);
  delay(wastetime);//wait for empty
  hppump(0);
  delay(1000);
  waiting(1);
  digitalWrite(waste, LOW);//tank valve close
        checkvalve = false;
  while(checkvalve == false){
    valvecheck();
    if (wastestatus ==0){
    checkvalve = true;
    } }


  }
  digitalWrite(rob, LOW);
  while(checkvalve == false){ //wait for  valve to be closed
    valvecheck();
    if (robstatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;
  digitalWrite(roa, LOW);//tank
    while(checkvalve == false){ //wait for valves to turn
      valvecheck();
      if (roastatus ==0){
        checkvalve = true;
      }
    }
    checkvalve = false;
    waiting(1);
  systemstate=2;
  lcd.setCursor(0, 3);
  lcd.print("RO complete  ");
 SBRAironoff(0);
 }
void NF(int target, int rinsecycle, int wastecycle, int sbraeration){//determine if need to run rinse of waste cycles
  if (sroftank>= target && snfftank< 5) {//if water needs to be treated, returns if not
    return;}
    if (sbraeration==1){
      SBRAironoff(1);
    }
  lcd.setCursor(0, 0);
  lcd.print("NF Treatment");
  closeallvalves();
  checkvalve = false;
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

  digitalWrite(nfa, HIGH);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (nfastatus ==1){
      checkvalve = true;
    }
  }
  checkvalve = false;

  digitalWrite(nfb, HIGH);
  while(checkvalve == false){ //wait for drain and vent valves to be closed
    valvecheck();
    if (nfbstatus ==1){
      checkvalve = true;
    }
  }
  checkvalve = false;

  nfcontrolopen();//open plug valve all the way
  hppump(1);
  pressures();
  float targetflow=.75;
  while (sroftank< target && snfftank> 7){//(swwtank< 80 && sroftank> 5){
    waiting(10000);
    lcd.setCursor(0, 3);
    lcd.print("productflow: ");lcd.print(flw[9]);//display product flow on lcd screen
    pressures();
    flows();
    if (sfeedp>240){
      hppump(0);
      return;
    }
    valvepos();
    if (spotnf<100){//if plug valve is turned really far, exit
      return;
    }
    delay(1000);

    if (flw[9]>=0 && flw[9]<=.5*targetflow && spotnf>100 &&pumpon==1 && sfeedp < 200){
      nfvalvecloseupflow(40);//close valve alot
      valvepos();
    }
    if (flw[9]>0.5*targetflow && flw[9]<targetflow-0.15 && spotnf>100 && pumpon==1 && sfeedp < 200){
      nfvalvecloseupflow(20);
      valvepos();
    }
    if (flw[9]>targetflow-0.15 && flw[9]<targetflow && spotnf>100 && pumpon==1 && sfeedp < 200){//close valve a little bit
      nfvalvecloseupflow(10);
      valvepos();
    }
    if (flw[9]>targetflow+0.05 && flw[9]<=targetflow+0.15 && spotnf<800 && pumpon==1 ){
      nfvalveopenupflow(10);//open valve a little bit
      valvepos();
    }
    if (flw[9]>targetflow+0.15 && spotnf>100 && spotnf<800 && pumpon==1 ){
      nfvalveopenupflow(20);//open valve a lot
      valvepos();
    }
    if (flw[9]>=targetflow && flw[9]<=targetflow+0.05){
      lcd.setCursor(0, 2);
      lcd.print("correct flow");
      valvepos();}
    }
  hppump(0);
  nfcontrolopen();//open plup valve all the way
  SBRAironoff(0);

  if (rinsecycle==1){
    waiting(1);
        digitalWrite(nfb, HIGH);//valve open
        checkvalve = false;
  while(checkvalve == false){
    valvecheck();
    if (nfbstatus ==1){
    checkvalve = true;
    }
  }checkvalve = false;
    digitalWrite(nfa, LOW);//valve closed
  while(checkvalve == false){
    valvecheck();
    if (nfastatus ==0){
    checkvalve = true;
    }
  }
  checkvalve = false;
  digitalWrite(wwrinse, HIGH);//valve open
    while(checkvalve == false){
      valvecheck();
      if (wwrinsestatus ==1){
        checkvalve = true;
      }
    }
    checkvalve = false;
    delay(1000);
    hppump(1);//pump on for rinse
    lcd.setCursor(0, 3);
    lcd.print("rinsing     ");
    unsigned long rinsetime =millis();//interval for rinse time. must be unsigned long
    waiting(1);
    while (t-rinsetime< 15000){ //rinse 15 sec
      waiting(3000);//report data every 3 sec
      if (sfeedp>240){//if pressure is too high, quit
      hppump(0);
      return;
      }
    }
    hppump(0);
    digitalWrite(wwrinse, LOW);
  checkvalve = false;
  while(checkvalve == false){
    valvecheck();
    if (wwrinsestatus ==0){
      checkvalve = true;
    }
  }
  }//end rinse


  if (wastecycle ==1 && swastetank<maxwaste-15){//decide if to run waste cycle if not does send back of cencentrate
    digitalWrite(nfa, HIGH);//valve open
        checkvalve = false;
  while(checkvalve == false){
    valvecheck();
    if (nfastatus ==1){
    checkvalve = true;
    }}
    digitalWrite(nfb, LOW);//valve close
        checkvalve = false;
  while(checkvalve == false){
    valvecheck();
    if (nfbstatus ==0){
    checkvalve = true;
    }}
    digitalWrite(waste, HIGH);//valve open
        checkvalve = false;
  while(checkvalve == false){
    valvecheck();
    if (wastestatus ==1){
    checkvalve = true;
    } }
  waiting(1);
      lcd.setCursor(0, 3);
    lcd.print("wasting NF       ");
  unsigned long wastetime = snfftank *4.8*1000;//milliseconds to run pump
  //Serial.print(wastetime);
  hppump(1);
  delay(wastetime);//wait for empty
  hppump(0);
  delay(5000);
  waiting(500);
  digitalWrite(waste, LOW);
        checkvalve = false;
  while(checkvalve == false){//waste tank closed
    valvecheck();
    if (wastestatus ==0){
    checkvalve = true;
    } }


  }
  else if (wastecycle==0 && ssbrtank<85){//if not waste day send back concentrate
    digitalWrite(nfa, HIGH);//valve open
        checkvalve = false;
  while(checkvalve == false){
    valvecheck();
    if (nfastatus ==1){
    checkvalve = true;
    }}
    digitalWrite(nfb, LOW);//valve close
        checkvalve = false;
  while(checkvalve == false){
    valvecheck();
    if (nfbstatus ==0){
    checkvalve = true;
    }}
    digitalWrite(sendback, HIGH);//valve open
        checkvalve = false;
  while(checkvalve == false){
    valvecheck();
    if (sendbackstatus ==1){
    checkvalve = true;
    } }
  waiting(1);
      lcd.setCursor(0, 3);
    lcd.print("sendback NF       ");
  unsigned long wastetime = snfftank *4.8*1000;//milliseconds to run pump
  //Serial.print(wastetime);
  hppump(1);
  delay(wastetime);//wait for empty
  hppump(0);
  delay(5000);
  waiting(500);
  digitalWrite(sendback, LOW);
        checkvalve = false;
  while(checkvalve == false){//sendback closed
    valvecheck();
    if (sendbackstatus ==0){
    checkvalve = true;
    } }
    uvdisinfect(0);

  checkvalve = false;

  digitalWrite(nfb, LOW);
  while(checkvalve == false){
    valvecheck();
    if (nfbstatus ==0){
      checkvalve = true;
    }
  }
  checkvalve = false;
  digitalWrite(nfa, LOW);
  while(checkvalve == false){
    valvecheck();
    if (nfastatus ==0){
      checkvalve = true;
    }
  }}
  checkvalve = false;
  waiting(1);
  //systemstate=1;
  lcd.setCursor(0, 3);
  lcd.print("NF complete  ");
    SBRAironoff(0);
  }
void SBRFill(int filllevel){
    lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Settle 2 SBR Fill");
  waiting(1);
  float highlvlEQ= 30;//inches
  float lowlvlEQ= 0.2;//inches
  if(ssbrtank<=filllevel && sSettletank>lowlvlEQ) {
    while(sSettletank>lowlvlEQ && ssbrtank<filllevel){
      Sett2SBR(1);
      waiting(3000);
      delay(100);
    }
    Sett2SBR(0);
    waiting(1);
    delay(1000);
  }
}
void SBRDecantCF(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SBR Decant    ");
  waiting(1);
  float highlvlEQ= 30;//inches
  float lowlvlEQ= 5;//inches
  if(ssbrtank>=14 && snfftank<80) {
    while(ssbrtank>14 && snfftank<80){
      digitalWrite(SBRDecant, HIGH);
      waiting(3000);
    }
    digitalWrite(SBRDecant, LOW);
    waiting(1);
    delay(1000);
  }
}
void SBRfullair(){
  sbraircyclestarttime=millis();
    unsigned long tnow= millis();
    SBRAironoff(1);
  sbrairontime=tnow;
  tnow= millis();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("full Air    ");
  waiting(1);
  lcd.setCursor(0,1);
  lcd.print(ssbrtank);lcd.print("gal  ");
  while (tnow-sbraircyclestarttime<3600000){//2hrs
    int timedisplay = round((3600000-(tnow-sbraircyclestarttime))/60/1000);
    delay(1000);
    waiting(5);
  lcd.setCursor(0,1);
  lcd.print(ssbrtank);lcd.print("gal  ");
  lcd.print(timedisplay);lcd.print("min  ");
    delay(1000);
    tnow= millis();
    }//turn on if it was off for 2 min
    SBRAironoff(0);}
void SBRthirtysecair(){
  sbraircyclestarttime=millis();
    unsigned long tnow= millis();
    SBRAironoff(1);
  sbrairontime=tnow;
  tnow= millis();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("30sec Air    ");
  waiting(1);
  lcd.setCursor(0,1);
  lcd.print(ssbrtank);lcd.print("gal  ");
  while (tnow-sbraircyclestarttime<7200000){//2hrs
    int timedisplay = round((7200000-(tnow-sbraircyclestarttime))/60/1000);
    delay(1000);
    waiting(5);
  lcd.setCursor(0,1);
  lcd.print(ssbrtank);lcd.print("gal  ");
  lcd.print(timedisplay);lcd.print("min  ");
    delay(1000);
    tnow= millis();
      if (sbrairstatus==1 && tnow-sbrairontime>30000){
        SBRAironoff(0); }//turn off if it was on for 30sec
      if (sbrairstatus==0 && tnow-sbrairofftime>120000){
        SBRAironoff(1); }}//turn on if it was off for 2 min
    SBRAironoff(0);}
void SBRfiveminair(){
  sbraircyclestarttime=millis();
    delay(1000);
    unsigned long tnow= millis();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("5min Air    ");
    waiting(1);
    lcd.setCursor(0,1);
    lcd.print(ssbrtank);lcd.print("gal  ");
    SBRAironoff(1);
  sbrairontime=tnow;
    tnow= millis();
  while (tnow-sbraircyclestarttime<3600000){//2hrs
    int timedisplay = round((3600000-(tnow-sbraircyclestarttime))/60/1000);
    delay(1000);
    waiting(5);
  lcd.setCursor(0,1);
  lcd.print(ssbrtank);lcd.print("gal  ");
  lcd.print(timedisplay);lcd.print("min  ");waiting(10000);
    tnow= millis();
      if (sbrairstatus==1 && tnow-sbrairontime>300000){
        SBRAironoff(0); }//turn off if it was on for 5 min
      if (sbrairstatus==0 && tnow-sbrairofftime>300000){
        SBRAironoff(1); }}//turn on if it was off for 5 min
    SBRAironoff(0);}
void SBRSettling(){
    float duration=2*60*60;//3hrs
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Settle ");
    waiting(1);
    lcd.setCursor(0,1);
    lcd.print(ssbrtank);lcd.print("gal  ");
    unsigned long starttime = timnow;
    delay(100);
    waiting(1);
    unsigned long tnow=timnow;
    while ((tnow-starttime)<duration){
      tnow=timnow;
      int timedisplay = round((duration-(tnow-starttime))/60);
      delay(10000);
      waiting(5);
    lcd.setCursor(0,1);
    lcd.print(ssbrtank);lcd.print("gal  ");
    lcd.print(timedisplay);lcd.print("min  ");} //2 min
  }
void eqtosettlefill(int UVwarmup){
  waiting(1);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("EQ to SettleTank ");
  //if (UVwarmup==1){
    //uvdisinfect(1);
  //}
  float highlvlEQ= 28;//inches
  float lowlvlEQ= 2;//inches
  if(sEQtank>=lowlvlEQ && sSettletank<=highlvlEQ) {
    while(sSettletank<highlvlEQ && sEQtank>=lowlvlEQ){
      EQ2Sett(1);
      waiting(3000);
    }
    EQ2Sett(0);
    waiting(1);
    delay(1000);
  }
}
void fixaverages(int number){//takes 10 readings to stabalize values
  for (int i=0; i<= number; i++){
  delay(10);
  waiting(1);}
  delay(3000);
}
void regularday(){
  fixaverages(10);
  systemstate=0;
  //uvdisinfect(1);
  if (systemstate==0){
    treattimes[0]=timnow;
    SBRFill(50);
    eqtosettlefill(0);
    SBRthirtysecair();
    SBRfiveminair();
    SBRfiveminair();
    fixaverages(10);
  }systemstate=3;
  if (systemstate ==3){
    treattimes[1]=timnow;
    uvdisinfect(1);
    RO(81,1,0,0);//ro treatment no waste cycle
    uvdisinfect(0);
    systemstate=2;
  }
  if (systemstate==2){
    treattimes[2]=timnow;
    fixaverages(10);
    NF(81,1,0,0);//nf treatment no waste cycle
    fixaverages(10);
  //  SBRfullair();
    SBRSettling();
    SBRDecantCF();
    lcd.setCursor(0, 0);
    lcd.print("Treatment cycle complete  ");
    fixaverages(10);
      SBRFill(50);
      eqtosettlefill(0);
      fixaverages(10);
      SBRthirtysecair();
      SBRfiveminair();
      SBRfiveminair();
      SBRfullair();
      SBRSettling();
      SBRDecantCF();
    lcd.setCursor(0, 0);
    lcd.print("sbr cycle done");
  }
  treattimes[3]=timnow;
  lcd.setCursor(0, 0);
  lcd.print("Treatment complete  ");
  systemstate=1;
}
void wasteday(){
  fixaverages(10);
  systemstate=0;
  if (systemstate==0){
    treattimes[0]=timnow;
    SBRFill(50);
    eqtosettlefill(0);
    SBRthirtysecair();
    SBRfiveminair();
    SBRfiveminair();
    fixaverages(10);
  }systemstate=3;
  if (systemstate ==3){
    treattimes[1]=timnow;
      uvdisinfect(1);
      RO(85,1,1,0);
      uvdisinfect(0);
      systemstate=2;
  }
  if (systemstate==2){
    treattimes[2]=timnow;
    fixaverages(10);
    NF(85,1,0,0);
    fixaverages(10);
    SBRSettling();
    SBRDecantCF();
    lcd.setCursor(0, 0);
    lcd.print("Treatment cycle complete  ");
    fixaverages(10);
      SBRFill(50);
      eqtosettlefill(0);
      fixaverages(10);
      SBRthirtysecair();
      SBRfiveminair();
      SBRfiveminair();
      SBRfullair();
      SBRSettling();
      SBRDecantCF();
    lcd.setCursor(0, 0);
    lcd.print("sbr cycle done");
  }
  treattimes[3]=timnow;
  lcd.setCursor(0, 0);
  lcd.print("Waste created ");
  systemstate=1;
}
void halfwasteday(){
  fixaverages(10);
    SBRFill(50);
    eqtosettlefill(0);
    fixaverages(10);
    SBRthirtysecair();
    SBRfiveminair();
    SBRfiveminair();
    SBRfullair();
    SBRSettling();
    SBRDecantCF();
  lcd.setCursor(0, 0);
  lcd.print("sbr cycle done");
}
//******     END FUNCTIONS     ******//
void serialEvent() {   //This interrupt will trigger when the data coming from the serial monitor(pc/mac/other) is received
  if (Serial.available() > 0) {
    incomingByte = Serial.readStringUntil('\n');
    if (incomingByte == String("RegularDay")){
      regularday();//command for start regular day
    }
    if (incomingByte == String("FullWasteDay")){
      wasteday();//command for start waste day
    }
    if (incomingByte == String("NFwoRinse")){
      NF(80,0,0,0);//comand for nf treatment with no rinse and no waste
    }
    if (incomingByte == String("NFwithRinse")){
      NF(80,1,0,1);//comand for nf treatment with rinse and no waste//test out waste
    }
    if (incomingByte == String("ROwoRinse")){
      RO(80,0,0,0);//command for ro treatment with no rinse no waste
    }
    if (incomingByte == String("ROwithRinse")){
      RO(80,1,0,1);//command for ro treatment with rinse no waste
    }
    if (incomingByte ==String("CFwoRinse")){
      SBRFill(50);//command for pretreatment without rinse
    }
    if (incomingByte ==String("CFwithRinse")){
      eqtosettlefill(0);//command for pretreatment with rinse
    }
    if (incomingByte == String("HalfWasteDay")){
      halfwasteday();
    }
  }

}
//******     BEGIN LOOP     ******//
void loop() {
  waiting(60000);//sending serial data
  delay(10000);
  aircalc();
  while(xx==1){
    xx=2;
    /*SBRfiveminair();
    fixaverages(10);
    uvdisinfect(1);
    SBRAironoff(1);
    RO(81,1,0,0);//ro treatment no waste cycle
    SBRAironoff(0);
    uvdisinfect(0);
    fixaverages(10);
    NF(81,1,0,0);//nf treatment no waste cycle
    fixaverages(10);
    SBRSettling();
    SBRDecantCF();*/
  }
  }
//******     END LOOP     ******//
