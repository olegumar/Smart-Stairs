void HTTP_init(void) {
  HTTP.on("/", []() {
  HTTP.send(200, "text/html", webPage());
  });
  //+++++++++++++++++++++++ START  LED-1 ++++++++++++++++++++
  HTTP.on("/1pin", []() {
    if (digitalRead(D0_pin) == 0)
      {  digitalWrite(D0_pin, HIGH);
         RL1="HIGH";  }
    else
      {  digitalWrite(D0_pin, LOW);
         RL1="LOW";  }
    HTTP.send(200, "text/html", webPage());
    delay(100);
  });
  //+++++++++++++++++++++++ END  LED-1 ++++++++++++++++++++
  //+++++++++++++++++++++++ START  LED-2  ++++++++++++++++++++
  HTTP.on("/2pin", []() {
    if (digitalRead(D2_pin) == 0)
      {  digitalWrite(D2_pin, HIGH);
         RL2="HIGH";  }
    else
      {  digitalWrite(D2_pin, LOW);
         RL2="LOW";  }
    HTTP.send(200, "text/html", webPage());
    delay(100);
  });
  // +++++++++++++++++++++++ END  LED-2 ++++++++++++++++++++
  //+++++++++++++++++++++++ START  LED-3 ++++++++++++++++++++
  HTTP.on("/3PL", []() {
    Serial.print(1);
    delay(200);
    HTTP.send(200, "text/html", webPage());
    delay(100);
  });
  HTTP.on("/3MIN", []() {
    Serial.print(0);
    delay(200);
    HTTP.send(200, "text/html", webPage());
    delay(100);
  });
  //+++++++++++++++++++++++ END  LED-3 ++++++++++++++++++++
  //+++++++++++++++++++++++ START GET ++++++++++++++++++++
// sett?Fixate=checked&Net=0&ssid=RTK70&password=77596501392&ip2=1&ip3=35&ip4=33&ssidAp=WiFi&passwordAp=
  HTTP.on("/sett", []() {                                 // Принимаем значение
   jsonWrite(jsonSettings, "Fixate", HTTP.arg("Fixate"));
   jsonWrite(jsonSettings, "Net", HTTP.arg("Net"));
   jsonWrite(jsonSettings, "ssid", HTTP.arg("ssid"));             // имя сети
   jsonWrite(jsonSettings, "password", HTTP.arg("password"));     // пароль сети
   jsonWrite(jsonSettings, "ssidAp", HTTP.arg("ssidAp"));         // имя точки
   jsonWrite(jsonSettings, "passwordAp", HTTP.arg("passwordAp")); // пароль точки
   jsonWrite(jsonSettings, "ip2", HTTP.arg("ip2"));
   jsonWrite(jsonSettings, "ip3", HTTP.arg("ip3"));
   jsonWrite(jsonSettings, "ip4", HTTP.arg("ip4"));
   saveSett();                                                    // сохраняем в файл settings.json
   HTTP.send(200, "text/plain", "OK");
   String chbox = HTTP.arg("Fixate");
   if (chbox == "V"){ 
      FixDHT = phDHT;
      jsonWrite(jsonVarData, "FixDHT", FixDHT);
      saveVarData();                                              // сохраняем в файл vardata.json
   }
  });
  
  HTTP.on("/vardata", []() {                                  // Принимаем значение
   for (uint8_t i = 0; i < HTTP.args(); i++) {                // args кол-во аргументов
      jsonWrite(jsonVarData, HTTP.argName(i), HTTP.arg(i));   // сохраняем каждый аргумент с его именем
   }
   saveVarData();                                             // созраняем в файл vardata.json
   HTTP.send(200, "text/plain", "OK");
  });
  //+++++++++++++++++++++++ END GET ++++++++++++++++++++
  //+++++++++++++++++++++++ RESSTART ++++++++++++++++++++
  HTTP.on("/restart", HTTP_GET, []() {
   String restart = HTTP.arg("device");         // Получаем значение device из запроса
   if (restart == "ok") {                       // Если значение равно Ок
    HTTP.send(200, "text / plain", "Reset OK"); // Oтправляем ответ Reset OK
    ESP.restart();                              // перезагружаем модуль
   } else {                                     // иначе
    HTTP.send(200, "text / plain", "No Reset"); // Oтправляем ответ No Reset
   }
  });
  //++++++++++++++++++++++ END START ++++++++++++++++++++
  HTTP.begin();                             // Запускаем HTTP сервер
  Serial.println("HTTP сервер запущен");
}
