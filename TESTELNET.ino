#include <WiFi.h>
  
// debug log, set to 1 to enable
#define ENABLE_DEBUG_LOG 0
  
// wifi config
const char* ssid     = "NATA";
const char* password = "11Januari";
  
// ethernet config
const IPAddress local_IP(192, 168, 1, 98);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 1);
const IPAddress primaryDNS(8, 8, 8, 8);
const IPAddress secondaryDNS(8, 8, 4, 4);
  
// rs-server config
const int serverPort = 1234;
const int serverPort1 = 1235;
  
// rs port config
const int baudrate = 115200;
const int rs_config = SERIAL_8N1;
  
// reading buffor config
#define BUFFER_SIZE 1024
  
// global objects
WiFiServer server, server1;
byte buff[BUFFER_SIZE];
  
void debug_log(char* str) {
#if ENABLE_DEBUG_LOG == 1
   Serial.println(str);
#endif
}
  
void setup() {
// init rs port
   Serial.begin(baudrate, rs_config);
   Serial2.begin(baudrate, rs_config);
   
  
// init wifi connection
if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
       debug_log("Failed to configure network settings");
}
   WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
       debug_log("connecting to WiFi network");
       delay(500);
}
  
#if ENABLE_DEBUG_LOG == 1
   Serial.println("connected to WiFi");
   Serial.println("IP adddr: ");
   Serial.println(WiFi.localIP());
#endif
   delay(1000);
  
//start server
   server = WiFiServer(serverPort);
   server1 = WiFiServer(serverPort1);
   server.begin();
   server1.begin();
   delay(1000);
   debug_log("server started");
}


void loop(){

Satu();
Dua();
  
}
void Satu() {
// wait for client
   WiFiClient client = server.available();
if (!client)
return;
  
   debug_log("client found");
while (client.connected()) {
int size = 0;
  
// read data from wifi client and send to serial
// while ((size = client.available())) {
//            size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
//            client.read(buff, size);
//            Serial.write(buff, size);
//            Serial.flush();
// }
  
// read data from serial and send to wifi client
while ((size = Serial.available())) {
           size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
           Serial.readBytes(buff, size);
           client.write(buff, size);
           client.flush();
}
}
   debug_log("client disconnected");
   client.stop();
}

void Dua() {
// wait for client
   WiFiClient client1 = server1.available();
if (!client1)
return;
  
   debug_log("client found");
while (client1.connected()) {
int size = 0;
  
// read data from wifi client and send to serial
while ((size = client1.available())) {
           size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
           client1.read(buff, size);
           Serial2.write(buff, size);
           Serial2.flush();
}
  
// read data from serial and send to wifi client
while ((size = Serial2.available())) {
           size = (size >= BUFFER_SIZE ? BUFFER_SIZE : size);
           Serial2.readBytes(buff, size);
           client1.write(buff, size);
           client1.flush();
}
}
   debug_log("client disconnected");
   client1.stop();
}
