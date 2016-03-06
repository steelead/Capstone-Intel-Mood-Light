int analogPin = 0;     // potentiometer wiper (middle terminal) connected to analog pin 3
                       // outside leads to ground and +5V
int val1 = 0;
int val = 0;           // variable to store the value read
int val2 = 0;


void setup()
{
  Serial.begin(9600);          //  setup serial
}



void loop()

{
  val1 = analogRead(analogPin);    // read the input pin
  if(val1 > 700)
  {
    val2 = val1;
    Serial.print(val1);
    Serial.print(" ");
  }

  else
  {
    val = analogRead(analogPin);
    val2 = val;
    Serial.print(val);
    Serial.print(" ");
  }
  
  Serial.print((val2-val)*1.13);             // debug value
  Serial.print(" lbs\n");
  delay(250);
}
