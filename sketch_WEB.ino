#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h> // для ОТА- прошивки
MDNSResponder mdns;

const char* ssid = "Krilova-52";
const char* password = "RTMK7NRA";

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

  while (WiFi.waitForConnectResult() != WL_CONNECTED) { // Ждем соединения
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println(WiFi.localIP());
  ArduinoOTA.setHostname("esp8266-01"); // имя сетевого порта
  ArduinoOTA.begin(); // запуск ОТА
  if (mdns.begin("esp-01", WiFi.localIP())) {
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
    tm = Var.substring(Var.indexOf("T1") + 2, Var.indexOf("T2"));
    ph = Var.substring(Var.indexOf("T2") + 2, Var.indexOf("T3"));
    ss = Var.substring(Var.indexOf("T3") + 2, Var.indexOf("T4"));
    phDHT = Var.substring(Var.indexOf("T4") + 2, Var.indexOf("T5"));
  }
  ArduinoOTA.handle();
  server.handleClient();
}
String webPage()
{
  String web;
  web += "<html lang=\"ru\"><head><meta charset=\"utf-8\"><title>Пульт</title><style>button{padding: 10px 20px;}</style>";
  web += "<style>div{text-align: center;margin: 5px 5px 0px;}</style></head>";
  web += "<body><div><h1 style=\"color: blue;font-size: 20px;\">Web Пульт</h1>";
  web += "<p style=\"margin-top: 10px;\">---1 Канал---</p>";
  if (digitalRead(D0_pin) == 0)
  {
    web += "<a href=\"1On\"><button style=\"background-color: #43a209;\">&nbsp;ВКЛ&nbsp;</button></a>";
  }
  else
  {
    web += "<a href=\"1Off\"><button style=\"background-color: #ec1212;\">ВЫКЛ</button></a>";
  }
  web += "</div><div><p style=\"margin-top: 10px;\">---2 Канал--</p>";
  if (digitalRead(D2_pin) == 0)
  {
    web += "<a href=\"2On\"><button style=\"background-color: #43a209;\">&nbsp;ВКЛ&nbsp;</button></a>";
  }
  else
  {
    web += "<a href=\"2Off\"><button style=\"background-color: #ec1212;\">ВЫКЛ</button></a>";
  }
  web += "</div><div><p style=\"margin-top: 10px;\">---Регулировка---</p>";
  web += "<a href=\"3On\"><button style=\"background-color: #43a209;\">&nbsp;+&nbsp;</button></a>";
  web += "&nbsp&nbsp" + String(phDHT) + "&nbsp&nbsp;";
  web += "<a href=\"3Off\"><button style=\"background-color: #43a209;\">&nbsp;-&nbsp;</button></a></div>";
  web += "<div>Температура:&nbsp" + String(tm) + "&nbsp;&deg;C</div>";
  web += "<div>Влажность:&nbsp&nbsp&nbsp" + String(ph) + "&nbsp;%</div>";
  web += "<div>Освещенность:&nbsp&nbsp" + String(ss) + "&nbsp;%</div>";
  web += "<div style=\"margin-top: 20px;\"><a href=\"/\"><button style=\"width:160px;\">Перезагрузить</button></a></div></body></html>";
  return (web);
}
