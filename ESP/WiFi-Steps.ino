#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>      // не выше 5 версии
#include <FS.h>
#include <ArduinoOTA.h>       // для ОТА- прошивки, при условии памяти не менее 1М

ESP8266WebServer HTTP(80);    // WEB интерфейс (сервер)
File fsUploadFile;            // Для файловой системы

String jsonSettings = "{}";   // данные для settings.json
String jsonVarData = "{}";    // данные для vardata.json
String phDHT = "";
String tm, ph, ss;            // переменные
String FixDHT, RL1, RL2;      // показания
String Net = "O";       // checked
String Fixate = "O";    // checked
int D0_pin = 0;         //ESP-01
int D2_pin = 2;         //ESP-01
int stat[3] = {0,1,2};  // Массив для счетчиков

void setup(void) {
  pinMode(D0_pin, OUTPUT);
  digitalWrite(D0_pin, HIGH);
  pinMode(D2_pin, OUTPUT);
  digitalWrite(D2_pin, HIGH);

  Serial.begin(74880);
  FS_init();    // Start FS
  Load_init();  // Variable
  WIFI_init();  // Start WiFi
  HTTP_init();  // Start Server
  
  ArduinoOTA.setHostname("esp8266"); // Задаем имя сетевого порта
  ArduinoOTA.begin();                // запуск ОТА
}
void loop(void) {
  if (Serial.available()) {
    String Var = "";                                               // ... или очищаем
    while (!Serial.available()) delay(20);                         // Слушаем порт
    delay(400);
    while (Serial.available())
    Var = Var + (char)(Serial.read());                             // Принимаем строку пока есть передача
    tm = Var.substring(Var.indexOf("T1") + 2, Var.indexOf("T2"));  // Присваиваем значение после Т1 до Т2
    ph = Var.substring(Var.indexOf("T2") + 2, Var.indexOf("T3"));
    ss = Var.substring(Var.indexOf("T3") + 2, Var.indexOf("T4"));
    phDHT = Var.substring(Var.indexOf("T4") + 2, Var.indexOf("T5"));
  }
  if (stat[1] >= 599) {       // Передаем данные через каждую 1 сек (400 слушаем + 599)
    if (Net == "V") {         // Проверяем "разрешение" на отправку
      sendData();             // Отправка данных на другую ESP
      stat[1] = 0;            // обнуляем счетчик
    }
  } else {  stat[1] ++;  }
  
  if (Fixate == "V"){                         // Проверяем включение фиксации
    FixDHT = jsonRead(jsonVarData, "FixDHT");
    if (phDHT < FixDHT){   // если меньше отправим PL
      Serial.print(1);
      delay(100);
    }
    if (phDHT > FixDHT){   // если больше отправим MIN
      Serial.print(0);
      delay(100);
    }
  }
  
  ArduinoOTA.handle();
  HTTP.handleClient();
}
