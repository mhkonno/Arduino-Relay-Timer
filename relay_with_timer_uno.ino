// EEPROM - Version: Latest 
#include <EEPROM.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header
hd44780_I2Cexp lcd; 
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

char line0[17];
char line1[17];
int seconds;
int minutes; 
int hours;
int offtimemin;
int ontimesec;
int nextmin;
int nextsec;

int OffIntervalUpButton = 6;
int OffIntervalDownButton = 5;
int OnIntervalUpButton = 8;
int OnIntervalDownButton = 7;
int SprayButton = 4;
int ResetButton = 3;
int StartStop = 2;

int Valve1 = 11;
int Valve2 = 12;
int outputs [] = {Valve1, Valve2};
int inputs [] = {OffIntervalUpButton, OffIntervalDownButton, OnIntervalUpButton, OnIntervalDownButton,
                  SprayButton, ResetButton, StartStop};
//const int pin_BL = 10; 
unsigned long previousMillis = 0;   
long lcdOnTime = 300;
long lcdOffTime = 300; 
int lcdblink = 1;


int EEPROM_OFF_INTERVAL_ADDRESS = 0;
int EEPROM_ON_INTERVAL_ADDRESS = 1;

long OffIntervalSteps = 60000;
long OnIntervalSteps = 1000;
int ButtonPressWait = 250;
long OffIntervalUpBuffer;
long OffIntervalDownBuffer;
long OnIntervalUpBuffer;
long OnIntervalDownBuffer;

unsigned long  LastSpray;
unsigned long  OffTime;
unsigned long  OnTime;

void setup(void) {
  // Enable this code and push program once on the first run of a new board or to move EEPROM registers.
  // Then disable and push the program to the board again for normal operation.
  //EEPROM.write(EEPROM_OFF_INTERVAL_ADDRESS, 15);
  //EEPROM.write(EEPROM_ON_INTERVAL_ADDRESS, 3);
 OffTime = EEPROM.read(EEPROM_OFF_INTERVAL_ADDRESS) * 60000;
 OnTime = EEPROM.read(EEPROM_ON_INTERVAL_ADDRESS) * 1000;
  
  unsigned long LastSpray = millis();
 for (int i = 0; i < 7; i++) {
pinMode(inputs[i], INPUT);
digitalWrite(inputs[i], HIGH);
}
for (int o = 0; o < 2; o++) {
pinMode(outputs[o], OUTPUT);
digitalWrite(outputs[o], HIGH);
}
int status;
  status = lcd.begin(LCD_COLS, LCD_ROWS);
  if(status) // non zero status means it was unsuccesful
  {
    status = -status; // convert negative status value to positive number

    // begin() failed so blink error code using the onboard LED if possible
    hd44780::fatalError(status); // does not return
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("==Starting...==");
  lcd.setCursor(8, 1);
  lcd.print("By Hideo");
  delay(1000);
  lcd.clear();
}}



void loop(void) 
{
unsigned long currentMillis = millis();



if (digitalRead(StartStop) == HIGH) 
   {
    LastSpray = millis();
    lcd.setCursor(0, 0);
    lcd.print("     STOPPED    ");
       lcd.setCursor(0, 1);
    lcd.print("                ");
     {digitalWrite(Valve1, HIGH);}
     {digitalWrite(Valve2, HIGH);}
     lcd.noBacklight();
   //  digitalWrite(pin_BL, LOW);
  //   pinMode(pin_BL, INPUT);
  //   pinMode(pin_BL, OUTPUT);
   }
if (digitalRead(StartStop) == LOW) 
    
      {
       lcd.backlight();
  minutes = seconds / 60;
  hours = minutes / 60;
  offtimemin = OffTime / 1000 / 60;
  ontimesec = (OnTime / 1000);
  nextmin =(((LastSpray + OffTime) -  millis()) / 1000 / 60);
  nextsec =((((LastSpray + OffTime) - millis()) / 1000 ) % 60);
  sprintf(line1, "%02dm  %02ds  %02dm%02ds", offtimemin, ontimesec,nextmin, nextsec);
  
  lcd.setCursor(0, 0);
  lcd.print("Off: On: NextIn:");
  
  lcd.setCursor(0, 1);
  lcd.print(line1);
  
   
      }   
   
  
  
 // lcd_key = read_LCD_buttons(); 
  if (digitalRead(ResetButton) == LOW) 
        {LastSpray = millis();
         valveoff();}
  
  if (digitalRead(SprayButton) == LOW) 
        { Spray();
}
  else 
         {  valveoff();
         }

 
  if (millis() - LastSpray >= OffTime) 
        {Spray();
         LastSpray = millis();}


  if (digitalRead(OffIntervalUpButton) == LOW) {
    OffIntervalUp();
  }
  
  if (digitalRead(OffIntervalDownButton) == LOW) {
    OffIntervalDown();
  }
  
  if (digitalRead(OnIntervalUpButton) == LOW) {
    OnIntervalUp();
  }
  
  if (digitalRead(OnIntervalDownButton) == LOW) {
    OnIntervalDown();

  }
}
void valveon(void) 
    {digitalWrite(Valve1, LOW);
    digitalWrite(Valve2, LOW);}
void valveoff(void) 
    {digitalWrite(Valve1, HIGH);
    digitalWrite(Valve2, HIGH);}


void Spray(void) 
{
    valveon();
    long SprayBuffer = millis() + OnTime;
    while (millis() < SprayBuffer) {
        unsigned long currentMillis = millis();
        if (digitalRead(StartStop) == HIGH || digitalRead(ResetButton) == LOW) 
            break;

        if((lcdblink==1) && (currentMillis - previousMillis >= lcdOnTime))
        {
        lcd.setCursor(0, 0);
        lcd.print("    SPRAY  ON    "); 
        previousMillis = currentMillis;
        lcdblink=0; 
        } 
        else if((lcdblink==0) && (currentMillis - previousMillis >= lcdOffTime))
        {
        lcd.setCursor(0, 0);
        lcd.print("               "); 
        previousMillis = currentMillis; 
        lcdblink=1;
        }
        sprintf(line1, "Will stop in %02ds     ", (SprayBuffer -  millis()) / 1000 );
        lcd.setCursor(0, 1);
        lcd.display();
        lcd.print(line1);
        }
     valveoff();

} 
  

void OffIntervalUp(void) {
  if (OffTime < 1200000) {
    if (OffIntervalUpBuffer + ButtonPressWait < millis()) {
      OffTime += OffIntervalSteps;
      EEPROM.write(EEPROM_OFF_INTERVAL_ADDRESS, OffTime / 60000);
      OffIntervalUpBuffer = millis();
      lcd.clear();
    }}}
void OffIntervalDown(void) {
  if (OffTime > 60000) {
    if (OffIntervalDownBuffer + ButtonPressWait < millis()) {
      OffTime -= OffIntervalSteps;
      EEPROM.write(EEPROM_OFF_INTERVAL_ADDRESS, OffTime / 60000);
      OffIntervalDownBuffer = millis();
      lcd.clear();
    }}}
void OnIntervalUp(void) {
  if (OnTime < 60000) {
    if (OnIntervalUpBuffer + ButtonPressWait < millis()) {
      OnTime += OnIntervalSteps;
      EEPROM.write(EEPROM_ON_INTERVAL_ADDRESS, OnTime / 1000);
      OnIntervalUpBuffer = millis();
      lcd.clear();
    }}}
void OnIntervalDown(void) {
  if (OnTime > 1000) {
   if (OnIntervalDownBuffer + ButtonPressWait < millis()) {
      OnTime -= OnIntervalSteps;
      EEPROM.write(EEPROM_ON_INTERVAL_ADDRESS, OnTime / 1000);
      OnIntervalDownBuffer = millis();
      lcd.clear();
    }}}
