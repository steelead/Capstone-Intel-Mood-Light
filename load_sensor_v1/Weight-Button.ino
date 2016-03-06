int analogPin = 0;     // potentiometer wiper (middle terminal) connected to analog pin 3
                       // outside leads to ground and +5V
int val1 = 0;
int val = 0;           // variable to store the value read
int val2 = 0;

#define PERSON 8



void setup()
{
  Serial.begin(9600);          //  setup serial
  pinMode(PERSON, INPUT);
}



void loop()

{
  if(digitalRead(PERSON) == HIGH)
  {
    Serial.print("HIGH ");
    val1 = analogRead(analogPin);
    val2 = val1;
  }
  else
  {
    Serial.print("LOW ");
    val2 = analogRead(analogPin);
  }

  Serial.print(val2);
  Serial.print(' ');
  Serial.print((val2-val1)*1.163);             // debug value
  Serial.print(" lbs\n");
  delay(250);
}
