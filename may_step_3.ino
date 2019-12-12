#include <DHT.h>
#include <Ultrasonic.h>
#include <SoftwareSerial.h>

#define DHTPIN 11              // номер пина, к которому подсоединен датчик
DHT dht(DHTPIN, DHT11);        // Инициируем датчик
Ultrasonic sonicUP(8, 7);      // Устанавливаем пины для дальномера 1 (Trig, Echo)
Ultrasonic sonicDOWN(10, 9);   // Устанавливаем пины для дальномера 2 (Trig, Echo)
SoftwareSerial mySerial(2, 3); // RX, ~TX
// Пины
int Ds_Pin = 4;   // Ds/SDI первого сдвигового регистра 75HC595
int ST_Pin = 5;   // ST/RCLK сдвиговых регистров
int SH_Pin = 6;   // SH/SRCLK сдвиговых регистров
int Sv_Pin = A0;  // Аналоговый сигнал с фоторезистора
int Dv_Pin = A1;  // Движение датчик PIR
int Led_Pin = A2; // Номер пина, к которому подсоединен светодиод
int Rele = 12;    // Номер пина, к которому подсоединен реле
// Параметры
int SvLimit = 600;        // Переменная для сравнения с освещенностью.
int phDHT = 49;           // Переменная для сравнения с влажностью.
const int MinDist = 85;   // Переменная для сравнения с шириной ступеньки, 80 см.
const int N = 16;         // Переменная для хранения количества ступеней лестницы.
int LS = "OFF";           // Переменная отражающая ON / OFF подсветки.
int Start = "null";       // Переменная отражающая факт присутствия и направления движения, по умолчанию null
unsigned int leds = 0;    // Переменная для хранения передаваемых битов в регистры.
const int TimTrap = 1500; // Переменная для хранения времени включенной лестницы.

void setup()
{
  pinMode(Ds_Pin, OUTPUT);
  pinMode(ST_Pin, OUTPUT);
  pinMode(SH_Pin, OUTPUT);
  pinMode(Dv_Pin, INPUT);
  pinMode(Led_Pin, OUTPUT);
  pinMode(Rele, OUTPUT);

  Serial.begin(74880);   // Инициализация последовательного порта на скорости 74880 бойд.
  mySerial.begin(74880); // Инициализация своего последовательного порта на скорости 74880 бойд для общения с ESP-01
  // тестовый запуск, демострация
  for (int i = 0; i <= N; i++)
  {
    leds = 0;
    bitWrite(leds, i, HIGH);
    Serial.println(leds);
    InRegister(MSBFIRST);
  }
  for (int i = 0; i <= N; i++)
  {
    leds = 0;
    bitWrite(leds, i, HIGH);
    Serial.println(leds);
    InRegister(LSBFIRST);
  }
}
void loop()
{
  int Svet_Value = analogRead(Sv_Pin);               // Считываем значение с фоторезистора от 0 до 1023
  int ss = map(analogRead(Sv_Pin), 0, 1023, 0, 100); // Переводим освещеность в проценты
  int tm = dht.readTemperature();                    // Считываем температуру
  int ph = dht.readHumidity();                       // Считываем влажность
  Serial.println(Svet_Value);       // Смотрим уровень освещенности
  Serial.println(phDHT);            // Смотрим параметр для вкл реле
  // Принимаем данные с сервера
  if (mySerial.available()) {       // Проверяем прием
    String Var = "";
    while (mySerial.available())    // Пока есть прием
      Var += char(mySerial.read()); // Cчитываем значение
    Serial.print("значение_");
    Serial.println(Var);       // Смотрим принятое значение
    if (Var == "1") {          // Если приняли значение 1
      phDHT += 2;              // Увеличиваем параметр для вкл реле
      if (phDHT >= 100) {      // Если значение превышает 100, то
        phDHT = 99;            // присваиваем 99
      }
    }
    if (Var == "0") {          // Если приняли значение 0
      phDHT -= 2;              // Уменьшаем параметр для вкл реле
      if (phDHT <= 0) {        // Если значение превышает 0, то
        phDHT = 1;             // присваиваем 1
      }
    }
  }
  String str = "T1";           // Готовим строку к передачи на сервер
  str += tm;                   // температура
  str += "T2";
  str += ph;                   // влажность
  str += "T3";
  str += ss;                   // освещенность в процентах
  str += "T4";
  str += phDHT;                // параметр влажности
  str += "T5";
  mySerial.print(str);         // Передаем строку на сервер
  delay(200);                  // Короткая пауза, что б сервер успел прочитать.

  if (ph > phDHT) {            // Сравниваем влажность для вкл или выкл реле
    digitalWrite(Rele, HIGH);
  } else {
    digitalWrite(Rele, LOW);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////
  if (Svet_Value < SvLimit)        // Сравнение "Достаточно ли темно для подсветки?".
  {
    digitalWrite(Led_Pin, HIGH);      // Влючаем светодиод, признак готовности (темно)
    if (sonicDOWN.read(CM) < MinDist) // Проверка факта срабатывания нижнего сенсора.
    {
      // Serial.print(sonicDOWN.read(CM));
      Serial.println("_Down_");    // Нижний
      Step(HIGH, LSBFIRST);        // Вызов процедуры(вкл, направление) на вкл лестницы сНизу вВерх.
      Start = MSBFIRST;            // Меняем направления, для выкл (признак)
      delay(TimTrap);              // Задержка включенной лестницы.
      SvLimit += 200;              // Изменяем параметр для сравнения с освещенностью, так как при включении становиться светлей.
    }
    if (sonicUP.read(CM) < MinDist) // Проверка факта срабатывания верхнего сенсора.
    {
      // Serial.print(sonicUP.read(CM));
      Serial.println("_Up_");      // Вверхний
      Step(HIGH, MSBFIRST);        // Вызов процедуры(вкл, направление) на вкл лестницы сВерху вНиз.
      Start = LSBFIRST;            // Меняем направления, для выкл (признак)
      delay(TimTrap);              // Задержка включенной лестницы.
      SvLimit += 200;              // Изменяем параметр для сравнения с освещенностью, что б сработало выключение.
    }
//   delay(200);                    // Задержка для чтения сенсоров.
    if (digitalRead(Dv_Pin) == LOW && Start != "null")  // Проверка факта срабатывания датчика движения.
    {                              // Если нет движения и задано направление (признак что подсветка вкл)
      Serial.println("_OFF_");     // Выключаем
      LS = "OFF";                  // Флаг выкл
      Step(LOW, Start);            // Вызов процедуры(выкл, направление) на выкл лестницы.
      if (LS == "OFF") Start = "null"; 
      LS = "ON";                   // Флаг вкл
      SvLimit = 600;               // Возвращаем параметр для сравнения с освещенностью.
    }
  } else {
    digitalWrite(Led_Pin, LOW);    // Выключаем светодиод (светло)
  }
  Serial.print("_");
  Serial.println(Start);
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void Step(int SIG, int Start)      // Процедура вкл или выкл подсветки
{
  for (int i = 0; i <= N; i++)
  {
    bitWrite(leds, i, SIG);
    InRegister(Start);             // Включение подсветки загрузкой измененного байта.
    if (LS == "OFF")               // Если происходит выкл, проверяем ниж и вер датчиков
    {
      if (sonicDOWN.read(CM) < MinDist || sonicUP.read(CM) < MinDist)  // Проверка на срабатывания
      {
        LS = "ON";                 // Если есть срабатывания, меняем флаг
        return;                    // и выходим из процедуры выкл
      }
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void InRegister(int Start)                 // Процедура загрузки битов в регистры 75HC595 (направление)
{
  digitalWrite(ST_Pin, LOW );              // Подаем сигнал для загрузки битов в регистры.
  byte First = lowByte(leds);              // Разделяем наш байт на младший и старший,так как функция shiftOut за раз пропускает только 8 бит.
  byte Second = highByte(leds);            // Следующий байт для shiftOut
  if (Start == MSBFIRST)
  {
  shiftOut(Ds_Pin, SH_Pin, Start, Second); // Проталкиваем 1-й байт.
  shiftOut(Ds_Pin, SH_Pin, Start, First);  // Проталкиваем 2-й байт.
  } else {
  shiftOut(Ds_Pin, SH_Pin, Start, First);  // Проталкиваем 2-й байт.
  shiftOut(Ds_Pin, SH_Pin, Start, Second); // Проталкиваем 1-й байт.
  }
  digitalWrite(ST_Pin, HIGH);              // Сбрасываем на выходы полученные биты.
  delay(170);                              // Задержка между включением ступеней.
}
