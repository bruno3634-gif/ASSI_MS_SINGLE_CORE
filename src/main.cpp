#include <Arduino.h>
#include <ESP32_CAN.h>
#include <Adafruit_NeoPixel.h>


#define LED_PIN 16
#define NUM_OF_PINS 48

#define BTN 34
#define Accelaration 4 
#define Skidpad 27
#define Autocross 5
#define Trackdrive 17
#define EBS_test 33
#define Inspection 23


unsigned long tim = 0;
unsigned long timeout = 0;
unsigned long timeout_interrupt = 0;
volatile int flag_interrupt = 0;

TWAI_Interface CAN1(1000, 21, 22);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_OF_PINS, LED_PIN, NEO_GRB + NEO_KHZ800);
int flag = 0;
int luz_state_3 = 0;
int luz_state_4 = 0;
int emergenencia = 0;
volatile int missao = 0;
uint32_t CAN_ID = 0;
uint8_t var[8];


void IRAM_ATTR ms_interrupt()
{
  flag_interrupt = 1;
  timeout_interrupt = millis();
}



void setup() 
{
  pinMode(BTN,INPUT);
  Serial.begin(115200);
  strip.begin();
  strip.fill(strip.Color(0, 0, 0), 0, strip.numPixels());
  //strip.setBrightness(127);
  attachInterrupt(BTN,ms_interrupt,FALLING);
}

void loop() 
{
  //tim = millis();
  
  CAN_ID = CAN1.RXpacketBegin();
  
  if(CAN_ID == 0x502)
  {
    //CAN1.RXgetDLC();


    uint8_t CAN_DLC = CAN1.RXgetDLC();


    for (int i = 0; i < CAN1.RXgetDLC(); i++)
    { // Function RXgetDLC() returns aboves frame DLC
      Serial.println("     ");
      var[i] = CAN1.RXpacketRead(i);
      Serial.print(var[i]);
      
      
    }
    flag = var[0];
    Serial.println(flag);
  }
  strip.clear(); 
  switch (flag)
  {
  case 1:
    emergenencia = 0;
    strip.fill(strip.Color(0, 0, 0), 0, strip.numPixels());
    strip.show();
    break;
  case 2:
    emergenencia = 0;
    strip.fill(strip.Color(150, 150, 0), 0, strip.numPixels());
    delay(50);
    strip.show();
    break;
  case 3:
    emergenencia = 0;
    if(luz_state_3 == 0)
      {
        if(timeout + 1000 <= millis())
        {
          strip.fill(strip.Color(150, 150, 0), 0, strip.numPixels());
          delay(50);
          strip.show(); 
          luz_state_3 = 1; 
          timeout = millis();
        }
      }
    else
      {
        if(timeout + 1000 <= millis())
        {
          strip.fill(strip.Color(0, 0, 0), 0, strip.numPixels());
          delay(50);
          strip.show(); 
          luz_state_3 = 0; 
          timeout = millis();
        }
      }
      break;
  case 4:
      emergenencia = 1;
      if(luz_state_4 == 0)
      {
        if(timeout + 1000 <= millis())
        {
          strip.fill(strip.Color(0, 0, 255), 0, strip.numPixels());
          delay(50);
          strip.show(); 
          luz_state_4 = 1; 
          timeout = millis();
        }
      }
      else
      {
        if(timeout + 1000 <= millis())
        {
          strip.fill(strip.Color(0, 0, 0), 0, strip.numPixels());
          delay(50);
          strip.show(); 
          luz_state_4 = 0; 
          timeout = millis();
        }
      }
      break;
    case 5:
      emergenencia = 0;
      strip.fill(strip.Color(0, 0, 255), 0, strip.numPixels());
      delay(50);
      strip.show(); 
      break;
  
  default:
    emergenencia = 0;
    strip.fill(strip.Color(0, 0, 0), 0, strip.numPixels());
    delay(50);
    strip.show();
    break;
  }
  if(tim + 500 <= millis())
  {
    CAN1.TXpacketBegin(0x50,0);
    CAN1.TXpacketLoad(emergenencia);
    CAN1.TXpacketLoad(missao);
    CAN1.TXpackettransmit();
    tim = millis();
  }

  if(flag_interrupt == 1 && millis()>= timeout_interrupt + 100)
  {
    if(digitalRead(BTN) == 0)
    {
      missao++;
      if(missao < 0 || missao > 6)
      {
        missao = 0;
      }
      flag_interrupt = 0;
    }
  }


  switch (missao)
  {
  case 0:
    digitalWrite(Accelaration, LOW);
    digitalWrite(Skidpad, LOW);
    digitalWrite(Autocross, LOW);
    digitalWrite(Trackdrive, LOW);
    digitalWrite(EBS_test, LOW);
    digitalWrite(Inspection, LOW);
    break;
  case 1:
    digitalWrite(Accelaration, HIGH);
    digitalWrite(Skidpad, LOW);
    digitalWrite(Autocross, LOW);
    digitalWrite(Trackdrive, LOW);
    digitalWrite(EBS_test, LOW);
    digitalWrite(Inspection, LOW);
    break;
  case 2:
    digitalWrite(Accelaration, LOW);
    digitalWrite(Skidpad, HIGH);
    digitalWrite(Autocross, LOW);
    digitalWrite(Trackdrive, LOW);
    digitalWrite(EBS_test, LOW);
    digitalWrite(Inspection, LOW);
    break;
  case 3:
    digitalWrite(Accelaration, LOW);
    digitalWrite(Skidpad, LOW);
    digitalWrite(Autocross, HIGH);
    digitalWrite(Trackdrive, LOW);
    digitalWrite(EBS_test, LOW);
    digitalWrite(Inspection, LOW);
    break;
  case 4:
    digitalWrite(Accelaration, LOW);
    digitalWrite(Skidpad, LOW);
    digitalWrite(Autocross, LOW);
    digitalWrite(Trackdrive, HIGH);
    digitalWrite(EBS_test, LOW);
    digitalWrite(Inspection, LOW);
    break;
  case 5:
    digitalWrite(Accelaration, LOW);
    digitalWrite(Skidpad, LOW);
    digitalWrite(Autocross, LOW);
    digitalWrite(Trackdrive, LOW);
    digitalWrite(EBS_test, HIGH);
    digitalWrite(Inspection, LOW);
    break;
  case 6:
    digitalWrite(Accelaration, LOW);
    digitalWrite(Skidpad, LOW);
    digitalWrite(Autocross, LOW);
    digitalWrite(Trackdrive, LOW);
    digitalWrite(EBS_test, LOW);
    digitalWrite(Inspection, HIGH);
    break;
  default:
    digitalWrite(Accelaration, LOW);
    digitalWrite(Skidpad, LOW);
    digitalWrite(Autocross, LOW);
    digitalWrite(Trackdrive, LOW);
    digitalWrite(EBS_test, LOW);
    digitalWrite(Inspection, LOW);
    break;
  }
  Serial.println(missao);
  
}

