#include <WiFi.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>

#include "WebServer_WT32_ETH01.h"

#define DEBUG_ETHERNET_WEBSERVER_PORT       Serial
#define ETH_CLK_MODE_ETH_CLOCK_GPIO0_IN
#define _ETHERNET_WEBSERVER_LOGLEVEL_       3

SoftwareSerial NMEA(36,15);

const char * ssid = "NATA"; 
const char * pwd = "11Januari";


// ethernet config
const IPAddress local_IP(192, 168, 1, 98);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);
const IPAddress primaryDNS(8, 8, 8, 8);
const IPAddress secondaryDNS(8, 8, 4, 4);
  
// IP address to send UDP data to.
// it can be ip address of the server or 
// a network broadcast address
// here is broadcast address
const char * udpAddress = "192.168.1.4"; // your pc ip
const int udpPort = 8081; //port server
const int udpPort1 = 8082; //port server



#define BUFFER_SIZE 1024
//create UDP instance
WiFiUDP udp, udp1;
byte buff[BUFFER_SIZE];


TaskHandle_t Task1;
TaskHandle_t Task2;

const int led_1 = 2;
const int led_2 = 23;

void setup() {
  Serial.begin(38400); 
  NMEA.begin(38400);
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  WT32_ETH01_onEvent();
  ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);
  ETH.config(local_IP, gateway, subnet, primaryDNS);
  WT32_ETH01_waitForConnect();
//  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
//      // debug_log("Failed to configure network settings");
//}
//   WiFi.begin(ssid, pwd);
// while (WiFi.status() != WL_CONNECTED) {
//       //debug_log("connecting to WiFi network");
//       delay(500);
//}
  udp.begin(udpPort);
  udp1.begin(udpPort1);
  
  xTaskCreatePinnedToCore(Task1code,"Task1",10000,NULL,1,&Task1,0);                         
  //delay(500); 
  xTaskCreatePinnedToCore(Task2code,"Task2",10000,NULL,1,&Task2,1);          
  //delay(500);
}

void Task1code( void * parameter ){


  for(;;){
    int size =0;
     while((size=NMEA.available()))
     {
      size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
      NMEA.readBytes(buff, size);
      udp1.beginPacket(udpAddress, udpPort1);
      udp1.write(buff,size);    
      udp1.endPacket();
      memset(buff, 0, 50);  
   
    }   
    vTaskDelay(100);
  } 
}

void Task2code( void * parameter ){

  for(;;){
     int size =0;
     while((size=Serial.available()))
     {
      size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
      Serial.readBytes(buff, size);
      udp.beginPacket(udpAddress, udpPort);
      udp.write(buff,size);    
      udp.endPacket();
      memset(buff, 0, 50);  
   
    }
    vTaskDelay(100);
  }
}

void loop() {

}
