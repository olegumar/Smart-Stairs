#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h> // для ОТА- прошивки
MDNSResponder mdns;

const char* ssid = "RTK70"; //"Krilova-52";
const char* password = "77596501392";  //"RTMK7NRA";
const char* host = "192.168.1.33";

IPAddress ip(192, 168, 1, 35);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
ESP8266WebServer server(80);
int D0_pin = 0;
int D2_pin = 2;
int sig = 40;
String tm, ph, ss, phDHT;

void setup(void) {
  pinMode(D0_pin, OUTPUT);
  digitalWrite(D0_pin, LOW);
  pinMode(D2_pin, OUTPUT);
  digitalWrite(D2_pin, LOW);

  Serial.begin(74880);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {  // Ждем соединения (подключения)
    Serial.println("Connection Failed! Rebooting...");  // "Подключиться не удалось! Перезапускаем ..."
    delay(5000);
    ESP.restart();                                      // Перегружаем модуль ESP
  }
  Serial.println(WiFi.localIP());
  
  ArduinoOTA.setHostname("esp8266-01"); // Задаем имя сетевого порта
  ArduinoOTA.begin();                   // запуск ОТА
  if (mdns.begin("esp8266-01", WiFi.localIP())) {
    Serial.println("MDNS responder started");   //  Запущен MDNSresponder
  }
  server.on("/", []() {
    server.send(200, "text/html", webPage());
  });
  //+++++++++++++++++++++++ START  LED-1 ++++++++++++++++++++
  server.on("/1On", []() {
    digitalWrite(D0_pin, HIGH);
    server.send(200, "text/html", webPage());
    delay(100);
  });
  server.on("/1Off", []() {
    digitalWrite(D0_pin, LOW);
    server.send(200, "text/html", webPage());
    delay(100);
  });
  //+++++++++++++++++++++++ END  LED-1 ++++++++++++++++++++
  //+++++++++++++++++++++++ START  LED-2  ++++++++++++++++++++
  server.on("/2On", []() {
    digitalWrite(D2_pin, HIGH);
    server.send(200, "text/html", webPage());
    delay(100);
  });
  server.on("/2Off", []() {
    digitalWrite(D2_pin, LOW);
    server.send(200, "text/html", webPage());
    delay(100);
  });
  // +++++++++++++++++++++++ END  LED-2 ++++++++++++++++++++
  //+++++++++++++++++++++++ START  LED-3 ++++++++++++++++++++
  server.on("/3On", []() {
    Serial.print(1);
    delay(200);
    server.send(200, "text/html", webPage());
    delay(100);
  });
  server.on("/3Off", []() {
    Serial.print(0);
    delay(200);
    server.send(200, "text/html", webPage());
    delay(100);
  });
  //+++++++++++++++++++++++ END  LED-3 ++++++++++++++++++++
  server.begin();
  Serial.println("HTTP сервер запущен");
}
void loop(void) {
  if (Serial.available()) {
    String Var = "";
    while (!Serial.available()) delay(20);
    delay(400);
    while (Serial.available())
    Var = Var + (char)(Serial.read());
    tm = Var.substring(Var.indexOf("T1") + 2, Var.indexOf("T2"));  // Присваиваем значение от Т1 до Т2
    ph = Var.substring(Var.indexOf("T2") + 2, Var.indexOf("T3"));
    ss = Var.substring(Var.indexOf("T3") + 2, Var.indexOf("T4"));
    phDHT = Var.substring(Var.indexOf("T4") + 2, Var.indexOf("T5"));
  }
  sendData(ss, tm, ph, phDHT);  // Отправка данных
  ArduinoOTA.handle();
  server.handleClient();
}
String webPage()
{
  String web;
  web += "<html lang=\"ru\"><head><meta charset=\"utf-8\">";
  web += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  web += "<title>Пульт</title><style>button{padding: 10px 20px;}</style>";
  web += "<style>div{text-align: center;margin: 5px 5px 0px;}</style>";
  web += "<style>h1{text-align: center;color: blue;font-size: 20px;}</style>";
  web += "</head><body><h1>Web Пульт</h1><table style=\"margin: auto;\"><tr>";
  web += "<td><p style=\"margin-top: 10px;\">---1 Канал---</p></td><td><p style=\"margin-top: 10px;\">---2 Канал--</p></td></tr><tr><td>";
  if (digitalRead(D0_pin) == 0)
  {
    web += "<a href=\"1On\"><button style=\"background-color: #43a209;\">&nbsp;ВКЛ&nbsp;</button></a>";
  }
  else
  {
    web += "<a href=\"1Off\"><button style=\"background-color: #ec1212;\">ВЫКЛ</button></a>";
  }
  web += "</td><td>";
  if (digitalRead(D2_pin) == 0)
  {
    web += "<a href=\"2On\"><button style=\"background-color: #43a209;\">&nbsp;ВКЛ&nbsp;</button></a>";
  }
  else
  {
    web += "<a href=\"2Off\"><button style=\"background-color: #ec1212;\">ВЫКЛ</button></a>";
  }
  web += "</td></tr></table><div><p style=\"margin-top: 10px;\">---Регулировка---</p>";
  web += "<a href=\"3On\"><button style=\"background-color: #43a209;\">&nbsp;+&nbsp;</button></a>";
  web += "&nbsp&nbsp" + String(phDHT) + "&nbsp&nbsp;";
  web += "<a href=\"3Off\"><button style=\"background-color: #43a209;\">&nbsp;-&nbsp;</button></a></div>";
  web += "<div>Температура:&nbsp" + String(tm) + "&nbsp;&deg;C</div>";
  web += "<div>Влажность:&nbsp&nbsp&nbsp" + String(ph) + "&nbsp;%</div>";
  web += "<div>Освещенность:&nbsp&nbsp" + String(ss) + "&nbsp;%</div>";
  web += "<div style=\"margin-top: 20px;\"><a href=\"/\"><button style=\"width:160px;\">Перезагрузить</button></a></div></body></html>";
  return (web);
}
/*------------------------------------------------------------------------
 Функция отправляет данные о температуре и влажности на другой WEB сервер.
-----------------------------------------------------------------------*/ 
void sendData(String ss, String tm, String ph, String phDHT){
  WiFiClient client;
  if (client.connect(host, 80)) {    // Подключение
  client.print( "GET /add?ss=");     // Отправка запроса
  client.print(ss);                  // Показания освещености
  client.print("&tm=");              //
  client.print(tm);                  // Показания температуры
  client.print("&ph=");              //
  client.print(ph);                  // Показания влажности
  client.print("&phDHT=");           //
  client.print(phDHT);               // Показания % влажности
  client.println(" HTTP/1.1");
  client.print( "Host: " );
  client.println(host);
  client.println( "Connection: close" ); //Соединение: закрыто
  client.println();
  client.println();
  client.stop();                      // Отключение
  client.flush();
  } else {
  //  Serial.println("Connection failed!]"); // "подключиться не удалось!"
    client.stop();
  }
}
