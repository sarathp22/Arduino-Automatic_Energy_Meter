#include<EEPROM.h>
#include<LiquidCrystal.h>
#include <RealTimeClockDS1307.h>
LiquidCrystal lcd(22,23,24,25,26,27);
unsigned long time_t=0, nopowercut_time=0, powercut_time=0;
int pulse_count=0, unit=0, unit_2=0, p=0, u=0, u2=0, days=0;
int i=0, j, alen=0, comp=1, t=1, off=0, on=0, y=0, z=0, toff=0, ton=0;
int interrupt=0, powercut=0, powercutack=0, powerstatus=1, prevpwrstat=0, ir=0, previrvalue=0, fusestatus=0, prevfusestat=0;
char a[80], b[]="+CMT", c[20], d[]="$1", e[]="$2", f[]="$3", g[]="$4", h[]="$5", msg[20];
void ackmsg(char []);
void LCD(int);
byte k, m, n, l, o;
float price=0;
void setup() 
{
 Serial.begin(9600);
 lcd.begin(16,2); 
 lcd.clear();
 lcd.print(" CONFIGURING... ");
 delay(12000);
 Serial.println("ATE0");
 Serial.println("AT+CMGF=1");
 Serial.println("AT+CNMI=2,2,0,0,0");
 pinMode(13,OUTPUT);
 //pinMode(A0,OUTPUT);
 pinMode(30,INPUT);
 pinMode(2,OUTPUT);
 pinMode(8,OUTPUT);//connection/disconnection pin
 pinMode(10,INPUT);//IR input pin
 pinMode(32,INPUT);//Reset Button Pin
 pinMode(33,INPUT);//Power Failure Switch
 
 /*EEPROM.write(0000,00);
 EEPROM.write(0001,00);
 EEPROM.write(0002,00);*/
 attachInterrupt(digitalPinToInterrupt(3),pulse,RISING); // Interrupt to detect pulses.
 digitalWrite(13,0);
 digitalWrite(2,0);
 digitalWrite(8,1);
}

 /*//////////////////////////////////////////
            PULSE COUNT   
 //////////////////////////////////////////*/

void pulse()
{
 pulse_count++;
 //Serial.println(pulse_count);
 EEPROM.write(0000,pulse_count);
 k=EEPROM.read(0000);
 if(k==3) // 3 pulses = 1 unit
 {
  unit++;
  Serial.print("Units = ");
  Serial.println(unit);
  EEPROM.write(0001,unit); // Unit value stored in EEPROM location 0001
  pulse_count=0;
  EEPROM.write(0000,pulse_count);
  //Serial.println(k,DEC);
  //Serial.print("\n");
 }
}

 /*//////////////////////////////////////////
               LCD DISPLAY   
 //////////////////////////////////////////*/

void LCD(int u)
{
 lcd.setCursor(0,0);
 if(days==30)
 {
  lcd.print("   BILL DATE   ");
  delay(1000);
 }
 lcd.print("UNITS = ");
 lcd.setCursor(8,0);
 lcd.print(u);
 if(u<=10) // price calculation based on unit consumption
 {
  price=u*5;
 }
 if(u>10 && u<=15)
 {
  price=u*5.7;
 }
 if(u>15 && u<=20)
 {
  price=u*6.1;
 }
 if(u>20 && u<=30)
 {
  price=u*6.7;
 }
 if(u>30)
 {
  price=u*7.5;
 }
 lcd.setCursor(0,1);
 lcd.print("AMT : Rs. ");
 lcd.setCursor(10,1);
 lcd.print(price);
}

//////////////////////////////////////////

void ackmsg(char ack[])
{
 Serial.println("AT+CMGF=1");
 delay(500);
 Serial.println("AT+CMGS=\"8129127297\"\r");
 delay(500);
 Serial.println(ack);
 delay(500);
 Serial.println((char)26);
 delay(500);
}

//////////////////////////////////////////

void loop() 
{
 //analogWrite(A0,127);
 RTC.readClock(); 
 pulse_count=EEPROM.read(0000);
 unit=EEPROM.read(0001);
 powerstatus=digitalRead(30);
 ir=digitalRead(10); // Checking if meter opened.
 days=RTC.getDay();  // Gets day from RTC
 digitalWrite(13,0);
 i=0;
 j=0;
 lcd.clear();
 LCD(unit);

 /*//////////////////////////////////////////
                    RESET  
 //////////////////////////////////////////*/

 if(digitalRead(32)) // If reset button pressed, meter is reset
 {
  lcd.clear();
  lcd.print("  METER RESET  ");
  EEPROM.write(0000,00);
  EEPROM.write(0001,00);
  EEPROM.write(0002,00);
  EEPROM.write(0010,00);
  EEPROM.write(0011,00); 
 }

 /*//////////////////////////////////////////
                  POWER FAILURE  
 //////////////////////////////////////////*/

 if(digitalRead(33))
 {
  while(digitalRead(33));
  ackmsg("POWER FAILURE");
 }

 /*//////////////////////////////////////////
            IR CHECK   
 //////////////////////////////////////////*/

 if(ir!=previrvalue)
 {
  if(ir==0)
  {
   ackmsg("METER OPENED");
   lcd.clear();
   lcd.print("  METER OPENED  ");
  }
 }
 previrvalue=ir;
 
//////////////////////////////////////////////
 
 if(Serial.available()>0)
 {

 /*//////////////////////////////////////////
            SMS READING   
 //////////////////////////////////////////*/
 
  i=0;
  j=0;
  t=1;
  comp=1;
  
  while(Serial.available()>0)
  {
   a[i]=Serial.read(); 
   //a[i]=d;
   i++;
  }
  alen=strlen(a);
  //Serial.println(alen);
  //delay(500);
  Serial.println(a);
  //delay(500);
 
  for(j=0,i=2;i<=5;i++,j++)
  {
   c[j]=a[i];
  }
  Serial.println(c);
  //delay(500);
  comp=strcmp(c,b);
  if(comp==0)//// CHECKS IF RECEIVED A MESSAGE
  {
   for(i=51,j=0;i<53;i++,j++)
   {
    msg[j]=a[i];
   }
   Serial.println(msg);
   //delay(500);
   comp=1;
  //}
// } 
//}

   t=strcmp(msg,d);
   if(t==0)////PULSE COUNT SMS
   {
    //Serial.println("AAAA");
    //delay(500);
    m=EEPROM.read(0001);
    Serial.println("AT+CMGF=1");
    delay(500);
    Serial.println("AT+CMGS=\"8129127297\"\r");
    delay(500);
    Serial.print(m,DEC);
    delay(500);
    Serial.println((char)26);
    delay(500);
    t=1;
    m=0;
    EEPROM.write(0001,m);
   }

   if(strcmp(msg,e)==0)////DISCONNECT SMS
   {
    off=1;
    on=0;
    z=1;
   }

   if(strcmp(msg,f)==0)////RECONNECT SMS
   {
    on=1;
    off=0;
    y=1;
   }

   if(strcmp(msg,g)==0)////POWER ON
   {
    ton=1;
    toff=0;
    y=1;
   }

   if(strcmp(msg,h)==0)////POWER OFF
   {
    ton=0;
    toff=1;
    z=1;
   }
   
  }
  delay(1000);
 }

///////////////////////////////////////////

 if(off==1)////DISCONNECTING
 {
  digitalWrite(8,0);
  //delay(500);
  if(powerstatus==1)
  {
   if(z==1)
   {
    ackmsg("DISCONNECTED");
   }
   z=0;
  }
 }

 if(on==1)////RECONNECTING
 {
  digitalWrite(8,1);
  //delay(500);
  if(powerstatus==0)
  {
   if(y==1)
   {
    ackmsg("RECONNECTED");
   }
   y=0;
  }             
 }

 if(toff==1)////DISCONNECTING
 {
  digitalWrite(8,0);
  //delay(500);
  if(powerstatus==1)
  {
   if(z==1)
   {
    ackmsg("POWER OFF");
   }
   z=0;
  }
 }

 if(ton==1)////RECONNECTING
 {
  digitalWrite(8,1);
  //delay(500);
  if(powerstatus==0)
  {
   if(y==1)
   {
    ackmsg("POWER ON");
   }
   y=0;
  }             
 }
 
// delay(500);
//}
 
//////////////////////////////////////
 delay(500); 
}
