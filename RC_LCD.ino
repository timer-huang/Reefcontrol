

void initLcd(){
  lcd.home(); 
  lcd.setCursor(0, 0);
  lcd.print("Time:");lcd.print(m_hour, DEC); lcd.print(":"); lcd.print(m_minute, DEC); lcd.print(":");lcd.print(" - ");
  lcd.print("Day:");lcd.print(m_dayOfMonth,DEC);
  lcd.setCursor(0, 1);
  lcd.print("T1:");
  lcd.print(temp3231);
  lcd.print(" -- ");
  lcd.print("T2:");
  lcd.print(temp18B20);
}
