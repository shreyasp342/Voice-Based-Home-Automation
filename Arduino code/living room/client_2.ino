#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

#define CLIENT_1 2              //Dining Room  
#define SERVER_ADDRESS 1        //Bed Room
#define CLIENT_2 3              //Living Room

#define light_pin 3
#define fan_pin 4

RH_NRF24 driver;

RHReliableDatagram manager(driver, CLIENT_2);

void setup() 
{
  //Initialization
  Serial.begin(9600);
  rf_init();
  pinMode(light_pin, OUTPUT);
  pinMode(fan_pin, OUTPUT);
}

uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
uint8_t light = 0, fan = 0;
int i;

void loop()
{
  //Check to see if data is available at the Receiver Buffer
  if (manager.available())
  {
   
    uint8_t len = sizeof(buf);
    uint8_t from;
   
    //Checks if the receiver message is for the destined address
    if (manager.recvfromAck(buf, &len, &from))
    {
      //Print Received message on the Serial Monitor for Debugging purpose
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      
      uint8_t x = buf[0];
        uint8_t y = buf[1]; 
        uint8_t datt[10] = {9};
      
      if(x == '0' )         //light
      {
         light = 0;
      }
      else if ( x == '1')
      {
        light = 1;
      }
      else if ( x == '9')
      {
        if(light == 0)
        {
          x = '0';
        }
        else if(light == 1)
        {
          x = '1';
        }
      }
      else
      {
        x = '8';
      }
      
      if(y == '0' )           //fan
      {
         fan = 0;
      }
      else if ( y == '1' || y == '2' || y == '3' || y == '4' || y == '5' )
      {
        fan = 1;
      }
      else if ( y == '9')
      {
        if(fan == 0)
        {
          y = '0';
        }
        else if(fan == '1')
        {
          y = '1';
        }
      }
      else
      {
        y = '8';
      }
      
      
      digitalWrite(light_pin, light);
      Serial.print("light is ");
      if(light == 0)
      {
        Serial.println("OFF");
      }
      else if(light ==1)
      {
           Serial.println("ON");
      }
      
      digitalWrite(fan_pin, fan);
       Serial.print("fan is ");
      if(fan == 0)
      {
        Serial.println("OFF");
      }
      else if(fan == 1)
      {
           Serial.println("ON");
      }
      
      //Send a reply back to the originator Server
      datt[0] = x;
      datt[1] = y;
      
      if (!manager.sendtoWait(datt, sizeof(datt), from))
       Serial.println("Reply to SERVER failed");
      else
        Serial.println("Reply sent to SERVER");
    }
  }
}


void rf_init(void)              //nRF24L01+ initialization
{
  Serial.print("RF setup begin\r\n");
  
  if (!manager.init())
    Serial.println("RF communication initialization failed");
  else
    Serial.println("RF communication initialized");
    
  Serial.print("RF setup complete\r\n");
}
