#include <Arduino.h>
/*#include <microWire.h>
#include <microLiquidCrystal_I2C.h>
#include <microDS18B20.h>
#include <microDS3231.h>*/
#include <GyverPID.h>
#include <microWire.h>
#include <microDS18B20.h>
#include <microDS3231.h>
#include <microLiquidCrystal_I2C.h>
#include <SoftwareSerial.h>


#define ONE_WIRE_BUS 12 // линия данных подключена к цифровому выводу 12 Arduino
#define TEN_PIN 10 //пин на реле ТЭНа


SoftwareSerial mySerial(3, 2);          // Выводы SIM800L Tx & Rx подключены к выводам Arduino 3 и 2


int second, minute, hour, date, month, year;
bool h12 = false; //True is 12-h, false is 24-hour.;
bool PM;
bool Century = false;
int deviceCount = 0;
float tempC;



LiquidCrystal_I2C lcd(0x27, 16, 2);
// адрес дисплея 0x3f или 0x27
// ширина дисплея (тут 20 символов)
// высота дисплея (тут 4 строки)

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
void setup()
{
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Hello, world!");
  lcd.setCursor(2, 1);
  lcd.print("Ywrobot Arduino!");



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
}


/*void getSensorsData()
{
  sensors.requestTemperatures();
  year = Clock.getYear();
  month = Clock.getMonth(Century);
  date = Clock.getDate();
  hour = Clock.getHour(h12, PM);
  minute = Clock.getMinute();
  second = Clock.getSecond();

}*/

void updateDisplay()
{
  //getSensorsData();

  lcd.setCursor(0, 0);
  lcd.print(tempC);
  tempC++;



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






void loop()
{
  //updateDisplay();
  updateSerial();

}