/*
 * 
 */
int latchPin = 12;  // Latch pin (STCP腳位)
int clockPin = 13; // Clock pin (SHCP腳位)
int dataPin = 14;  // Data pin (DS腳位)
//int blankPin = 4;
String serialData;

/*
 * 
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

#ifndef STASSID
#define STASSID "238H8F"
#define STAPSK  "0920360270"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;
IPAddress ip;  
ESP8266WebServer server(80);
/*
 * 
 */
String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}
/*
 * 
 */
String body = " <!DOCTYPE html> "
"<html> "
"<title></title> "
"<head> </head> "
"<body> "
"<h1>Online HTML Editor</h1> "
"<p id='demo'></p> "
"</body> "
"</html>  "
"<script> "
" var x = setInterval(function() {    "
"  const now = new Date();   "
"  const current = now.getHours() + ':' + now.getMinutes() + ':' + now.getSeconds();   " 
"   document.getElementById('demo').innerHTML =  current;  "  
"   const Http = new XMLHttpRequest(); "
"  const url='/clock/'+current;   "
"  Http.open('GET', url);   "
"  Http.send();   "
"   if (distance < 0) {    " 
"    clearInterval(x);     "
"    document.getElementById('demo').innerHTML = 'EXPIRED';   "
"    } "
"    }, 300); "
"    </script> ";
 
/*
 * 
 */
void setup() 
{
  /*
   * 
   */
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");


  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  ip = WiFi.localIP();
  serialData = ipToString(ip);
  Serial.println(ip);

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on(F("/"), []() {
    server.send(200, "text/html", body);
  });

  server.on(UriBraces("/clock/{}"), []() {
    String user = server.pathArg(0);
    server.send(200, "text/plain", "");
    serialData = user;
  });

  server.on(UriRegex("^\\/users\\/([0-9]+)\\/devices\\/([0-9]+)$"), []() {
    String user = server.pathArg(0);
    String device = server.pathArg(1);
    server.send(200, "text/plain", "User: '" + user + "' and Device: '" + device + "'");
  });

  server.begin();
  Serial.println("HTTP server started");
  /*
   * 
   */ 
  digitalWrite(latchPin, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin, clockPin, MSBFIRST, 0);  //
  shiftOut(dataPin, clockPin, MSBFIRST, 0);  //
  shiftOut(dataPin, clockPin, MSBFIRST, 0);  //
  digitalWrite(latchPin, HIGH); // 送完資料後要把 latchPin 設成高電位
  
  // Set all the pins of 74HC595 as OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
//  pinMode(blankPin, OUTPUT);
}
void iv18_digit(int digit, int num)
{
  unsigned char byte1,byte2,byte3;
  byte3=0xff;

  int digit_convert_array[9]={12,19,13,18,14,17,16,15,11};
//  int digit_convert_array[9]={15,14,13,16,12,17,11,18,19};
//  int digit_convert_array[9]={18,11,17,12,16,13,14,15,19};
  byte1=0x10>>(20-digit_convert_array[digit]);
  byte2=0x100>>(16-digit_convert_array[digit]);
  byte2|=0x07;
//  #define dp 1<<7
//  #define a 1
//  #define b 1<<2
//  #define c 1<<5
//  #define d 1<<6
//  #define e 1<<4
//  #define f 1<<1
//  #define g 1<<3
  #define dp 1<<0
  #define a 1<<7
  #define b 1<<5
  #define c 1<<2
  #define d 1<<1
  #define e 1<<3
  #define f 1<<6
  #define g 1<<4
  int segment_convert_array[11]={a|b|c|d|e|f, b|c, a|b|g|e|d, 
  a|b|g|c|d, f|g|b|c, a|f|g|c|d, 
  a|f|g|c|d|e, a|b|c, a|b|c|d|e|f|g, a|b|c|d|f|g,0};
  byte3 = segment_convert_array[num];
  //byte3 = 0x80;
  digitalWrite(latchPin, LOW);  // 送資料前要先把 latchPin 設成低電位
  shiftOut(dataPin, clockPin, MSBFIRST, byte1);  //
  shiftOut(dataPin, clockPin, MSBFIRST, byte2);  //
  shiftOut(dataPin, clockPin, MSBFIRST, byte3);  //
  digitalWrite(latchPin, HIGH); // 送完資料後要把 latchPin 設成高電位
//  Serial.print(digit,HEX);Serial.print(" ");
//  Serial.print(digit_convert_array[digit],DEC);Serial.print(" ");
//  Serial.print(byte1,HEX);Serial.print(" ");
//  Serial.print(byte2,HEX);Serial.print(" ");
//  Serial.println(byte3,HEX);
}
void print_iv18(float num)
{
  char str[20];
  dtostrf(num, 1, 7, str);
  //sprintf(str,"%f",num);

  Serial.println(str);
}
int fetch_digit(long n,int i){
  int r;
  switch(i){
    case 0:
      r=(n/1L)%10;
    break;
    case 1:
      r=(n/10L)%10;
    break;
    case 2:
      r=(n/100L)%10;
    break;
    case 3:
      r=(n/1000L)%10;
    break;
    case 4:
      r=(n/10000L)%10;
    break;
    case 5:
      r=(n/100000L)%10;
    break;
    case 6:
      r=(n/1000000L)%10;
    break;
    case 7:
      r=(n/10000000L)%10;
    break;
    default:
    break;
  }
  return r;
}
unsigned  char i=0x0;
unsigned long tick = millis();
unsigned  long n=12345678;
  int digit_selector=7;
int high_zero_blank=1;
//
String resultData;
unsigned long string_shift_delay_time = 0;
int serialDataSubStart = 0;
void loop() 
{
  /*
   * 
   */
   server.handleClient();
   /*  
    *   
    */
   if (Serial.available() > 0) {
      // read the incoming byte:
      serialData = Serial.readStringUntil('\n');
      n=serialData.toInt();
   }
  int dgt;

  if(serialData.length()>0){
    /*
     *  serialData to seprate digit
     */
    
    if(string_shift_delay_time == 0 ) resultData = serialData;
    if((millis() - string_shift_delay_time) > 1500){
      string_shift_delay_time = millis();
     if(serialData.length()>8){
      if((serialDataSubStart + 8) > serialData.length()){
        serialDataSubStart = 0;
        resultData = "        ";
      }else{
        resultData = serialData.substring(serialDataSubStart);
        serialDataSubStart++;
      }
     }else{
      resultData = serialData;
     }
    }
    char ch = resultData.charAt(digit_selector);
    if(isDigit(ch)){
      dgt=String(ch).toInt();
      iv18_digit(7-digit_selector,dgt);
      delay(1);
    }else{
      iv18_digit(7-digit_selector,10);
    }
    tick = millis()+2000;
  }else{
    /*
     *  n to seprate digit
     */
    dgt=fetch_digit(n,digit_selector);
    if(high_zero_blank && dgt==0){
      iv18_digit(digit_selector,10); //blank
    }else{
      high_zero_blank=0;
      iv18_digit(digit_selector,dgt);
    }
      delay(1);
  }
//  Serial.print(i);
//  Serial.print(" ");
  digit_selector--;
  if(digit_selector<0){
//    Serial.println(" ");
    high_zero_blank=1;
    digit_selector=7;
  }
  
  if(millis()-tick < 1300){
    
    n=random(0,99999999);
//    n=12345678;
    Serial.println(n);
    Serial.print(fetch_digit(n,7));    
    Serial.print(" ");
    Serial.print(fetch_digit(n,6));    
    Serial.print(" ");
    Serial.print(fetch_digit(n,5));    
    Serial.print(" ");
    Serial.print(fetch_digit(n,4));    
    Serial.print(" ");
    Serial.print(fetch_digit(n,3));    
    Serial.print(" ");
    Serial.print(fetch_digit(n,2));    
    Serial.print(" ");
    Serial.print(fetch_digit(n,1));    
    Serial.print(" ");
    Serial.print(fetch_digit(n,0));    
    Serial.println(" ");
  }
  if(millis()-tick > 12000){
    tick = millis();
  }
  delay(0);
}
