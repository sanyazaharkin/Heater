#include <GyverPID.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DS3231.h>
#include <SoftwareSerial.h>



#define ONE_WIRE_BUS 12 // линия данных подключена к цифровому выводу 12 Arduino
#define TEN_PIN 10 //пин на реле ТЭНа


OneWire oneWire(ONE_WIRE_BUS);// настройка объекта oneWire для связи с любым устройством OneWire
DallasTemperature sensors(&oneWire);// передать ссылку на oneWire библиотеке DallasTemperature
DS3231 Clock;
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
SoftwareSerial mySerial(3, 2);          // Выводы SIM800L Tx & Rx подключены к выводам Arduino 3 и 2
GyverPID regulator_1(0.1, 0.05, 0.01, 10);

int second, minute, hour, date, month, year;
bool h12 = false; //True is 12-h, false is 24-hour.;
bool PM;
bool Century = false;
int deviceCount = 0;
float tempC;



void setup()
{
  Wire.begin();
  lcd.init();// initialize the lcd
  sensors.begin();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hello, world!");
  deviceCount = sensors.getDeviceCount();

  Serial.begin(19200);                   // Инициализация последовательной связи с Arduino и Arduino IDE (Serial Monitor)
  mySerial.begin(19200);                 // Инициализация последовательной связи с Arduino и SIM800L
  Serial.println("Initializing...");    // Печать текста
  delay(1000);                          // Пауза 1 с

  while (!mySerial.available())
  { // Зацикливаем и ждем инициализацию SIM800L
    mySerial.println("AT");                  // Отправка команды AT
    delay(1000);                             // Пауза
    Serial.println("Connecting…");         // Печатаем текст
  }
  Serial.println("Connected!");            // Печатаем текст
  mySerial.println("AT+CMGF=1");           // Отправка команды AT+CMGF=1
  delay(1000);                             // Пауза
  mySerial.println("AT+CNMI=1,2,0,0,0");   // Отправка команды AT+CNMI=1,2,0,0,0
  delay(1000);                             // Пауза
  mySerial.println("AT+CMGL=\"REC UNREAD\"");


  //sms(String("HelloSMS"),String("+79822679654"));


  regulator_1.setDirection(NORMAL); // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL
  regulator_1.setLimits(0, 255);    // пределы (ставим для 8 битного ШИМ). ПО УМОЛЧАНИЮ СТОЯТ 0 И 255
  regulator_1.setpoint = 50;        // сообщаем регулятору температуру, которую он должен поддерживать

  // в процессе работы можно менять коэффициенты
  //regulator_1.Kp = 5.2;
  //regulator_1.Ki += 0.5;
  //regulator_1.Kd = 0;
}

void loop()
{
  updateDisplay();
  updateSerial();

  int temp;                 // читаем с датчика температуру
  regulator_1.input = temp;   // сообщаем регулятору текущую температуру

  // getResultTimer возвращает значение для управляющего устройства
  // (после вызова можно получать это значение как regulator_1.output)
  // обновление происходит по встроенному таймеру на millis()
  analogWrite(TEN_PIN, regulator_1.getResultTimer());  // отправляем на мосфет

  // .getResultTimer() по сути возвращает regulator_1.output
}

void getSensorsData()
{
  sensors.requestTemperatures();
  year = Clock.getYear();
  month = Clock.getMonth(Century);
  date = Clock.getDate();
  hour = Clock.getHour(h12, PM);
  minute = Clock.getMinute();
  second = Clock.getSecond();

}

void updateDisplay()
{
  getSensorsData();

  lcd.setCursor(0, 0);
  lcd.print("1:");
  lcd.print(sensors.getTempCByIndex(0));
  lcd.print(",2:");
  lcd.print(sensors.getTempCByIndex(1));



}

void updateSerial()
{
  delay(500);                           // Пауза 500 мс
  while (Serial.available())
  {
    mySerial.write(Serial.read());      // Переадресация с последовательного порта SIM800L на последовательный порт Arduino IDE
  }
  while (mySerial.available())
  {
    Serial.write(mySerial.read());      // Переадресация c Arduino IDE на последовательный порт SIM800L
  }
}




void sms(String text, String phone)  // Процедура Отправка SMS
{
  Serial.println("SMS send started");
  mySerial.println("AT+CMGS=\"" + phone + "\"");
  delay(500);
  mySerial.print(text);
  delay(500);
  mySerial.print((char)26);
  delay(500);
  Serial.println("SMS send complete");
  delay(2000);
}
