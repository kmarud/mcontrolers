#include <IRremote.h>
#define irPin 11
IRrecv irrecv(irPin);
decode_results results;
 
 
 
class display1
{
    boolean out[7];
    boolean A,B,C,D;
    boolean aktywny;
  public:
    void func(int liczba);
    void wyswietl(int liczba);
    void wlacz();
    void wylacz();
};
int i;

void display1::func(int liczba)
{
    D = (liczba & 8) > 0;
    C = (liczba & 4) > 0;
    B = (liczba & 2) > 0;
    A = (liczba & 1) > 0;
    
    out[0] = ((!B&&!D)&&!(A&&C)&&!(!A&&!C));
    out[1] = (C&&!(!A&&!B)&&!(A&&B));
    out[2] = (!A&&B&&!C);
    out[3] = (!D&&!(!A&&B)&&!(!C&&!(A&&!B))&&!(A&&!B&&C));
    out[4] = !(!A&&!(!B&&C));
    out[5] = (!(!A&&!B)&&!C&&!D);
    out[6] = (!(!B&&C)&&!D&&!(B&&!(A&&C)));
    
    int licznik=6;
    for (int k=0; k<7; ++k)
    {
      if(out[k])
          digitalWrite(licznik,HIGH);
      else 
          digitalWrite(licznik,LOW);
      --licznik;
    } 
}

void display1::wyswietl(int liczba)
{
  if(aktywny==false)
      return;
    int k= liczba%10;
    digitalWrite(7,LOW);
    digitalWrite(8,HIGH);
    func(k);
    delay(10);
    
    k=liczba/10;
    digitalWrite(7,HIGH);
    digitalWrite(8,LOW);
 
    func(k);
    delay(10);    
 
}

void display1::wylacz()
{
    digitalWrite(8,HIGH);
    digitalWrite(7,HIGH);
    aktywny=false;
    i=0;
}

void display1::wlacz()
{
    aktywny=true;
    i=50;
} 
 

display1 wyswietlacz; 
void setup() {
   irrecv.enableIRIn();
   for(int i=0; i<9; ++i)
     pinMode(i, OUTPUT);
     i=50;
     wyswietlacz.wlacz();
    pinMode(9, OUTPUT);
    //display1 wyswietlacz;
}



void loop(){
  
   if (irrecv.decode(&results)) 
   {
       if((results.value==0xD1D4CC60 || results.value==0x967BB80C )&& i<99)
             i++;
           if((results.value==0xCED4C7A9 || results.value==0x937BB355) && i>0)
             i--;
           if(results.value==0x560DBB35 || results.value==0x1AB4A6E1)
             wyswietlacz.wylacz();
          if(results.value==0x17B4A228 || results.value==0x530DB67C)
             wyswietlacz.wlacz();
      irrecv.resume();
     }
    int val= map(i, 0, 99, 0, 255);
      analogWrite(9, val);
//      analogWrite(13, 255);
    
//  digitalWrite(9,LOW);

      wyswietlacz.wyswietl(i);
}
