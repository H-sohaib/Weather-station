#include <LiquidCrystal.h>
#include <TimeLib.h>
#include <DHT.h>
#include <DS3231.h>//Memanggil RTC3231 Library
#include <Wire.h>  // i2C Conection Library
#include <EEPROM.h>

//----DEFINE---------------------
#define sensorThres  400
#define DHTTYPE DHT11
#define bt_time   A0
#define bt_up     A1
#define bt_down   A2
#define bt_alarm  A3
#define bt_select A4
#define smokeA0   A5
#define LDR       A6
#define buzzer    8
#define DHTPIN    9
//---------------------------------
//--------------------------------------
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(2,3,4,5,6,7);
DS3231 rtc(20, 21);
// Init a Time-data structure
Time  t; //pencacah string time()

int hh = 0, mm = 0, ss = 0, dd = 0, bb = 0, set_day;
int yy = 0;
String Day = "  ";

int AlarmHH  = 21, AlarmMM  = 22, AlarmSS  = 23, setMode = 0, setAlarm = 0, alarmMode=0;
bool select_mode = false ;
int stop =0, mode=0, flag=0;

//Eeprom Store Variable
uint8_t HH;
uint8_t MM;

 byte bell_symbol[8] = {
        B00100,
        B01110,
        B01110,
        B01110,
        B01110,
        B11111,
        B01000,
        B00100};
byte thermometer_symbol[8] = {
        B00100,
        B01010,
        B01010,
        B01110,
        B01110,
        B11111,
        B11111,
        B01110};
byte night_symbol[8] = {
        B00100,
        B01010,
        B01010,
        B01110,
        B01110,
        B11111,
        B11111,
        B01110};


void setup(){
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16,2);                      // initialize the lcd 
  rtc.begin(); // memulai koneksi i2c dengan RTC

   pinMode(bt_time,  INPUT_PULLUP);
   pinMode(bt_up,    INPUT_PULLUP);
   pinMode(bt_down,  INPUT_PULLUP);
   pinMode(bt_alarm, INPUT_PULLUP);
   pinMode(bt_select, INPUT_PULLUP);
   pinMode(smokeA0, INPUT);
   pinMode(LDR,INPUT);
   pinMode(buzzer, OUTPUT);

  lcd.createChar(1, thermometer_symbol);
  lcd.createChar(2, bell_symbol);

  lcd.setCursor (0,0);
  lcd.print(" Station Meteo");
  lcd.setCursor (0,1);
  lcd.print("   With Alarm ");
  delay (2000);
  lcd.clear();

  stop=EEPROM.read(50);
  if(stop==0){  
  }else{WriteEeprom ();}

  EEPROM.write(50,0); 
  ReadEeprom();
}

void loop(){ 
  int analogSensor = analogRead(smokeA0);
  int ldrStatus = analogRead(LDR);
//--------------------------------------------
  t = rtc.getTime();
  Day = rtc.getDOWStr(1);
  int state=digitalRead(bt_select);
  if (state== LOW) {delay(100);select_mode = !select_mode;delay(50);lcd.clear();}
  
  if(!select_mode){
    Serial.println("nonapuuier");
  if (setMode == 0){
  hh = t.hour,DEC;
  mm = t.min,DEC;
  ss = t.sec,DEC;
  dd = t.date,DEC;
  bb = t.mon,DEC;
  yy = t.year,DEC;
  }  

  if(setAlarm==0){
  lcd.setCursor(0,0); 
  lcd.print((hh/10)%10);
  lcd.print(hh % 10); 
  lcd.print(":");
  lcd.print((mm/10)%10);
  lcd.print(mm % 10);
  lcd.print(":");
  lcd.print((ss/10)%10);
  lcd.print(ss % 10);
  lcd.print(" ");  
  if(mode==1){lcd.write(2);}
  else{lcd.print(" ");}   
  lcd.print(" "); 
  lcd.write(1); 
  lcd.print(rtc.getTemp(),0);
  lcd.write(223); 
  lcd.print("C");
  lcd.print("  "); 

  lcd.setCursor(1,1);
  lcd.print(Day);
  lcd.print(" ");
  lcd.print((dd/10)%10);
  lcd.print(dd % 10); 
  lcd.print("/");
  lcd.print((bb/10)%10);
  lcd.print(bb % 10);
  lcd.print("/"); 
  lcd.print((yy/1000)%10);
  lcd.print((yy/100)%10);
  lcd.print((yy/10)%10);
  lcd.print(yy % 10);
  }


setupClock();
setTimer();
delay (100);
blinking();

//Alarm
if (alarmMode==1 && mode==1 && hh==AlarmHH && mm==AlarmMM && ss>=AlarmSS) {
digitalWrite(buzzer, HIGH);
delay (300);
digitalWrite(buzzer, LOW);
}else{digitalWrite(buzzer, LOW);}

delay (100);
}
if(select_mode) {
  //lcd.clear();
  //lcd.setCursor(0,0);
  //lcd.print("Mode 2") ;
  Serial.println("appuier");
  //**************************
  //if (state== LOW) {delay(100);select_mode = !select_mode;delay(50);lcd.clear();}
  select_mode = !select_mode ;
 if (analogSensor > sensorThres)
  {
    lcd.setCursor(0, 0);
    lcd.print("qualite d'Air:");
    lcd.setCursor(4, 1);
    lcd.print("dangereux");
    tone(buzzer, 1000, 200);
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("qualite d'Air:");
    lcd.setCursor(4, 1);
    lcd.print("Bonne");
    noTone(buzzer);
  }
  delay(3000);
  lcd.clear();
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("Temperature = ");
  lcd.setCursor(4, 1);
   lcd.print( String(dht.readTemperature())+" C");
   delay(3000);
    delay(100);
   lcd.clear();
    lcd.setCursor(0, 0);
  lcd.print("Humidite = ");
  lcd.setCursor(4, 1);
  lcd.print(String(dht.readHumidity())+" %");
   delay(3000);
   lcd.clear();
    delay(100);
    
  lcd.setCursor(5, 0);  
    if (ldrStatus <= 250) {
lcd.print("jour");
} else {
lcd.print("nuit");
}
 delay(3000);
   lcd.clear();
    delay(100);
  
  }
}

void blinking (){
//BLINKING SCREEN
if (setAlarm <2 && setMode == 1){lcd.setCursor(0,0);  lcd.print("  ");}
if (setAlarm <2 && setMode == 2){lcd.setCursor(3,0);  lcd.print("  ");}
if (setAlarm <2 && setMode == 3){lcd.setCursor(6,0);  lcd.print("  ");}
if (setAlarm <2 && setMode == 4){lcd.setCursor(1,1);  lcd.print("   ");}
if (setAlarm <2 && setMode == 5){lcd.setCursor(5,1);  lcd.print("  ");}
if (setAlarm <2 && setMode == 6){lcd.setCursor(8,1);  lcd.print("  ");}
if (setAlarm <2 && setMode == 7){lcd.setCursor(11,1); lcd.print("    "); }
//Alarm
if (setMode == 0 && setAlarm == 1){lcd.setCursor(6,0); lcd.print("           "); }
if (setMode == 0 && setAlarm == 2){lcd.setCursor(4,1); lcd.print("  "); }
if (setMode == 0 && setAlarm == 3){lcd.setCursor(7,1); lcd.print("  "); }
if (setMode == 0 && setAlarm == 4){lcd.setCursor(10,1);lcd.print("  "); }
}

//Seting Jam ,Tanggal,Alarm/Timer
void setupClock (void) {
   
    if (setMode == 8){
    lcd.setCursor (0,0);
    lcd.print (F("Set Date Finish "));
    lcd.setCursor (0,1);
    lcd.print (F("Set Time Finish "));
    delay (1000);
    rtc.setTime (hh, mm, ss);
    rtc.setDate (dd, bb, yy);  
    lcd.clear();
    setMode = 0;
    }

    if (setAlarm == 5){
    lcd.setCursor (0,0);
    lcd.print (F("Set Alarm Finish"));
    lcd.setCursor (0,1);
    lcd.print (F("-EEPROM Updated-"));
    WriteEeprom();
    delay (2000); 
    lcd.clear();
    setAlarm=0;
    alarmMode=1;
    }
    
 if (setAlarm >0){ alarmMode=0;}
    
 if(digitalRead (bt_time) == 0 && flag==0) {flag=1;
 if(setAlarm>0){setAlarm=5;}
 else{setMode = setMode+1;}
 }
  
 if(digitalRead (bt_alarm) == 0 && flag==0){flag=1;
 if(setMode>0){setMode=8;}
  else{setAlarm = setAlarm+1;} 
  lcd.clear();} 

if(digitalRead (bt_time) == 1 && digitalRead (bt_alarm) == 1){flag=0;}
  
 if(digitalRead (bt_up) == 0){                          
            if (setAlarm<2 && setMode==1)hh=hh+1; 
            if (setAlarm<2 && setMode==2)mm=mm+1;
            if (setAlarm<2 && setMode==3)ss=ss+1;
            if (setAlarm<2 && setMode==4)set_day=set_day+1;
            if (setAlarm<2 && setMode==5)dd=dd+1;
            if (setAlarm<2 && setMode==6)bb=bb+1;
            if (setAlarm<2 && setMode==7)yy=yy+1;
            //Alarm
            if (setMode==0 && setAlarm==1)mode=1;
            if (setMode==0 && setAlarm==2 && AlarmHH<23)AlarmHH=AlarmHH+1;
            if (setMode==0 && setAlarm==3 && AlarmMM<59)AlarmMM=AlarmMM+1;
            if (setMode==0 && setAlarm==4 && AlarmSS<59)AlarmSS=AlarmSS+1;

if(hh>23)hh=0;
if(mm>59)mm=0;
if(ss>59)ss=0;
if(set_day>7)set_day=0;
if(dd>31)dd=0;
if(bb>12)bb=0;
if(yy>2030)yy=2000;
rtc.setDOW(set_day);
 }        

if(digitalRead (bt_down) == 0){                                      
            if (setAlarm<2 && setMode==1)hh=hh-1; 
            if (setAlarm<2 && setMode==2)mm=mm-1;
            if (setAlarm<2 && setMode==3)ss=ss-1;
            if (setAlarm<2 && setMode==4)set_day=set_day-1;
            if (setAlarm<2 && setMode==5)dd=dd-1;
            if (setAlarm<2 && setMode==6)bb=bb-1;
            if (setAlarm<2 && setMode==7)yy=yy-1;
            //Alarm
            if (setMode==0 && setAlarm==1 )mode=0;
            if (setMode==0 && setAlarm==2 && AlarmHH>0)AlarmHH=AlarmHH-1;
            if (setMode==0 && setAlarm==3 && AlarmMM>0)AlarmMM=AlarmMM-1;
            if (setMode==0 && setAlarm==4 && AlarmSS>0)AlarmSS=AlarmSS-1;
if(hh<0)hh=23;
if(mm<0)mm=59;
if(ss<0)ss=59;
if(set_day<0)set_day=7;
if(dd<0)dd=31;
if(bb<0)bb=12;
if(yy<0)yy=2030;
rtc.setDOW(set_day);
 }
 
}

void setTimer (){
  //Alarm
 if (setMode == 0 && setAlarm >0){
  lcd.setCursor (0,0);
  lcd.print("Alarm ");
 if(mode==0){lcd.print("Deactivate");}
        else{lcd.print("Activated ");}
        
  lcd.setCursor (4,1);
  lcd.print((AlarmHH/10)%10);
  lcd.print(AlarmHH % 10);
  lcd.print(":");
  lcd.print((AlarmMM/10)%10);
  lcd.print(AlarmMM % 10);
  lcd.print(":");
  lcd.print((AlarmSS/10)%10);
  lcd.print(AlarmSS % 10);
 }
}

void ReadEeprom () {
  AlarmHH=EEPROM.read(1);
  AlarmMM=EEPROM.read(2);
  AlarmSS=EEPROM.read(3);
  
  mode=EEPROM.read(4); 
}

void WriteEeprom () {
  EEPROM.write(1,AlarmHH);
  EEPROM.write(2,AlarmMM);
  EEPROM.write(3,AlarmSS);
  
  EEPROM.write(4,mode);
}
