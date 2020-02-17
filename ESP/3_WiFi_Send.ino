void WIFI_init() {
  IPAddress IPlocal;
  IPAddress IPgete;
  String ssid = jsonRead(jsonSettings, "ssid");
  String password = jsonRead(jsonSettings, "password");
  if (ssid == "" && password == ""){
    ssid = "RTK70";            // "Krilova-52"
    password = "77596501392";  // "RTMK7NRA"
  }
  int ip2 = jsonReadtoInt(jsonSettings, "ip2");
  int ip3 = jsonReadtoInt(jsonSettings, "ip3");
  if (jsonRead(jsonSettings, "ip2") == "" && jsonRead(jsonSettings, "ip3") == ""){
    ip2 = 0; ip3 = 0;
  }
  if (ip2 != 0 && ip3 != 0 && ip3 != 1 ){   //Приметивная проверка на не желаемый адрес
  IPlocal[0] = 192;            //Формируем наш IP желаемый (стат) адрес
  IPlocal[1] = 168;
  IPlocal[2] = ip2;            //
  IPlocal[3] = ip3;            //
  IPgete[0] = 192;             //Формируем IP адрес шлюза
  IPgete[1] = 168;
  IPgete[2] = ip2;             //
  IPgete[3] = 1;
  WiFi.config(IPlocal, IPgete , IPAddress(255, 255, 255, 0));
  } else {
    Serial.println("IP не задан.");
    IPAddress IPgete(0, 0, 0, 0);
  }
  WiFi.mode(WIFI_STA);
  byte tries = 11;
  //WiFi.begin(ssid.c_str(), password.c_str());
  WiFi.begin(ssid, password);
  // Делаем проверку подключения до тех пор пока счетчик tries
  // не станет равен нулю или не получим подключение
  while (--tries && WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  if (WiFi.status() != WL_CONNECTED)
  {
  // Если не удалось подключиться запускаем в режиме AP
    Serial.println("");
    Serial.print("Не удалось подключиться к: ");
    Serial.println(ssid);
    Serial.print("с ключом: ");
    Serial.println(password);
    StartAPMode();
  }
  else {
   //Иначе удалось подключиться отправляем сообщение о подключении и выводим адрес IP 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("IP gateway: ");
    Serial.println(WiFi.gatewayIP());
  }
}
bool StartAPMode() {
    Serial.println("WiFi up AP");
    Serial.println("IP address: 192.168.4.1");
    Serial.println("");
  String ssidAP = jsonRead(jsonSettings, "ssidAP");
  String passwordAP = jsonRead(jsonSettings, "passwordAP");
  if (ssidAP == "" ){ ssidAP = "WiFi-ESP"; }
  IPAddress apIP(192, 168, 4, 1);
  // Отключаем WIFI
  WiFi.disconnect();
  // Меняем режим на режим точки доступа
  WiFi.mode(WIFI_AP);
  // Задаем настройки сети
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  // Включаем WIFI в режиме точки доступа с именем и паролем хронящихся в переменных ssidAP passwordAP
  WiFi.softAP(ssidAP.c_str(), passwordAP.c_str());
  return true;
}
