#include <DueFlashStorage.h>
DueFlashStorage dueFlashStorage;


char val;         // variable to receive data from the serial port
int ledpin = 2;  // LED connected to pin 2 (on-board LED)


void setup()
{
  pinMode(ledpin = 13, OUTPUT);  // pin 13 (on-board LED) as OUTPUT
 
  Serial3.begin(9600);       // start serial communication at 115200bps
  
  delay(1000);                  // waits for a second
  digitalWrite(ledpin, LOW);    // turn Off pin 13 off
  delay(1000);                  // waits for a seconds
  digitalWrite(ledpin = 13, HIGH);  // turn ON pin 13 on
      delay(1000);                  // waits for a second
  digitalWrite(ledpin, LOW);    // turn Off pin 13 off
  delay(1000);                  // waits for a second
  digitalWrite(ledpin = 13, HIGH);  // turn ON pin 13 on
  delay(1000);                  // waits for a second
  digitalWrite(ledpin, LOW);    // turn Off pin 13 off
  delay(1000);                  // waits for a seconds
  digitalWrite(ledpin = 13, HIGH);  // turn ON pin 13 on
  delay(1000);                  // waits for a second
  digitalWrite(ledpin, LOW);    // turn Off pin 13 off
  delay(1000);                  // waits for a second 
  Serial3.println("gimmy a 1 digit number: ");
 
}
 
void loop() {
  if(Serial3.available() > 0)       // if data is available to read
  {
    val = Serial3.read();         // read it and store it in 'val'
    if( isDigit(val))
    {
      byte b2[sizeof(val)];
      memcpy(b2, &val, sizeof(char));
      dueFlashStorage.write(0, b2, sizeof(char));
      Serial3.print("Wrote ");
      Serial3.print(val);
      Serial3.println(" to flash");
    }
    
    else
    {
      byte* b = dueFlashStorage.readAddress(0);
      char fromFlash;
      memcpy(&fromFlash, b, sizeof(char));
      Serial3.print("Read ");
      Serial3.print(fromFlash);
      Serial3.println(" from flash");
    }
    
    delay(1000);
  }
}
