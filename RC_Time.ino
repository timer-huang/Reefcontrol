byte tMSB, tLSB;

char* getdatestring(const time_t t, char* buffer) {
  tmElements_t tm;
  char* ret = buffer;
  breakTime(t, tm);
  *buffer++ = tm.Month/10 + '0';
  *buffer++ = tm.Month%10 + '0';
  *buffer++ = '/';
  *buffer++ = tm.Day/10 + '0';
  *buffer++ = tm.Day%10 + '0';
  *buffer++ = '/';
  uint16_t yr = tmYearToCalendar(tm.Year);
  *buffer++ = yr/1000 + '0';
  *buffer++ = yr%1000/100 + '0';
  *buffer++ = yr%100/10 + '0';
  *buffer++ = yr%10 + '0';
  *buffer++ = ' ';
  *buffer++ = tm.Hour/10 + '0';
  *buffer++ = tm.Hour%10 + '0';
  *buffer++ = ':';
  *buffer++ = tm.Minute/10 + '0';
  *buffer++ = tm.Minute%10 + '0';
  *buffer++ = ':';
  *buffer++ = tm.Second/10 + '0';
  *buffer++ = tm.Second%10 + '0';
  *buffer = 0;
  return ret;
}

byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)           // Convert binary coded decimal to normal decimal numbers
{
  return ( (val/16*10) + (val%16) );
}


                                  // 1) Sets the date and time on the ds3231
                                  // 2) Starts the clock
                                  // 3) Sets hour mode to 24 hour clock
                                  // Assumes you're passing in valid numbers

void setDateDs3231(byte second,   // 0-59
byte minute,                      // 0-59
byte hour,                        // 1-23
byte dayOfWeek,                   // 1-7
byte dayOfMonth,                  // 1-28/29/30/31
byte month,                       // 1-12
byte year)                        // 0-99
{
  Wire.beginTransmission(RTC_ADDR);
  Wire.write(0);
  Wire.write(decToBcd(second));    // 0 to bit 7 starts the clock
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));      // If you want 12 hour am/pm you need to set
                                  // bit 6 (also need to change readDateDs1307)
  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}

                                  // Gets the date and time from the ds1307
void getDateDs3231(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  // Reset the register pointer
  Wire.beginTransmission(RTC_ADDR);
  Wire.write(0);
  Wire.endTransmission();

  Wire.requestFrom(RTC_ADDR, 7);

  // A few of these need masks because certain bits are control bits
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f); // Need to change this if 12 hour am/pm
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
/* ******************************************************************************************************************** */
/* *                                                                                                                  * */
/* *                                 D E F I N E  :  O N E S E C O N D                                                * */
/* *                                                                                                                  * */
/* ******************************************************************************************************************** */

void onesecond() //function that runs once per second while program is running
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  getDateDs3231(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
/*  lcd.setCursor(0, 0);
  if(hour>0)
  {
    if(hour<=12)
    {
      lcd.print(hour, DEC);
    }
    else
    {
      lcd.print(hour-12, DEC);
    }
  }
  else
  {
    lcd.print("12");
  }
  lcd.print(":");
  if (minute < 10) {
    lcd.print("0");
  }
  lcd.print(minute, DEC);
  lcd.print(":");
  if (second < 10) {
    lcd.print("0");
  }
  lcd.print(second, DEC);
  if(hour<12)
  {
    lcd.print("am");
  }
  else
  {
    lcd.print("pm");
  }
  lcd.print(" ");
  delay(1000);*/
}

float get3231Temp()
{
  //temp registers (11h-12h) get updated automatically every 64s
  Wire.beginTransmission(RTC_ADDR);
  Wire.write(0x11);
  Wire.endTransmission();
  Wire.requestFrom(RTC_ADDR, 2);
  
  if(Wire.available()) {
    tMSB = Wire.read(); //2's complement int portion
    tLSB = Wire.read(); //fraction portion
    
    temp3231 = (tMSB & B01111111); //do 2's math on Tmsb
    temp3231 += ( (tLSB >> 6) * 0.25 ); //only care about bits 7 & 8
  }
  else {
    //oh noes, no data!
  }
    
  return temp3231;
}
