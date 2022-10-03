#include <WiFi.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>
#define ON_Board_LED 2 

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif


/* WiFi network name and password */
// Your wirelless router ssid and password


SoftwareSerial NMEA(36,15);
SoftwareSerial ThirdPty(35,4);

const char * ssid = "NATA"; 
const char * pwd = "11Januari";
WiFiUDP udp;

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;
TaskHandle_t Task4;


// ethernet config
const IPAddress local_IP(192, 168, 1, 98);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);
const IPAddress primaryDNS(8, 8, 8, 8);
const IPAddress secondaryDNS(8, 8, 4, 4);

//UDP server 
const char * udpAddress = "192.168.1.4"; // your pc ip
const int udpPort = 8082; //port server


boolean parsing = false;
boolean state_anotasi = false;
String dt[20];
String data;
String header;
char packetBuffer[50];
float d_head;
float depth_L;
float depth_H;
float d_win;
long ping_id = 0;


//unsigned long ping_id = 0;
String cmd,cmd1;
String depth_cor;
char Depth_f[15], Depth_M[15], Depth_F[15];
char Temp_H[15], Temp_L[15], Temp_C[15];
char Depth_M_H[15], Depth_M_L[15];
float float_Depth_M_H, float_Depth_M_L;
//float depth_cc, depth, depth_H, depth_L;
float temp_dbt, temp_xdr;

#define BUFFER_SIZE 1024
//create UDP instance
byte buff[BUFFER_SIZE];

//char packetBuffer[50];
String strCon;


void setup(){
  pinMode(ON_Board_LED,OUTPUT);
  digitalWrite(ON_Board_LED,HIGH);
  Serial.begin(38400); // to monitor activity
  NMEA.begin(38400); // to monitor activity
  ThirdPty.begin(38400); // to monitor activity
  // init wifi connection
if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
}
  //Connect to the WiFi network
   WiFi.begin(ssid, pwd);
   while (WiFi.status() != WL_CONNECTED) {
   delay(500);
}
 udp.begin(udpPort); 
  
}

void loop(){
  int size = 0;
  serialEvent1();
  UDPtoSer();
  if( parsing ){
  parsingdata();
  data ="";
  parsing = false;
 }
   if(packetBuffer[0] == 'M'){ 
    
     if((size=ThirdPty.available())){
      ThirdPty.readBytes(buff, size);
      udp.beginPacket(udpAddress, udpPort);
      udp.write(buff,size);    
      udp.endPacket();
      memset(buff, 0, 50);
    }
      ping_id = 0-1;
  }   


}
void serialEvent1() {
  if (Serial.available()>0) {
    char inChar = (char)Serial.read();
    data += inChar;
    if (inChar == '\n') {
      parsing = true;
    }
  }
}

void parsingdata()
{
  
  int j = 0;
  int k = 0;
  dt[j] = "";
  int minLength;
  if (data.length() >= 70) {
    minLength = 20;
  }
  else                      {
    minLength = 0;
  }
  for (k = 1; k < data.length() - minLength ; k++) {
    if ((data[k] == '$') || (data[k] == ',' ) || (data[k] == '*' ) ) {
      j++;
      dt[j] = "";
    }
    else {
      dt[j] = dt[j] + data[k];
    }
  }
  if (dt[0]=="SDDBT") {
      ping_id++;
      
      cmd = "AN," + String(ping_id) + "*";
      udp.beginPacket(udpAddress, udpPort);
      udp.println(cmd);  
      udp.endPacket();
      Serial.flush();
      
  }
  else if ( dt[0] == "SDXDR" ){
      ping_id++;
      cmd1 = "AN," + String(ping_id) + "*";
      udp.beginPacket(udpAddress, udpPort);
      udp.println(cmd1);  
      udp.endPacket();
      Serial.flush();
  }
}
void UDPtoSer()
{
    int packetSize = udp.parsePacket();
    if (packetSize)
    {
      IPAddress remoteIp = udp.remoteIP();
      int len = udp.read(packetBuffer, 255);
      if (len > 0) packetBuffer[len] = 0;
      strCon = packetBuffer;
      Serial.print(packetBuffer);     
    }
}
