#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <Ethernet.h>
#include "config.h"

LiquidCrystal_I2C lcd(0x27,20,4);  // 设置LCD地址0x27，显示4行20字符

int jdqPin1=22;//定义继电器接口
int jdqPin2=23;
int jdqPin3=24;
int jdqPin4=25;
volatile int tz = -8;

// 温度使用one-wire 46端口
#define ONE_WIRE_BUS 46


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);



//温度变量
float temp3231, temp18B20;

//byte seconds, minutes, hours, day, date, month, year;
char weekDay[4];


byte m_second, m_minute, m_hour, m_dayOfWeek, m_dayOfMonth, m_month, m_year;

void setup(void)
{
  
  lcd.init();
  lcd.backlight();
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  initOutlets();
  
  Wire.begin();

}
 
void loop(void)
{ 
  getDateDs3231(&m_second, &m_minute, &m_hour, &m_dayOfWeek, &m_dayOfMonth, &m_month, &m_year);
  get3231Temp();
  get18B20Temp();
  
  Serial.print(m_dayOfWeek,DEC); Serial.print(", "); Serial.print(m_dayOfMonth, DEC); Serial.print("/"); Serial.print(m_month, DEC); Serial.print("/"); Serial.print(m_year, DEC); Serial.print(" - ");
  Serial.print(m_hour, DEC); Serial.print(":"); Serial.print(m_minute, DEC); Serial.print(":"); Serial.print(m_second, DEC);
  
  Serial.print(" - Temp: "); 
  Serial.println(temp3231);
  
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
 
  Serial.print("Temperature for the device 1 (index 0) is: ");
  Serial.println(temp18B20); 
  
  initLcd();
  
  if(temp18B20 > 27)
  {
    digitalWrite(jdqPin3,HIGH);
    lcd.setCursor(0,2);
    lcd.print("Heater: OFF");
  }else
  {
    digitalWrite(jdqPin3,LOW);
    lcd.setCursor(0,2);
    lcd.print("Heater: ON ");
  }
  
  if(m_hour > 8 && m_hour < 20)
  {
    digitalWrite(jdqPin4,LOW);
    lcd.setCursor(12,2);
    lcd.print("LED: ON ");
  }else
  {
    if(m_hour == 20){
      if(m_minute >= 30){
        digitalWrite(jdqPin4,HIGH);
       
        }
    }
     lcd.setCursor(12,2);
     lcd.print("LED: OFF");
  }
}

