// Загружаем значения из settings.json и vardata.json
void Load_init() {
  Serial.println("Сохраняем переменые в строки");
  jsonSettings = readFile("settings.json", 2048);
  jsonVarData = readFile("vardata.json", 2048);
  Net = jsonRead(jsonSettings, "Net");
  Fixate = jsonRead(jsonSettings, "Fixate");
  FixDHT = jsonRead(jsonVarData, "FixDHT");
}
// Сщхраняем переменые
void Var_init() {
  jsonWrite(jsonVarData, "tm", tm);
  jsonWrite(jsonVarData, "ph", ph);
  jsonWrite(jsonVarData, "ss", ss);
  jsonWrite(jsonVarData, "phDHT", phDHT);
  saveVarData();
}
// Формируем HTML строку
String webPage()
{
  String web;                                                                              // Формируем HTML строку (страницу)
  web += "<html lang=\"ru\"><head><meta charset=\"utf-8\">";                               // На русском языке, в utf-8 кодировке
  web += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";     // Перва meta маштабирует страницу,
  web += "<meta http-equiv=\"refresh\" content=\"5; url=/\">";                         // вторая meta перезагружает страницу через 5 сек на начальную страницу.
  web += "<title>Пульт</title><style>button{padding: 10px 20px;}</style>";                 // title - Задаем название страницы
  web += "<style>div{text-align: center;margin: 5px 5px 0px;}</style>";                    // Задаем стиль для тексат между div
  web += "<style>h1{text-align: center;color: blue;font-size: 20px;}</style>";             // Задаем стиль для заголовка между h1
  web += "</head><body bgcolor=\"#e6e6e6\"><h1>Web Пульт</h1><table style=\"margin: auto;\"><tr>";          //Фон страницы(#e6e6e6) и заголовок, строим таблицу
  web += "<td><p style=\"margin-top: 10px;\">---1 Канал---</p></td><td><p style=\"margin-top: 10px;\">---2 Канал--</p></td></tr><tr><td>";
  if (digitalRead(D0_pin) == 0)  {  web += "<a href=\"1pin\"><button style=\"background-color: #43a209;\">&nbsp;ВКЛ&nbsp;</button></a>";  }
                           else  {  web += "<a href=\"1pin\"><button style=\"background-color: #ec1212;\">ВЫКЛ</button></a>";  }
  web += "</td><td>";
  if (digitalRead(D2_pin) == 0)  {  web += "<a href=\"2pin\"><button style=\"background-color: #43a209;\">&nbsp;ВКЛ&nbsp;</button></a>";  }
                           else  {  web += "<a href=\"2pin\"><button style=\"background-color: #ec1212;\">ВЫКЛ</button></a>";  }
  web += "</td></tr></table><div><p style=\"margin-top: 10px;\">---Регулировка---</p>";
  web += "<a href=\"3PL\"><button style=\"background-color: #43a209;\">&nbsp;+&nbsp;</button></a>";
  web += "&nbsp&nbsp" + String(phDHT) + "&nbsp&nbsp;";
  web += "<a href=\"3MIN\"><button style=\"background-color: #43a209;\">&nbsp;-&nbsp;</button></a></div>";
  web += "<div>Температура:&nbsp" + String(tm) + "&nbsp;&deg;C</div>";
  web += "<div>Влажность:&nbsp&nbsp&nbsp" + String(ph) + "&nbsp;%</div>";
  web += "<div>Освещенность:&nbsp&nbsp" + String(ss) + "&nbsp;%</div>";
  web += "<div style=\"margin-top: 20px;\"><a href=\"/\"><button style=\"width:160px; background-color: #48b9e5;\">Обновить</button></a><br><br>";
  web += "<a href=\"./settings.htm\"><button style=\"width:160px; background-color: #48b9e5;\">Настройки</button></a>";
  web += "</div></body></html>";
  return (web);
}

// Функция отправляет данные на другой WEB сервер.
void sendData(){
  String ip2 = jsonRead(jsonSettings, "ip2");
  String ip4 = jsonRead(jsonSettings, "ip4");
  String host = "192.168."+ip2+"."+ip4;
  WiFiClient client;
  if (client.connect(host, 80)) {    // Подключение
  client.print( "GET /addDHT?ss=");  // Отправка запроса методом GET
  client.print(ss);                  // Показания освещености
  client.print("&tm=");              //
  client.print(tm);                  // Показания температуры
  client.print("&ph=");              //
  client.print(ph);                  // Показания влажности
  client.print("&phDHT=");           //
  client.print(phDHT);               // Показания % влажности
  client.print("&rl1=");             //
  client.print(RL1);                 // Положение реле 1
  client.print("&rl2=");             //
  client.print(RL2);                 // Положение реле 2
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
