 #include "ESP8266.h"

#include <RHReliableDatagram.h>
#include <RH_NRF24.h>
#include <SPI.h>

#define CLIENT_1 2                //Dining Room
#define SERVER_ADDRESS 1          //Bed Room
#define CLIENT_2 3                //Living Room

#define light_pin 30
#define fan_pin 32

RH_NRF24 driver(8, 7);           //  Arduino Mega

RHReliableDatagram manager(driver, SERVER_ADDRESS);
 
ESP8266 wifi(Serial1);

void setup(void)
{
    //Initialization
    Serial.begin(9600);
    rf_init(); //RF24 initialization
    wifi_init(); //WIFI initialization
    
}

void loop(void)
{
   uint8_t buffer[1024] = {0};
   uint8_t msg[1024]={"HTTP/1.1 200 OK\n\rContent-Type: text/html; charset=utf-8\n\rContent-Length: length\n\r\n\r"}; //Response to HTTP POST request
   uint32_t len_msg = sizeof(msg);
   uint8_t mux_id;
    
   uint8_t MyArray[10] = {0};
   uint8_t data_1[10];
   uint8_t data_2[10];
   uint8_t data_3[10];
   
   //Receive any message from the WiFi if any message is available 
   uint32_t len = wifi.recv(&mux_id, buffer, sizeof(buffer), 100);
   uint32_t len_buffer = sizeof(buffer);
    if (len > 0)
    {
        //Display the received message on the Serial Monitor for Verification and Debugging
        Serial.print("Status:[");
        Serial.print(wifi.getIPStatus().c_str());
        Serial.println("]");
        
        Serial.print("Received from :");
        Serial.print(mux_id);
        Serial.print("[");
        for(uint32_t i = 0; i < len; i++)
        {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
        
        //Extract the required data for the received message by removing the headers
        for (int j = 0; j < len_buffer; j++)
        {
          if(buffer[j] == '$')
          {
            j++;
            int k=0;
            while( buffer[j] != '$')
            {
              MyArray[k]=buffer[j];
              k++;
              j++;
            }
          }
        }
        
         //Display the received message on the Serial Monitor
         Serial.println("MyArray is");
         Serial.println((char*)MyArray);
 
         //Split the received data for the different stations
         data_1[0]=MyArray[0];
         data_1[1]=MyArray[1];
         data_2[0]=MyArray[2];
         data_2[1]=MyArray[3];
         data_3[0]=MyArray[4];
         data_3[1]=MyArray[5];
 
         Serial.println("data_1 is");
         Serial.println((char*)data_1);
         Serial.println("data_2 is");
         Serial.println((char*)data_2);
         Serial.println("data_2 is");
         Serial.println((char*)data_3);
         
         //Perform switching actions related to this station
         swi(data_1);
 
         //Communicate with the client modules with their respective data
         Serial.println("communicating with client 1");
          comm(data_2, CLIENT_1);
         Serial.println("done cwith client 1");
         Serial.println("communicating with client 2");
           comm(data_3, CLIENT_2);
         Serial.println("done client 2");
  
         //Send acknoledgement back using Wifi
         if(wifi.send(mux_id, msg, len_msg))
         {
            Serial.print("send back ok\r\n");
         }
         else
         {
            Serial.print("send back err\r\n");
         }
       
         //Release socket at TCP Port
         if (wifi.releaseTCP(mux_id))
         {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
          } 
          else 
          {
            Serial.print("release tcp");
            Serial.print(mux_id);
            Serial.println("err");
          }
        
        Serial.print("Status:[");
        Serial.print(wifi.getIPStatus().c_str());
        Serial.println("]");
    }
}

uint8_t light = 0, fan = 0;

void swi(uint8_t dat[])
{
        uint8_t x = dat[0];
        uint8_t y = dat[1]; 
        uint8_t datt[10] = {9};
      
      if(x == '0' )           //light
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
      
      if(y == '0' )             //fan
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
        else if(fan == 1)
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
      
      datt[0] = x;
      datt[1] = y;
}


//WiFi Initilization Procedure
void wifi_init(void)
{
  Serial.print("WIFI setup begin\r\n");
    
    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());
      
    if (wifi.setOprToStationSoftAP())
    {
        Serial.print("to station + softap ok\r\n");
    } 
    else
    {
        Serial.print("to station + softap err\r\n");
    }
 
    if (wifi.joinAP("shreyas","qwertyuio"))
    {
        Serial.print("Join AP success\r\n");
        Serial.print("IP: ");
        Serial.println(wifi.getLocalIP().c_str());    
    }
    else
    {
        Serial.print("Join AP failure\r\n");
    }
    
    if (wifi.enableMUX())
    {
        Serial.print("multiple ok\r\n");
    }
    else
    {
        Serial.print("multiple err\r\n");
    }
    
    if (wifi.startTCPServer(8090))
    {
        Serial.print("start tcp server ok\r\n");
    }
    else 
    {
        Serial.print("start tcp server err\r\n");
    }
    
    if (wifi.setTCPServerTimeout(10)) 
    { 
        Serial.print("set tcp server timout 10 seconds\r\n");
    }
    else
    {
        Serial.print("set tcp server timout err\r\n");
    }
    
    Serial.print("WIFI setup end\r\n");
}

uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
#define x 6

//Procedure for Communication with the Clients
void comm(uint8_t data[], uint8_t ADDRESS)
{
  Serial.print("Sending to ");
  Serial.println(ADDRESS, HEX);
    
  // Send a message to manager_server
  if (manager.sendtoWait(data, sizeof(data), ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    
    if (manager.recvfromAckTimeout(buf, &len, 5000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      //Staatus of the devices connected to the Client modules
      uint8_t a = buf[0];  
      uint8_t b = buf[1];
      
      Serial.print("light is ");
      if(a == '0')
      {
        Serial.println("OFF");
      }
      else if(a == '1')
      {
           Serial.println("ON");
      }
      
      
       Serial.print("fan is ");
      if(b == '0')
      {
        Serial.println("OFF");
      }
      else if(b == '1')
      {
           Serial.println("ON");
      }
    }
    else
    {
      Serial.println("No reply, is nrf24_reliable_datagram_server running?");
    }
  }
  else
    Serial.println("sending failed"); 
}


//nRF24L01+ Initialization Procedure
void rf_init(void)
{
  Serial.print("RF setup begin\r\n");
  
  if (!manager.init())
    Serial.println("RF communication initialization failed");
  else
    Serial.println("RF communication initialized");
    
  Serial.print("RF setup complete\r\n");
}
