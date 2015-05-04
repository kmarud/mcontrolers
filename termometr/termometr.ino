#include <OneWire.h>
#include <LiquidCrystal.h>
#include <IRremote.h>


OneWire  ds(10);
LiquidCrystal lcd(7, 8, 5, 12, 11, 9);
IRsend irsend;


int zmiennaGlobalna=0;
int docelowa=30;
volatile boolean automat=true;
 
 
 
void setup(void) {
  Serial.begin(9600);
  lcd.begin(16, 2);
  
 attachInterrupt(0, ffff, CHANGE);
  pinMode(A0, INPUT);
  pinMode(2, INPUT_PULLUP);
}

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;

  if ( !ds.search(addr)) {
    delay(250);
    return;
  }
  
  type_s = 0;
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        
  
  delay(1000);     
  
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); 

  for ( i = 0; i < 9; i++) {          
    data[i] = ds.read();
  }

  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  
  docelowa=analogRead(A0);
  docelowa=map(docelowa,0,1023,20,40);
  Serial.print(celsius);
  
  lcd.home();
  lcd.print("Temp: ");
  lcd.setCursor(8, 0);
  
  lcd.print(celsius);
  lcd.write(223);
  lcd.print("C");
  if(automat==true)
  {
  Serial.print(" , tryb auto\n");
  lcd.setCursor(0, 1);
  lcd.print("Zadana: ");
  lcd.print(docelowa);
   lcd.write(223);
  lcd.print("C");
  }
  else
  {
    Serial.print("\n");
     lcd.setCursor(0, 1);
     lcd.print("                ");
  }
  zmiennaGlobalna++;
   if((int)celsius<docelowa && zmiennaGlobalna>=5 && automat==true)
   {
     irsend.sendSony(0x937BB355,32); // Sony TV power code
     zmiennaGlobalna=0;
   }
   else if((int)celsius>docelowa && zmiennaGlobalna>=5 && automat==true)
   {
     irsend.sendSony(0xD1D4CC60,32); // Sony TV power code
     zmiennaGlobalna=0;
   }
}


void ffff()
{
  automat=digitalRead(2);
}
