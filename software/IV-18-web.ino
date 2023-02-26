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
//#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

#include "RTClib.h"
RTC_DS3231 rtc;

#ifndef STASSID
#define STASSID "238H8F"
#define STAPSK  "0920360270"
#endif

#ifndef APSSID
#define APSSID "ESP_VFD_Clock"
#define APPSK "00000000"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
IPAddress ip;  
DNSServer dnsServer;
ESP8266WebServer webServer(80);
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
String responseHTML = ""
"<style>"
//"body {"
//"  background: #ffff00;"
//"  font-size: 96px !important;"
//"}"
".all-news-btn {"
"  "
"  background: #666666;"
"  "
"  background-image: -webkit-linear-gradient(top, #666666, #333333);"
"  background-image: -moz-linear-gradient(top, #666666, #333333);"
"  background-image: -ms-linear-gradient(top, #666666, #333333);"
"  background-image: -o-linear-gradient(top, #666666, #333333);"
"  background-image: linear-gradient(to bottom, #666666, #333333);"
"  "
"  -webkit-border-radius: 3;"
"  -moz-border-radius: 3;"
"  border-radius: 3px;"
"  "
"  font-family: Arial !important;"
"  color: #ffffff !important;"
"  font-size: 96px !important;"
"  padding: 7px 8px 7px 7px;"
"  text-decoration: none;"
"  margin-left: 3px;"
""
"  min-width: 100px;"
"  min-height: 30px;"
"  display: inline;"
"}"
""
".all-news-btn:hover {"
"  background: #666666;"
"  background-image: -webkit-linear-gradient(top, #666666, #000000);"
"  background-image: -moz-linear-gradient(top, #666666, #000000);"
"  background-image: -ms-linear-gradient(top, #666666, #000000);"
"  background-image: -o-linear-gradient(top, #666666, #000000);"
"  background-image: linear-gradient(to bottom, #666666, #000000);"
"  text-decoration: none;"
"}"
""
" @media only screen and (max-width:480px) { "
"  .all-news-btn {width: 100%; font-size: 4.25em; }"
"}"
"  body {  "
" font-family: Arial, sans-serif; "
" background-color: #f1f1f1;  "
" } "
" #calculator { "
" width: 80wh;  "
" height: 100vh;  "
" margin: 0 auto; "
" background-color: #fff; "
" border: 1px solid #ccc; "
" border-radius: 5px; "
" padding: 10px;  "
" } "
" #display {  "
" border: 1px solid #ccc; "
" border-radius: 5px; "
" padding: 5px; "
" margin-bottom: 10px;  "
" } "
" #display input[type=\"text\"] { "
" width: 100%;  "
" text-align: right;  "
" font-size: 24px;  "
" border: none; "
" } "
" .button-row { "
" display: flex;  "
" } "
" .button-row button {  "
" flex: 1;  "
" font-size: 18px;  "
" margin: 5px;  "
" padding: 10px;  "
" border: none; "
" border-radius: 5px; "
" background-color: #eee; "
" } "
" .button-row button:hover {  "
" background-color: #ddd; "
" } "
" #equals { "
" background-color: #f44336;  "
" color: #fff;  "
" } "
" #equals:hover { "
" background-color: #d32f2f;  "
" } "
" #clear {  "
" background-color: #bfbfbf;  "
" color: #fff;  "
" } "
" #clear:hover {  "
" background-color: #8c8c8c;  "
" } "
"</style>"
""

""
"<html>"
"<head>"
"<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'>"
"<meta charset='utf-8'>"
"</head>"
"<body>"
"<input id='inputText' class='display' type='text' value='hello'/>"
"<button onclick = 'stop_timer_and_send_text()' class='display'>傳送</button>"
"<p>"
"<button onclick ='active_clock()' class='display'>時鐘</button>"
"<p>"
"<h1><p id='demo'></p></h1>"
"  <div id='calculator'> "
" <div id='display'>  "
" <input type='text' id='result' value='0' readonly>  "
" </div>  "
" <div class='button-row'>  "
" <button id='clear'>清除</button>  "
" <button id='sqrt'>√</button>  "
" <button id='percent'>%</button> "
" <button id='divide'>÷</button>  "
" </div>  "
" <div class='button-row'>  "
" <button id='seven'>7</button> "
" <button id='eight'>8</button> "
" <button id='nine'>9</button>  "
" <button id='multiply'>×</button>  "
" </div>  "
" <div class='button-row'>  "
" <button id='four'>4</button>  "
" <button id='five'>5</button>  "
" <button id='six'>6</button> "
" <button id='subtract'>-</button>  "
" </div>  "
" <div class='button-row'>  "
" <button id='one'>1</button> "
" <button id='two'>2</button> "
" <button id='three'>3</button> "
" <button id='add'>+</button> "
" </div>  "
" <div class='button-row'>  "
" <button id='zero'>0</button>  "
" <button id='decimal'>.</button> "
" <button id='equals'>=</button>  "
" </div>  "
" <div> "
" dahai with 9527.  "
" </div>  "
" </div>  "
"<script> "
" var timer = null;"
"function start_timer() {"
"  const now = new Date();   "
"  const nowDay = now.getDay() + 1;"
"  const nowMonth = now.getMonth() + 1;"
"  const current = now.getFullYear().toString() + '-' + nowMonth.toString().padStart(2,' ') + '-' + nowDay.toString().padStart(2,' ') + 'T' + now.getHours().toString().padStart(2,' ') + ':' + now.getMinutes().toString().padStart(2,' ') + ':' + now.getSeconds().toString().padStart(2,' ');    "
"   document.getElementById('demo').innerHTML =  current;    "
"   const Http = new XMLHttpRequest(); "
"  const url='/clock/'+current;   "
"  Http.open('GET', url);   "
"  Http.send();   "
" timer = setInterval(function() {    "
"  const now = new Date();   "
"  const nowDay = now.getDay() + 1;"
"  const nowMonth = now.getMonth() + 1;"
"  const current = now.getFullYear().toString() + '-' + nowMonth.toString().padStart(2,' ') + '-' + nowDay.toString().padStart(2,' ') + 'T' + now.getHours().toString().padStart(2,' ') + ':' + now.getMinutes().toString().padStart(2,' ') + ':' + now.getSeconds().toString().padStart(2,' ');    "
"   document.getElementById('demo').innerHTML =  current;    "
"   const Http = new XMLHttpRequest(); "
"  const url='/clock/'+current;   "
"  Http.open('GET', url);   "
"  Http.send();   "
"   if (distance < 0) {     "
"    clearInterval(x);     "
"    document.getElementById('demo').innerHTML = 'EXPIRED';   "
"    } "
"    }, 10000);"
"}"
""
"function active_clock() {"
"  if (timer == null) {"
"    start_timer();"
"    return;"
"  }"
"} "
""
"function stop_timer_and_send_text() {"
"  if (timer !== null) {"
"    clearInterval(timer);"
"    timer = null;"
"    document.getElementById('demo').innerHTML = '';"
"  }"
"  const Http = new XMLHttpRequest();"
"  const url='/text/'+document.getElementById('inputText').value;"
"  Http.open('GET', url);   "
"  Http.send();   "
""
"}"
"  const result = document.getElementById('result'); "
" const clear = document.getElementById('clear'); "
" const sqrt = document.getElementById('sqrt'); "
" const percent = document.getElementById('percent'); "
" const divide = document.getElementById('divide'); "
" const seven = document.getElementById('seven'); "
" const eight = document.getElementById('eight'); "
" const nine = document.getElementById('nine'); "
" const multiply = document.getElementById('multiply'); "
" const four = document.getElementById('four'); "
" const five = document.getElementById('five'); "
" const six = document.getElementById('six'); "
" const subtract = document.getElementById('subtract'); "
" const one = document.getElementById('one'); "
" const two = document.getElementById('two'); "
" const three = document.getElementById('three'); "
" const add = document.getElementById('add'); "
" const zero = document.getElementById('zero'); "
" const decimal = document.getElementById('decimal'); "
" const equals = document.getElementById('equals'); "
" let input = ''; "
" let operand1 = '';  "
" let operand2 = '';  "
" let operator = '';  "
" let isOperatorClicked = false;  "
" let Finish = true;  "
" function updateResult(value) {  "
" result.value = value; "
"  const Http = new XMLHttpRequest();"
"  const url='/text/'+value;"
"  Http.open('GET', url);   "
"  Http.send();   "

" } "
" function addInput(newInput) { "
" if (input === '0' && newInput === '0') {  "
" return; "
" } "
" if (input === '0' || isOperatorClicked || Finish) { "
" input = newInput; "
" Finish = false; "
" } else {  "
" input += newInput;  "
" } "
" isOperatorClicked = false;  "
" updateResult(input);  "
" } "
" function clearInput() { "
" input = '0';  "
" operand1 = '';  "
" operand2 = '';  "
" operator = '';  "
" isOperatorClicked = false;  "
" Finish = true;  "
" updateResult(input);  "
" } "
" function calculate() {  "
" switch (operator) { "
" case '+': "
" input = parseFloat(operand1) + parseFloat(input); "
" break;  "
" case '-': "
" input = parseFloat(operand1) - parseFloat(input); "
" break;  "
" case '*': "
" input = parseFloat(operand1) * parseFloat(input); "
" break;  "
" case '/': "
" input = parseFloat((parseFloat(operand1) / parseFloat(input)).toFixed(6));  "
" break;  "

" default:  "
" break;  "
" } "
" input = input.toString(); "
" operator = '';  "
" updateResult(input);  "
" } "
" clear.addEventListener('click', () => { "
" clearInput(); "
" }); "
" sqrt.addEventListener('click', () => {  "
" input = parseFloat(Math.sqrt(parseFloat(input)).toFixed(6)); "
" input = input.toString(); "
" updateResult(input);  "

" }); "
" percent.addEventListener('click', () => { "
" input = parseFloat(input) * 1 / 100;  "
" input = input.toString(); "
" updateResult(input);  "

" }); "
" divide.addEventListener('click', () => {  "
"  if(operand1 !=''){"
"    calculate();"
"  }"
" operator = '/'; "
" operand1 = input; "
" isOperatorClicked = true; "
" }); "
" seven.addEventListener('click', () => { "
" addInput('7');  "
" }); "
" eight.addEventListener('click', () => { "
" addInput('8');  "
" }); "
" nine.addEventListener('click', () => {  "
" addInput('9');  "
" }); "
" multiply.addEventListener('click', () => {  "
"  if(operand1 !=''){"
"    calculate();"
"  }"
" operator = '*'; "
" operand1 = input; "
" isOperatorClicked = true; "
" }); "
" four.addEventListener('click', () => {  "
" addInput('4');  "
" }); "
" five.addEventListener('click', () => {  "
" addInput('5');  "
" }); "
" six.addEventListener('click', () => { "
" addInput('6');  "
" }); "
" subtract.addEventListener('click', () => {  "
"  if(operand1 !=''){"
"    calculate();"
"  }"
" operator = '-'; "
" operand1 = input; "
" isOperatorClicked = true; "
" }); "
" one.addEventListener('click', () => { "
" addInput('1');  "
" }); "
" two.addEventListener('click', () => { "
" addInput('2');  "
" }); "
" three.addEventListener('click', () => { "
" addInput('3');  "
" }); "
" add.addEventListener('click', () => { "
"  if(operand1 !=''){"
"    calculate();"
"  }"
" operator = '+'; "
" operand1 = input; "
" isOperatorClicked = true; "
" }); "
" zero.addEventListener('click', () => {  "
" addInput('0');  "
" }); "
" function handleDecimal() {  "
" if (input === '') { "
" addInput('0.'); "
" } else if (!input.includes('.')) {  "
" addInput('.');  "
" } "
" } "
" decimal.addEventListener('click', () => { "
" handleDecimal();  "
" }); "
" equals.addEventListener('click', () => {  "
" calculate();  "
" Finish = true;  "
" }); "
" document.addEventListener('keydown', (event) => { "
" const key = event.key;  "
" if (!isNaN(key) || key === '.') { "
" addInput(key);  "
" } else if (key === '+' || key === '-' || key === '*' || key === '/') {  "
" operator = key; "
" isOperatorClicked = true; "
" } else if (key === '%') { "
" operator = '%'; "
" isOperatorClicked = true; "
" calculate();  "
" } else if (key === 'Enter' || key === '=') {  "
" calculate();  "
" } else if (key === 'Backspace') { "
" input = input.slice(0, -1); "
" updateResult(input);  "
" } else if (key === 'Escape') {  "
" clearInput(); "
" } "
" }); "
"</script> "
"</body>"
"</html>";
 
String responseHTML_ = " <!DOCTYPE html> "
"<html> "
"<title></title> "
"<head> </head> "
"<body> "
"<h1>Online HTML Editor</h1> "
"<h1><p id='demo'></p></h1> "
"</body> "
"</html>  "
"<script> "
" var x = setInterval(function() {    "
"  const now = new Date();   "
"  const current = now.getHours().toString().padStart(2,' ') + '.' + now.getMinutes().toString().padStart(2,' ') + '.' + now.getSeconds().toString().padStart(2,' ');   " 
"   document.getElementById('demo').innerHTML =  current;  "  
"   const Http = new XMLHttpRequest(); "
"  const url='/clock/'+current;   "
"  Http.open('GET', url);   "
"  Http.send();   "
"   if (distance < 0) {    " 
"    clearInterval(x);     "
"    document.getElementById('demo').innerHTML = 'EXPIRED';   "
"    } "
"    }, 700); "
"    </script> ";

/*
 * 
 */
 unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}
String urldecode(String str)
{
    
    String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str.length(); i++){
        c=str.charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str.charAt(i);
        i++;
        code1=str.charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
        
        encodedString+=c;  
      }
      
      yield();
    }
    
   return encodedString;
}  
/*
 * 
 */
void cegc(void) {
  analogWriteFreq(1046);
  analogWrite(15,127);
  delay(100);
  analogWriteFreq(1318);
  analogWrite(15,127);
  delay(100);
  analogWriteFreq(1567);
  analogWrite(15,127);
  delay(100);
  analogWriteFreq(2093);
  analogWrite(15,127);
  delay(100);
  analogWrite(15,0);  
}
void cgec(void) {
  analogWriteFreq(2093);
  analogWrite(15,127);
  delay(100);
  analogWriteFreq(1567);
  analogWrite(15,127);
  delay(100);
  analogWriteFreq(1318);
  analogWrite(15,127);
  delay(100);
  analogWriteFreq(1046);
  analogWrite(15,127);
  delay(100);
  analogWrite(15,0);    
}
void beep(){
  analogWrite(15,127);
  delay(333);
  analogWrite(15,0);  
}
/*
 * 
 */
int serialDataSubStart = 0;
bool rtc_on = 0;

void setup() 
{
  delay(1000);
  /*
   * 
   */
  Serial.begin(115200);
  
//  WiFi.mode(WIFI_STA);
//  WiFi.begin(ssid, password);
  /*
   *  Wifi soft-AP
   */
  Serial.println("");
  Serial.print("Setting soft-AP ... ");
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  boolean result = WiFi.softAP(APSSID);
  
  serialData = APSSID;
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }

  Serial.println("");
  Serial.println(F(__DATE__));
  Serial.println(F(__TIME__));
  /*
   * 
   */
   rtc.begin();
    rtc_on = 1;    
//  if (! rtc.begin()) {
//    Serial.println("Couldn't find RTC");
//    Serial.flush();
//  }else{
//    if (rtc.lostPower()) {
//      Serial.println("RTC lost power, let's set the time!");
//      // When time needs to be set on a new device, or after a power loss, the
//      // following line sets the RTC to the date & time this sketch was compiled
//      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//      // This line sets the RTC with an explicit date & time, for example to set
//      // January 21, 2014 at 3am you would call:
//      // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
//    }
//    rtc_on = 1;    
//  }
  
  
  // Wait for connection
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
  Serial.println("");
//  Serial.print("Connected to ");
//  Serial.println(ssid);
//  Serial.print("IP address: ");
//  ip = WiFi.localIP();
//  serialData = ipToString(ip);
//  Serial.println(ip);
/*
 *   captive portal
 */
  dnsServer.start(DNS_PORT, "*", apIP);

  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });

/*
 * 
 */
//  if (MDNS.begin("esp8266")) {
//    Serisal.println("MDNS responder started");
//  }
/*
 * 
 */
  webServer.on(F("/"), []() {
    webServer.send(200, "text/html", responseHTML);
  });

  webServer.on(UriBraces("/clock/{}"), []() {
    String user = webServer.pathArg(0);
    webServer.send(200, "text/html", "<h1>"+urldecode(user)+"</h1>");
    serialDataSubStart = 0;
    serialData = urldecode(user);
    String Year = serialData.substring(0,serialData.indexOf("-"));
    String remain = serialData.substring(serialData.indexOf("-")+1,serialData.length());
    String Month = remain.substring(0,remain.indexOf("-"));
    String remain1 = remain.substring(remain.indexOf("-")+1,remain.length());
    String Day = remain1.substring(0,remain1.indexOf("T"));
    String remain2 = remain1.substring(remain1.indexOf("T")+1,remain1.length());
    String Hour = remain2.substring(0,remain2.indexOf(":"));
    String remain3 = remain2.substring(remain2.indexOf(":")+1,remain2.length());
    String Min = remain3.substring(0,remain3.indexOf(":"));
    String remain4 = remain3.substring(remain3.indexOf(":")+1,remain3.length());
    String Sec = remain4;
    Serial.print(Year.toInt());
    Serial.print(" ");
    Serial.print(Month.toInt());
    Serial.print(" ");
    Serial.print(Day.toInt());
    Serial.print(" ");
    Serial.print(Hour.toInt());
    Serial.print(" ");
    Serial.print(Min.toInt());
    Serial.print(" ");
    Serial.println(Sec.toInt());
    rtc.adjust(DateTime(Year.toInt(),Month.toInt(),Day.toInt(),Hour.toInt(),Min.toInt(),Sec.toInt()));
  });

  webServer.on(UriBraces("/text/{}"), []() {
    String user = webServer.pathArg(0);
    webServer.send(200, "text/html", "<h1>"+urldecode(user)+"</h1>");
    serialDataSubStart = 0;
    serialData = urldecode(user);
  });

//  server.on(UriRegex("^\\/users\\/([0-9]+)\\/devices\\/([0-9]+)$"), []() {
//    String user = server.pathArg(0);
//    String device = server.pathArg(1);
//    server.send(200, "text/plain", "User: '" + user + "' and Device: '" + device + "'");
//  });

  webServer.begin();
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
  pinMode(15, OUTPUT);
  analogWrite(15,127);
  delay(1000);
  analogWrite(15,0);
}
/*
 * 
 */
void iv18_digit(int digit, char ch)
{
  if(ch < 0x20 || ch > 0x7f) return;
  
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
  int segment_convert_array[0x7f-0x20+1]={
    0,
    b,
    b|f,
    b|c|e|f,
    a|d|f|g,
    a|c|d|f,
    a|b|d|e|f|g,
    f,
    a|d|e|f,
    a|b|c|d,
    a|b|f|g,
    e|f|g,
    c|d,
    g,
    dp,
    b|g,
    a|b|c|d|e|f, // 0
    b|c, 
    a|b|g|e|d, 
  a|b|g|c|d, 
  f|g|b|c, 
  a|f|g|c|d, 
  a|f|g|c|d|e, 
  a|b|c, 
  a|b|c|d|e|f|g, 
  a|b|c|d|f|g,  // 9
  d|g,
  c|d|g,
  a|f|g,
  a|g,
  a|b|g,
  a|b|e|g,
  a|b|c|d|e|g,
  a|b|c|e|f|g, // A
  c|d|e|f|g,
  a|d|e|f,
  b|c|d|e|g,
  a|d|e|f|g,
  a|e|f|g,
  a|c|d|e|f,
  c|e|f|g,
  a|d|e,
  b|c|d|e,
  a|c|e|f|g,
  d|e|f,
  a|c|e,
  a|b|c|e|f,
  c|d|e|g,
  a|b|e|f|g,
  a|b|c|f|g,
  a|b|e|f,
  c|f|g,
  d|e|f|g,
  b|c|d|e|f,
  b|e|f,
  b|d|f,
  b|c|e|f|g,
  b|c|d|f|g,
  d|g,       // Z
  a|d|e|f,
  f|g,
  a|b|c|d,
  a|b|f,
  d,
  0,
  a|b|c|e|f|g, // A
  c|d|e|f|g,
  a|d|e|f,
  b|c|d|e|g,
  a|d|e|f|g,
  a|e|f|g,
  a|c|d|e|f,
  c|e|f|g,
  a|d|e,
  b|c|d|e,
  a|c|e|f|g,
  d|e|f,
  a|c|e,
  a|b|c|e|f,
  c|d|e|g,
  a|b|e|f|g,
  a|b|c|f|g,
  a|b|e|f,
  c|f|g,
  d|e|f|g,
  b|c|d|e|f,
  b|e|f,
  b|d|f,
  b|c|e|f|g,
  b|c|d|f|g,
  d|g,       // Z
  0,
  0,
  0,
  0,
  0  
  };
  byte3 = segment_convert_array[ch-0x20];
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
int old_softAPgetStationNum = 0;
void loop() 
{
  if((millis()-tick) % 500 == 0){
    if(WiFi.softAPgetStationNum() != old_softAPgetStationNum){
      old_softAPgetStationNum = WiFi.softAPgetStationNum();
      Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
      if(WiFi.softAPgetStationNum() > 0){
        cegc();
      }
      if(WiFi.softAPgetStationNum() == 0){
        serialDataSubStart = 0;
        serialData = APSSID;
        cgec();
      }
    }
  }
  /*
   * 
   */
   dnsServer.processNextRequest();
   webServer.handleClient();
   /*  
    *   
    */
   if (Serial.available() > 0) {
      // read the incoming byte:
      String serialData_;
      serialData_ = Serial.readStringUntil('\n');
      n=serialData_.toInt();
      serialDataSubStart = 0;
      serialData = urldecode(serialData_);
   }
   if (WiFi.softAPgetStationNum() == 0 && rtc_on == 1) {
      serialDataSubStart = 0;
      DateTime now = rtc.now();
//      String hour_str = ((String(now.hour()).length() == 1)?" ":"") + String(now.hour());
//      String min_str = ((String(now.minute()).length() == 1)?" ":"") + String(now.minute());
//      String sec_str = ((String(now.second()).length() == 1)?" ":"") + String(now.second());
//      serialData = hour_str + " " + min_str + " " + sec_str;
      char str_buffer[12];
      sprintf(str_buffer,"%2d %2d %2d",now.hour(),now.minute(),now.second());
      serialData = String(str_buffer);
   }
  
  int dgt;

  if(serialData.length()>0){
    /*
     *  serialData to seprate digit
     */
    
    if(string_shift_delay_time == 0 ) resultData = serialData;
    if((millis() - string_shift_delay_time) > 800){
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
    if(isAscii(ch)){
//      dgt=String(ch).toInt();
      iv18_digit(7-digit_selector,ch);
      delay(0);
    }

    tick = millis()+2000;
  }else{
    /*
     *  n to seprate digit
     */
    dgt=fetch_digit(n,digit_selector);
    if(high_zero_blank && dgt==0){
      iv18_digit(digit_selector,' '); //blank
    }else{
      high_zero_blank=0;
      iv18_digit(digit_selector,dgt);
    }
      delay(0);
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
