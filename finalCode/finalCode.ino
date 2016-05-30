#include <LinkedList.h>
#include <DueFlashStorage.h>
DueFlashStorage dueFlashStorage;


//1 to print debug statements to Serial (serial monitor), 0 to turn off
#define DEBUG 1

#ifdef DEBUG
  #define DEBUG_PRINT(x)  Serial.print(x)
#else
  #define DEBUG_PRINT(x)
#endif

#define SCALE_ANALOG_PIN 0     // potentiometer wiper (middle terminal) connected to analog pin 3
                       // outside leads to ground and +5V
#define REDPIN 9
#define GREENPIN 10
#define BLUEPIN 11 
#define PERSON 8
#define MULTIPLIER 2.43
#define MAXUSERLEN 50
#define NUMSAMPLES 500
#define CALIBRATE_WEIGHT_LED_PIN 13

int weightSample[NUMSAMPLES];
int i;
int lastVal = 0;
int baseVal;
String input = "";
int currentMenu = 1;
int range = 5;
int storeWeight = 0;
int editProfileIndex = 0;

struct Database
{
  char user[MAXUSERLEN];
  int weight;
  int color;
};

typedef struct Database database;

LinkedList<database*> profiles = LinkedList<database*>();
database *newProfile;
int numProfiles;


void setup()
{
  //setup listening on Bluetooth connection (to modify profile info from computer script)
  Serial3.begin(9600);
  
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  DEBUG_PRINT("Connected to bluetooth\n");
  
  //set up pin modes on arduino
  pinMode(PERSON, INPUT);
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  pinMode(CALIBRATE_WEIGHT_LED_PIN, OUTPUT);

  //test LED strip by turning white for half a second
  analogWrite(REDPIN, 255);
  analogWrite(GREENPIN, 255);
  analogWrite(BLUEPIN, 255);
  delay(500);
  analogWrite(REDPIN, 0);
  analogWrite(GREENPIN, 0);
  analogWrite(BLUEPIN, 0);

  //get user profiles from flash memory
  //check if we have written to the flash before
  uint8_t firstRun = dueFlashStorage.read(0);
  //if we have not written to the memory location before, the byte value will be 255
  if(firstRun)
  {
    DEBUG_PRINT("Have not written to flash before, no profiles in memory yet\n");
    dueFlashStorage.write(0,0);    //write 0 to byte at address 0 to signal that we have written to the flash before
    byte b[sizeof(int)];
    unsigned int currentAddress = 4;
    numProfiles = 0;
    memcpy(b, &numProfiles, sizeof(int));
    dueFlashStorage.write(currentAddress, b, sizeof(int));     //write that the number of profiles we have stored is 0
  }
  
  else
  {
    DEBUG_PRINT("Flash has been written to before, retrieving profiles.\n");
    //get number of profiles (stored at address 4)
    unsigned int currentAddress = 4;
    byte *b = dueFlashStorage.readAddress(currentAddress);
    memcpy(&numProfiles, b, sizeof(int));

    //get profiles (stored in following addresses)
    currentAddress += (sizeof(int) * 4);
    database *tempProfile;
    for(i = 0; i < numProfiles; ++i)
    {
      tempProfile = new database;
      byte *b2 = dueFlashStorage.readAddress(currentAddress);
      memcpy(tempProfile, b2, sizeof(database));
      profiles.add(tempProfile);
      currentAddress += (sizeof(database) * 4);
    }

    DEBUG_PRINT("Number of profiles retrieved from memory: ");
    DEBUG_PRINT(numProfiles);
    DEBUG_PRINT("\n");
  }
}



void loop()
{
  //Get input from computer (over bluetooth)
  if (Serial3.available() > 0)  // do nothing if no serial connection
  {
    //serial connection sends one byte (character) at a time, grab the byte and append to string buffer
    char state = Serial3.read();
    input += state;

    //comma delimiter to signal end of message
    if (state == ',')
    {
      DEBUG_PRINT("Message over bluetooth recieved: ");
      DEBUG_PRINT(input);
      DEBUG_PRINT("\n");
      
      //messages sent from computer are in key value pairs separated by an '=' without spaces
      String inputKey = input.substring(0, input.indexOf('='));
      String inputVal = input.substring(input.indexOf('=')+1, input.indexOf(','));
      
      if(inputKey == "menu")
      {
        currentMenu = inputVal.toInt();

        //send over bluetooth all the profiles we have in the system
        if (currentMenu == 2)
        {
          DEBUG_PRINT("currentMenu = 2, returning profiles to user interface program.\n");
          Serial3.print("numProfiles=");
          Serial3.println(numProfiles);
          
          for(i = 0; i < numProfiles; ++i)
          {
            Serial3.print("user=");
            Serial3.print(profiles.get(i)->user);
            Serial3.print(", weight=");
            Serial3.print(profiles.get(i)->weight);
            Serial3.print(", color=");
            Serial3.println(profiles.get(i)->color);
          }
        }
      }

      else if(inputKey == "name" && currentMenu == 1)
      {
        DEBUG_PRINT("currentMenu = 1, got new profile name.\n");
        newProfile = new database;
        inputVal.toCharArray(newProfile->user, MAXUSERLEN);
      }

      else if(inputKey == "color" && currentMenu == 1)
      {
        DEBUG_PRINT("currentMenu = 1, got new profile color.\n");
        newProfile->color = inputVal.toInt();
      }

      else if(inputKey == "weight" && currentMenu == 1)
      {
        DEBUG_PRINT("currentMenu = 1, got indication to store the next weight calibrated as a new profile\n");
        storeWeight = inputVal.toInt();
      }

      else if(inputKey == "profile" && currentMenu == 3)
      {
        DEBUG_PRINT("currentMenu = 3, got profile index to modify.\n");
        editProfileIndex = inputVal.toInt();
      }

      else if (inputKey == "name" && currentMenu == 3)
      {
        DEBUG_PRINT("currentMenu = 3, got name to modify.\n");
        database *aProfile;
        aProfile = profiles.get(editProfileIndex);
        inputVal.toCharArray(aProfile->user, MAXUSERLEN);
        profiles.set(editProfileIndex, aProfile);
        write2flash();
      }

      else if (inputKey == "color" && currentMenu == 3)
      {
        DEBUG_PRINT("currentMenu = 3, got color to modify.\n");
        database *aProfile;
        aProfile = profiles.get(editProfileIndex);
        aProfile->color = inputVal.toInt();
        profiles.set(editProfileIndex, aProfile);
        write2flash();
      }

      else if(inputKey == "weight" && currentMenu == 3)
      {
        DEBUG_PRINT("currentMenu = 3, got indication to modify an existing profile for next weight on scale.\n");
        storeWeight = inputVal.toInt();
      }

      else if(inputKey == "profile" && currentMenu == 4)
      {
        DEBUG_PRINT("currentMenu = 4, removing a profile.\n");
        int removeIndex = inputVal.toInt();
        database * aProfile;
        aProfile = profiles.remove(removeIndex);
        DEBUG_PRINT("profile # - ");
        DEBUG_PRINT(removeIndex);
        DEBUG_PRINT("name - ");
        DEBUG_PRINT(aProfile->user);
        DEBUG_PRINT(" color - ");
        DEBUG_PRINT(aProfile->color);
        DEBUG_PRINT(" weight - ");
        DEBUG_PRINT(aProfile->weight);
        DEBUG_PRINT("\n");
        delete aProfile;
        --numProfiles;
        write2flash();
      }

      else
      {
        DEBUG_PRINT("Unrecognized input received from user program.");
        Serial3.println("Error: unrecognized input");
      }

      //Clear the serial3 (bluetooth) input buffer
      input = "";
    }
  }



  // Button on scale is pressed, someone on the scale
  if(digitalRead(PERSON) == LOW)
  {
    int redo = 1;
    int error = 0;
    float average;
    float weightSum;

    DEBUG_PRINT("Someone is on the scale.\nSetting led pin on DUE high.\nCalibrating weight.\n");

    digitalWrite(CALIBRATE_WEIGHT_LED_PIN, HIGH);
    
    while(redo == 1)
    {
      weightSum = 0;
      redo = 0;
      for(i = 0; i < NUMSAMPLES; ++i)
      {
        weightSample[i] = analogRead(SCALE_ANALOG_PIN);     //get sample
        weightSample[i] = (weightSample[i] - baseVal) * 2.43;     //convert weight to pounds
        
        weightSum += weightSample[i];
        
        //check if sample is outside of acceptable range
        if ((((weightSum / (i+1)) + range) < weightSample[i]) || 
            (((weightSum / (i+1)) - range) > weightSample[i]))
        {
          redo = 1;
          break;
        }

        //if the person stepped off the scale before we were able to finish calibrating the weight
        if(digitalRead(PERSON) == HIGH)
        {
          DEBUG_PRINT("Person stepped off scale before able to fully calibrate weight.\n");
          error = 1;
          break;
        }
      }

      //if person stepped off the scale before we were able to finish calibrating the weight
      if (error == 1)
      {
        break;
      }

      if (redo == 0)
      {
        for(i = 0; i < NUMSAMPLES; ++i)
        {
          if ((((weightSum / NUMSAMPLES) + range) < weightSample[i]) ||
              (((weightSum / NUMSAMPLES) - range) > weightSample[i]))
              redo = 1;
              break;
        }
      }

      if(redo == 0)
      {
        lastVal = int(weightSum / NUMSAMPLES);
        DEBUG_PRINT("Got weight on scale as :");
        DEBUG_PRINT(lastVal);
        DEBUG_PRINT("\n");
      }
    }

    //if person stepped off scale before we were able to finish calibrating the weight
    if(error == 1)
    {
      digitalWrite(CALIBRATE_WEIGHT_LED_PIN, LOW);
    }
    
    //compare lastVal to profiles
    if(storeWeight == 0)
    {
      DEBUG_PRINT("Comparing weight to profiles.\n");
      int userWeight = 0;
      int profileMatch = 0;
      
      //go through the list of profiles and find the closest match
      for(i = 0; i < numProfiles; ++i)
      {
        userWeight = profiles.get(i)->weight;
        if(abs(profiles.get(i)->weight - lastVal) < abs(profiles.get(profileMatch)->weight - lastVal))
        {
          profileMatch = i;
        }
      }
      DEBUG_PRINT("Closest profile weight is: user - ");
      DEBUG_PRINT(profiles.get(profileMatch)->user);
      DEBUG_PRINT(" weight - ");
      DEBUG_PRINT(profiles.get(profileMatch)->weight);
      DEBUG_PRINT("\n");
      
      //set led strip to profile color
      setLEDStrip(profiles.get(profileMatch)->color);
    }

    //store weight into new profile
    else if(storeWeight == 1 && currentMenu == 1)
    {
      DEBUG_PRINT("Storing weight as a new profile.\n");
      newProfile->weight = int(lastVal);
      profiles.add(newProfile);
      ++numProfiles;
      write2flash();
      Serial3.print("weight=");
      Serial3.println(newProfile->weight);
      storeWeight = 0;
    }

    else if(storeWeight == 1 && currentMenu == 3)
    {      
      database *aProfile;
      aProfile = profiles.get(editProfileIndex);
      DEBUG_PRINT("Modifying existing profile weight: profile # - ");
      DEBUG_PRINT(editProfileIndex);
      DEBUG_PRINT(" user - ");
      DEBUG_PRINT(aProfile->user);
      DEBUG_PRINT(" color - ");
      DEBUG_PRINT(aProfile->color);
      DEBUG_PRINT(" oldweight - ");
      DEBUG_PRINT(aProfile->weight);
      DEBUG_PRINT("\n");
      aProfile->weight = int(lastVal);
      profiles.set(editProfileIndex, aProfile);
      write2flash();
      Serial3.print("weight=");
      Serial3.println(newProfile->weight);
      storeWeight = 0;
    }

    //delay loop wait for user to step off scale
    while(digitalRead(PERSON) == LOW)
    {
      DEBUG_PRINT("Done, blinking DUE LED, waiting for person to step off scale.\n");
      digitalWrite(CALIBRATE_WEIGHT_LED_PIN, HIGH);
      delay(200);
      digitalWrite(CALIBRATE_WEIGHT_LED_PIN, LOW);
      delay(200);
    }

  }

  // Button on scale not pressed, nobody on the scale
  else
  {
    baseVal = analogRead(SCALE_ANALOG_PIN);
  }

  
}


void write2flash()
{
  DEBUG_PRINT("Writing to flash.\n");
  byte b[sizeof(int)];
  unsigned int currentAddress = 4;
  int i;
  memcpy(b, &numProfiles, sizeof(int));
  dueFlashStorage.write(currentAddress, b, sizeof(int));

  currentAddress += (sizeof(int) * 4);
  for(i = 0; i < numProfiles; ++i)
  {
    byte b2[sizeof(database)];
    memcpy(b2, profiles.get(i), sizeof(database));
    dueFlashStorage.write(currentAddress, b2, sizeof(database));
    currentAddress += (sizeof(database) * 4);
  }

  return;
}


void setLEDStrip(int color)
{
  int r, g, b;

  //off
  if(color == 0)
  {
    DEBUG_PRINT("Setting LED strip off");
    r = 0;
    g = 0;
    b = 0;
  }
  
  //green
  else if(color == 1)
  {
    DEBUG_PRINT("Setting LED strip to green");
    r = 0;
    g = 255;
    b = 0;
  }

  //red
  else if(color == 2)
  {
    DEBUG_PRINT("Setting LED strip to red");
    r = 255;
    g = 0;
    b = 0;
  }

  //blue
  else if(color == 3)
  {
    DEBUG_PRINT("Setting LED strip to blue");
    r = 0;
    g = 0;
    b = 255;
  }

  //yellow
  else if(color == 4)
  {
    DEBUG_PRINT("Setting LED strip to yellow");
    r = 255;
    g = 255;
    b = 0;
  }

  //white
  else if(color == 5)
  {
    DEBUG_PRINT("Setting LED strip to white");
    r = 255;
    g = 255;
    b = 255;
  }

  //purple
  else if(color == 6)
  {
    DEBUG_PRINT("Setting LED strip to purple");
    r = 238;
    g = 130;
    b = 238;
  }

  //orange
  else if(color == 7)
  {
    DEBUG_PRINT("Setting LED strip to orange");
    r = 255;
    g = 165;
    b = 0;
  }

  analogWrite(REDPIN, r);
  analogWrite(GREENPIN, g);
  analogWrite(BLUEPIN, b);
  
  return;
}

