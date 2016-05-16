int analogPin = 0;     // potentiometer wiper (middle terminal) connected to analog pin 3
                       // outside leads to ground and +5V
int val1 = 0;
int val = 0;           // variable to store the value read
int val2 = 0;
#define REDPIN 9
#define GREENPIN 10
#define BLUEPIN 11 
#define PERSON 8
int read1 = 0;
int read2 = 0;
int read3 = 0;
int read4 = 0;
int lastVal = 0;
#define MULTIPLIER 2.43


void setup()
{
  Serial.begin(9600);          //  setup serial
  pinMode(PERSON, INPUT);
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  analogWrite(REDPIN, 255);
  analogWrite(GREENPIN, 255);
  analogWrite(BLUEPIN, 255);
}



void loop()
{
  if(digitalRead(PERSON) == HIGH)
  {
    Serial.print("HIGH ");
    val1 = analogRead(analogPin);
    val2 = val1;
    delay(1000);
  }
  else
  {
    Serial.print("LOW ");
    delay(1000);
    val2 = analogRead(analogPin);
    lastVal = (val2-val1)*MULTIPLIER;
  }

  if (lastVal < 170 && lastVal > 130)
  {
    analogWrite(REDPIN, 255);
    analogWrite(GREENPIN, 0);
    analogWrite(BLUEPIN, 0);
  }
  else if (lastVal < 120 && lastVal > 80)
  {
    analogWrite(REDPIN, 0);
    analogWrite(GREENPIN, 255);
    analogWrite(BLUEPIN, 0);
  }
  else
  {
    analogWrite(REDPIN, 0);
    analogWrite(GREENPIN, 0);
    analogWrite(BLUEPIN, 255);
  }

  Serial.print(lastVal);
  Serial.print(' ');
  Serial.print(val2);
  Serial.print(' ');
  Serial.print((val2-val1)*MULTIPLIER);             // debug value
  Serial.print(" lbs\n");
}
