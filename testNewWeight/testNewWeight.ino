
int analogPin = 0;     // potentiometer wiper (middle terminal) connected to analog pin 3

#define PERSON 8
#define MULTIPLIER 2.43
#define NUMSAMPLES 1000
float weightSample[NUMSAMPLES];
int i;
int lastVal = 0;
int baseVal;
int range = 15;
int storeWeight = 0;




void setup()
{
  //setup listening on Bluetooth connection (to modify profile info from computer script)
  Serial.begin(9600);
  pinMode(PERSON, INPUT);

}



void loop()
{


  // Button on scale is pressed, someone on the scale
  if(digitalRead(PERSON) == LOW)
  {
    delay(200);
    int redo = 1;
    float average;
    while(redo == 1)
    {
      average = 0;
      redo = 0;
      Serial.println("Sampling");
      for(i = 0; i < NUMSAMPLES; ++i)
      {
        delay(1);
        weightSample[i] = analogRead(analogPin);
        //Serial.print("value read from analog input: ");
        //Serial.println(weightSample[i]);
        //Serial.print("baseVal = ");
        //Serial.println(baseVal);
        weightSample[i] = (weightSample[i] - baseVal) * 2.43;
        
        //Serial.print("weight sample: ");
        //Serial.println(weightSample[i]);
    
        average += weightSample[i];
        //Serial.print("average = ");
        //Serial.println(average / (i + 1));

        if ((((average / (i+1)) + range) < weightSample[i]) || (((average / (i+1)) - range) > weightSample[i]))
        {
          Serial.print("out of range: average = ");
          Serial.print(average/(i+1));
          Serial.print(" weightsample = ");
          Serial.println(weightSample[i]);
          //delay(5000);
          redo = 1;
          break;
        }
      }
      if(redo == 0)
      {
        for(i = 0; i < NUMSAMPLES; ++i)
        {
          if ((((average / (NUMSAMPLES)) + range) < weightSample[i]) || (((average / (NUMSAMPLES)) - range) > weightSample[i]))
          {
            Serial.print("out of range: average = ");
            Serial.print(average/(NUMSAMPLES));
            Serial.print(" weightsample = ");
            Serial.println(weightSample[i]);
            //delay(5000);
            redo = 1;
            break;
          }
        }
      }
      

      if (redo == 0)
      {
        lastVal = average / NUMSAMPLES;
      }
    }
    
    //compare lastVal to profiles

    //delay loop wait for user to step off scale
    while(digitalRead(PERSON) == LOW)
    {
      Serial.println("waiting for person to step off scale");
      delay(200);
    }
    Serial.print("saved weight = ");
    Serial.println(lastVal);
    delay(1000);
  }

  // Button on scale not pressed, nobody on the scale
  else
  {
    Serial.println("Nobody on scale");
    baseVal = analogRead(analogPin);
  }

  delay(200);
}
