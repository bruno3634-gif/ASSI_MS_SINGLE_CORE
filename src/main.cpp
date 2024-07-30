#include <Arduino.h>
#include <ESP32_CAN.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <ESP2SOTA.h>

#define LED_PIN 16
#define NUM_OF_PINS 48

unsigned long tim = 0;
unsigned long timeout = 0;


const char* ssid = "ASSI_LART_T24";
const char* password = "T24E_aut";
WebServer server(80);


TWAI_Interface CAN1(1000, 21, 22);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_OF_PINS, LED_PIN, NEO_GRB + NEO_KHZ800);
int flag = 0;
int luz_state_3 = 0;
int luz_state_4 = 0;
int emergenencia = 0;
int missao = 0;
uint32_t CAN_ID = 0;
uint8_t var[8];


void setup() 
{
  /***        --- Initiate access point and configure static ip addresss as 06.14.22.24 ---         ***/

  WiFi.mode(WIFI_AP);  
  WiFi.softAP(ssid, password);
  IPAddress IP = IPAddress (06, 14, 22, 24);
  IPAddress NMask = IPAddress (255, 255, 255, 0);
  IPAddress myIP = WiFi.softAPIP();
  WiFi.softAPConfig(IP, IP, NMask);
  pinMode(4,OUTPUT);
  digitalWrite(4,0);

/***          --- Initiate webserver  ---         ***/

  server.on("/display", HTTP_GET, []() {
    String message = "Missao: " + String(missao) + "\nEstado da Missao: " + String(flag);
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", message);
  });

  /* INITIALIZE ESP2SOTA LIBRARY */
  ESP2SOTA.begin(&server);
  server.begin();

  Serial.begin(115200); 
  strip.begin();
  strip.fill(strip.Color(0, 0, 0), 0, strip.numPixels());
  //strip.setBrightness(127);

}

void loop() 
{

  //tim = millis();
  server.handleClient();
  
  CAN_ID = CAN1.RXpacketBegin();
  
  if(CAN_ID == 0x502)
  {
    CAN1.RXgetDLC();


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
    flag = 1;
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
  //strip.fill(strip.Color(255, 0, 0), 0, strip.numPixels());
}

