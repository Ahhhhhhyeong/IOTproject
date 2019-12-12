#include <Wire.h>
#include <Adafruit_MLX90614.h>
///////SERVER INCLUDE/////////
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiUdp.h>

const char *ssid = "wifiID";   ////Wifi ID
const char *password = "password";   ////Wifi password

//////////MLX90614/////////////////
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


int Status = 12;   ///MLX90614
int sensor = 14;   ///PIR sensor
const int button = 16;   ///switch
int temp = 0;     /////btn

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(sensor, INPUT);
  pinMode(Status, OUTPUT);
  pinMode(button, INPUT);
  Serial.println("Adafruit MLX90614 test");  
  mlx.begin();  

////////SERVER//////////
  delay(10);

  Serial.printf("%d: Connecting to %d\n", millis(), ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.printf("\n%d: WiFi connected: %s\n", millis(), WiFi.localIP().toString().c_str());
}


void loop() {
  temp = digitalRead(button);
  long state = digitalRead(sensor);
 
  if(temp == LOW){ //push the button
    ///////SERVER CONNECTED///////
      WiFiClient client;
      String host = "petstart.herokuapp.com";
    
      bool connected = client.connect(host, 80);
    
      Serial.printf("%d: Connected - %d\n", millis(), connected);

    if(state == HIGH){ ////sensor detected
         digitalWrite(Status, HIGH);
         Serial.println("Motion detected!");
         Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
         Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
         Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
         Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");
          int temp1 = mlx.readObjectTempC();
         Serial.println();
         delay(1000);
        /////DATA INPUT/////
        if(temp1 >= 34){
        do
          {
            if (connected == false)
            {
              Serial.println("not success");
              break;
            }
        
            client.print(String("GET /") + "serverHost" + temp1 + " HTTP/1.1\r\n" + 
                    "Host: " + host + "\r\n" +
                    "Connection: close\r\n" + "\r\n");
        
            int readBytes = 0;
            while (client.connected() == 1)
            {
              if (client.available())
              {
                // 아래의 코드 영역만 원하는 용도에 맞게 재구성
               
                String line = client.readStringUntil('\n');
                readBytes += line.length();
              }
            }
        
            Serial.printf("%d: # of recv bytes: %d\n", millis(),readBytes);
          } while (false);
        }
        if(temp1 >= 44){
          client.stop();       
        }
      }
     else{ ////sensor absent
       digitalWrite(Status, LOW);
       Serial.println("Motion absent!");       
       delay(1000);
      }
  }
  else{
    Serial.println("Turn OFF");
    delay(1000);
  }
  
   
}
