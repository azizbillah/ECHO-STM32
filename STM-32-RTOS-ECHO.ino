
#include <MapleFreeRTOS900.h>

// Global Variable
String ENVHF_string        = "";
String ENVLF_string        = "";
boolean ENVHF_string_ready  = false;
boolean ENVLF_string_ready  = false;
/*----------------------------------*/

/*Echo Parameter */
const byte buff_len = 90;
char    CRCbuffer[buff_len];

char    char_draft[100];
char    char_sow[100];
char    char_dfilter[100];
char    char_sfilter[100];
char    char_dblank[100];
int     range = 0;
String  cmd;

// Led status
#define led_dbt             PB7
#define led_stn             PB8
#define led_xdr             PB9
#define state_Anotasi_pin   PB5
#define led_init            PB6
#define led_ready           PA11

// SerialPort

#define Serial_HF Serial2
#define Serial_LF Serial3
#define Serial_NMEA Serial1
// Echosounder setting
float   draft = 0;
int     sow = 0;
int     dfilter = 4;;
int     sfilter = 4;;
int     dblank = 20;

// anotasi
int     flag_anotasi = 0;
int     flag_input = 0;
String  data;
/* -------------------- */


void setup() {
  pinMode(led_dbt, OUTPUT);
  pinMode(led_stn, OUTPUT);
  pinMode(led_xdr, OUTPUT);
  pinMode(state_Anotasi_pin, OUTPUT);
  pinMode(led_init, OUTPUT);
  pinMode(led_ready, OUTPUT);
  digitalWrite(led_dbt, HIGH);
  digitalWrite(led_stn, HIGH);
  digitalWrite(led_xdr, LOW);
  digitalWrite(led_init, HIGH);
  digitalWrite(led_ready, LOW);
  Serial_NMEA.begin(38400);
  Serial_HF.begin(921600);
  Serial_LF.begin(921600);
  delay(2000);

  //Setting awal
  Serial_NMEA.println("$PAMTC,EN,L*0C"); delay(2000);
  Serial_NMEA.println("$PAMTC,OPTION,SET,PING,OFF*55"); delay(2000);
  Serial_NMEA.println("$PAMTC,OPTION,SET,OUTPUTMC,PING*0B"); delay(2000);
  Serial_NMEA.println("$PAMTC,OPTION,SET,SLAVE,ON*46"); delay(2000);
  Serial_NMEA.println("$PAMTC,OPTION,SET,SYNCMODE,MANUAL*14"); delay(2000);
  Serial_NMEA.println("$PAMTC,OPTION,SET,PULSESPP,2,6,21,24,S*54"); delay(2000);
  Serial_NMEA.println("$PAMTC,OPTION,SET,PULSESPP,2,10,50,60,M*7B"); delay(2000);
  Serial_NMEA.println("$PAMTC,OPTION,SET,RANGE,0,S*1A"); delay(2000);
  Serial_NMEA.println("$PAMTC,OPTION,SET,RANGE,0,M*04"); delay(2000);
  Serial_NMEA.println("$PAMTC,EEC,SUBSET,FULL*31");  delay(2000);
  Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,TS*0A"); delay(2000);
  Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B"); delay(2000);
  Serial_NMEA.println("$PAMTC,EN,ALL,0*1D\r\n");delay(2000);
  Serial_NMEA.println("$PAMTC,EN,XDRX,1,10*66"); delay(2000);
  Serial_NMEA.println("$PAMTC,EN,DPT,1,10*30");  delay(2000);
  Serial_NMEA.println("$PAMTC,EN,MTW,1,10*3E");delay(2000);
  Serial_NMEA.println("$PAMTC,EN,S*13\r\n");delay(2000); 
  digitalWrite(led_init, LOW);delay(2000);
  digitalWrite(led_ready,HIGH);delay(2000);
  
    // initialize the digital pin as an output:
    xTaskCreate(MainTask,
                "Task0",
                512,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);
    xTaskCreate(HFhandler,
                "Task1",
                512,
                NULL,
                tskIDLE_PRIORITY + 0,
                NULL);
   xTaskCreate(PChandler,
                "Task2",
                512,
                NULL,
                tskIDLE_PRIORITY + 0,
                NULL);
   vTaskStartScheduler();
}

void loop() {
    // Insert background code here
}



void edit_input(int c){

  if( c == 1 ){
      if        ( flag_anotasi == 0){ digitalWrite(state_Anotasi_pin,LOW);}
      else if   ( flag_anotasi == 1){ digitalWrite(state_Anotasi_pin,HIGH);}
  }

  else if ( c == 2){
      if        ( range == 1){Serial_NMEA.println("$PAMTC,OPTION,SET,RANGE,0,S*1A"); 
                              Serial_NMEA.println("$PAMTC,OPTION,SET,RANGE,0,M*04"); 
                              Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                              Serial_NMEA.println("$PAMTC,EN,S*13");}
      else if   ( range == 2){Serial_NMEA.println("$PAMTC,OPTION,SET,RANGE,1,S*1B"); 
                              Serial_NMEA.println("$PAMTC,OPTION,SET,RANGE,1,M*05"); 
                              Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                              Serial_NMEA.println("$PAMTC,EN,S*13");}
      else if   ( range == 3){Serial_NMEA.println("$PAMTC,OPTION,SET,RANGE,2,S*18"); 
                              Serial_NMEA.println("$PAMTC,OPTION,SET,RANGE,2,M*06"); 
                              Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                              Serial_NMEA.println("$PAMTC,EN,S*13");}
      else if   ( range == 4){Serial_NMEA.println("$PAMTC,OPTION,SET,RANGE,3,S*19"); 
                              Serial_NMEA.println("$PAMTC,OPTION,SET,RANGE,3,M*07"); 
                              Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                              Serial_NMEA.println("$PAMTC,EN,S*13");
                               }
      else if   ( range == 5){Serial_NMEA.println("$PAMTC,OPTION,SET,RANGE,4,S*1E"); 
                              Serial_NMEA.println("$PAMTC,OPTION,SET,RANGE,4,M*00"); 
                              Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                              Serial_NMEA.println("$PAMTC,EN,S*13");
                              }
  }
}

void outputMsg(String msg) {
  msg.toCharArray(CRCbuffer,sizeof(CRCbuffer));
  byte crc = convertToCRC(CRCbuffer);
  Serial_NMEA.print(msg);                //Serial.print(msg);
  if (crc < 16) {Serial_NMEA.print("0"); }//Serial.print("0");}
  Serial_NMEA.println(crc,HEX);          //Serial.println(crc,HEX);
}

byte convertToCRC(char *buff) {
  char c;
  byte i;
  byte start_with = 0,end_with = 0;
  byte crc = 0;

  for (i = 0; i < buff_len; i++) {
    c = buff[i];
    if(c == '$') {start_with = i;}
    if(c == '*') {end_with = i;}      
  }
  if (end_with > start_with){
    for (i = start_with+1; i < end_with; i++){ crc = crc ^ buff[i] ;}
  }
  else {//Serial_NMEA.println("CRC ERROR");
    return 0;
  }
  return crc;
}


/* RTOS TASK */

static void HFhandler(void *pvParameters)  // This is a task.
{
 
  for (;;) // A Task shall never return or exit.
  {
      if(Serial_HF.available() || Serial_LF.available() > 0){
      ENVHF_string = Serial_HF.readStringUntil('\n');           //read the string until we see a <CR>
      ENVLF_string = Serial_LF.readStringUntil('\n');           //read the string until we see a <CR>
      ENVHF_string_ready = true;
      ENVLF_string_ready = true;    
      }
      vTaskDelay(100); // wait for one second
  }
}
static void MainTask(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  for (;;) // A Task shall never return or exit.
  {
    if(ENVHF_string_ready && ENVLF_string_ready )
    {
      Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
      ENVHF_string        = "";
      ENVLF_string        = "";
      ENVHF_string_ready  = false;
      ENVLF_string_ready  = false;
    }
   vTaskDelay(100); // wait for one second
  }
}

static void PChandler ( void *pvParameters)
{
  for(;;){
  char buffer[100];
  while(Serial_NMEA.available())
  {
    char inChar = (char)Serial_NMEA.read();
   

    if                ( inChar == 'A') {  digitalWrite(led_dbt, HIGH);
                                          digitalWrite(led_stn, HIGH);
                                          digitalWrite(led_xdr, LOW);
                                          Serial_NMEA.println("$PAMTC,EN,ALL,0*1D\r\n");
                                          Serial_NMEA.println("$PAMTC,EN,XDRX,1,10*66");delay(500);
                                          Serial_NMEA.println("$PAMTC,EN,DPT,1,10*30"); delay(500);
                                          Serial_NMEA.println("$PAMTC,EN,S*13");delay(500);
                                          Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          }
    else if           ( inChar == 'B') {  digitalWrite(led_dbt, LOW);
                                          digitalWrite(led_stn, HIGH);
                                          digitalWrite(led_xdr, HIGH);
                                          Serial_NMEA.println("$PAMTC,EN,ALL,0*1D\r\n");
                                          Serial_NMEA.println("$PAMTC,EN,DBT,1,10*22"); delay(500);
                                          Serial_NMEA.println("$PAMTC,EN,DPT,1,10*30"); delay(500);
                                          Serial_NMEA.println("$PAMTC,EN,MTW,1,10*3E"); delay(500);
                                          Serial_NMEA.println("$PAMTC,EN,S*13");delay(500);
                                          Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          }
    else if           ( inChar == 'C') {  digitalWrite(led_dbt, HIGH);
                                          digitalWrite(led_stn, LOW);
                                          digitalWrite(led_xdr, HIGH);
                                          Serial_NMEA.println("$PAMTC,EN,ALL,0*1D\r\n");
                                          Serial_NMEA.println("$PAMTC,EN,XDRX,1,10*66"); delay(500);
                                          Serial_NMEA.println("$PAMTC,EN,DPT,1,10*30");  delay(500);
                                          Serial_NMEA.println("$PAMTC,EN,S*13"); delay(500);
                                          Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          }
    else if           ( inChar == 'D') {  draft=data.toFloat();
                                          data="";
                                          int c_draft = draft*1000;
                                          dtostrf(draft,3,2,char_draft);
                                          sprintf(buffer,"$PAMTC,OPTION,SET,DOFFSET,%d*",c_draft);
                                          cmd = buffer;
                                          outputMsg(cmd); delay(500);
                                          Serial_NMEA.println("$PAMTC,EN,S*13");
                                          Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          memset(CRCbuffer,0,sizeof(CRCbuffer)); //clear data
                                          }
    else if           ( inChar == 'E') {  sow=data.toInt();
                                          data="";
                                          String str = String(sow);
                                          str.toCharArray(char_sow,5);
                                          int c_sow = sow*10;
                                          sprintf(buffer, "$PAMTC,OPTION,SOSTW,%d*",c_sow);
                                          cmd= buffer;
                                          outputMsg(cmd); delay(500);
                                          Serial_NMEA.println("$PAMTC,EN,S*13"); delay(500);
                                          Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          memset(CRCbuffer,0,sizeof(CRCbuffer)); //clear data
                                          }  
    else if           ( inChar == 'F') {  dfilter=data.toInt(); 
                                          data="";
                                          String str = String(dfilter);
                                          str.toCharArray(char_dfilter,2);
                                          sprintf(buffer,"$PAMTC,OPTION,SET,DFILTER,%d*",dfilter);
                                          cmd = buffer;
                                          outputMsg(cmd); delay(500);
                                          Serial_NMEA.println("$PAMTC,EN,S*13");delay(500);
                                          Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          memset(CRCbuffer,0,sizeof(CRCbuffer)); //clear data
                                          }
    else if           ( inChar == 'G') {  sfilter=data.toInt();
                                          data = "";
                                          String str = String(sfilter);
                                          str.toCharArray(char_sfilter,2);
                                          sprintf(buffer,"$PAMTC,OPTION,SET,SFILTER,%d*",sfilter);
                                          cmd = buffer;
                                          outputMsg(cmd); delay(500);
                                          Serial_NMEA.println("$PAMTC,EN,S*13"); delay(500);
                                          Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          memset(CRCbuffer,0,sizeof(CRCbuffer)); //clear data 
                                          }
    else if           ( inChar == 'H') {  dblank=data.toInt();
                                          data="";
                                          char str_tenth[6];
                                          String str1 = String(dblank);
                                          str1.toCharArray(char_dblank,4);
                                          float float_tenth = (float)dblank/10;
                                          dtostrf(float_tenth, 3, 1, str_tenth);
                                          sprintf(buffer,"$PAMTC,OPTION,SET,DBLANK,%s*",str_tenth);
                                          cmd = buffer;
                                          outputMsg(cmd);delay(500);
                                          Serial_NMEA.println("$PAMTC,EN,S*13"); delay(500);
                                          Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,NOW*5B");
                                          memset(CRCbuffer,0,sizeof(CRCbuffer)); //clear data
    }    
    else if           ( inChar == 'K') {  flag_anotasi=data.toInt();
                                          edit_input(1);
                                          data="";    
                                          }    
    else if           ( inChar == 'M') {  range=data.toInt();
                                          edit_input(2);
                                          data="";}      
    else if           ( inChar == 'S') {
                                          Serial_NMEA.println("$PAMTC,OPTION,SET,SYNC,TS*0A"); delay(500);
                                          data="";
                                          }                                                                                                                   
    else{data.concat(inChar);}
    
    
    } 

    vTaskDelay(100); // wait for one second
  }
}
