#include <Time.h>  

void setup()  {
  Serial.begin(9600);
  tmElements_t tm;

  tm.Year  = 2014 - 1970;
  tm.Month =  2;
  tm.Day   = 14;
  tm.Hour  = 15;
  tm.Minute=  0;
//  time_t pctime = makeTime(tm);
//  setTime(pctime);
}

void loop(){
  digitalClockDisplay();  
  delay(1000);
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(year());
  Serial.print("/");
  Serial.print(month());
  Serial.print("/");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
