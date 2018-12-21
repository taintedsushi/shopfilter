#include <Wire.h> 
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header


hd44780_I2Cexp lcd;
const int PM = 6;
const int RELAY = 3;
const int BUTTON = 9;
const int ratioCount = 4;

unsigned long sampleTime=10000;
unsigned long duration;
unsigned long startTime;
unsigned long lpo=0;
float threshold = 4.0;
float ratio = 0.0;
int ratioCounter = 0;

int fanMode = 2; // 3 States :: 0=OFF / 1=ON / 2=AUTO

const int LCD_COLS = 16;
const int LCD_ROWS = 2;
int buttonState = LOW;         // variable for reading the pushbutton status
unsigned long buttonTimer;
int longPressTime = 1000;
bool fanStatus = 0;
bool longPress = false;


void setup() {

  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.clear();
  lcd.print("   AUTO MODE     ");

  lcd.setCursor(0,1);
  lcd.print("Crud Level  --.-");

  Serial.begin(9600);
  
  pinMode(PM,INPUT);
  pinMode(BUTTON,INPUT);
  pinMode(RELAY,OUTPUT);

  startTime=millis();  // Start the timer
  /* // Make sure relay is off...that's right off.  
     // I know, it doesn't make sense.
     // Depends on if your relay
     // Change this to low if it seems you are in backward world
 */
  
  digitalWrite(RELAY,HIGH); 
  Serial.println(buttonState);
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonState = digitalRead(BUTTON);
  if (buttonState == HIGH) {
    Serial.println("Button Pressed");  
    longPress = false;
    buttonTimer = millis();
    while (buttonState == HIGH)  {
      buttonState = digitalRead(BUTTON);
      if ((millis() - buttonTimer) > longPressTime) {
        Serial.println(millis() - buttonTimer);
        fanMode = 2; // Back to Auto  
        lcd.setCursor(0 ,0);
        lcd.print("   AUTO MODE    ");
        longPress = true;
        buttonState = LOW;
        delay(1000);
        buttonTimer = millis();
      }
    }
    Serial.print("longPress  :  ");
    Serial.println(longPress);
    
    if (longPress == false) {
      lcd.setCursor(0,0);
      if (fanStatus == 0) {
        lcd.print(" ------> Fan On ");
        fanStatus = 1;
        fanMode = 1;
        digitalWrite(RELAY, LOW);

      } else {
        lcd.print(" Fan Off <------");
        fanStatus = 0;
        fanMode = 0;
        digitalWrite(RELAY, HIGH);

      }
    }
  }
  longPress=false;


  // DISPLAY CURRENT CRUD LEVEL 
  
  duration = pulseIn(PM, LOW);
  lpo=lpo+duration;
  if((millis() - startTime) > sampleTime) {
    ratio=lpo/(sampleTime*10.0);   
    lcd.setCursor(0,1);
    lcd.print("Crud Level ");
    lcd.setCursor(12,1);
    lcd.print("    ");
    lcd.setCursor(12,1);
    lcd.print(ratio);
    startTime=millis();
    lpo=0;
    if ((ratio > threshold) && (ratioCounter < ratioCount)) {
      ratioCounter++; 
    } 
    if ((ratio < threshold) && (ratioCounter >  0)) { 
      ratioCounter--;
    }

    // SET FAN LEVEL
    if ((fanMode == 2) && (ratioCounter == ratioCount)) {
      digitalWrite(RELAY,LOW); // Fan goes on
      fanStatus = 1;
    }  
    if ((fanMode == 2) && (ratioCounter == 0)) {
      digitalWrite(RELAY,HIGH); // Fan goes off
      fanStatus = 0;
    }
  }

}
