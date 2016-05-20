#include <LinkedList.h>
#include <DueFlashStorage.h>
DueFlashStorage dueFlashStorage;


//1 to print debug statements to Serial (serial monitor), 0 to turn off
#define DEBUG 1

int analogPin = 0;     // potentiometer wiper (middle terminal) connected to analog pin 3
                       // outside leads to ground and +5V
#define REDPIN 9
#define GREENPIN 10
#define BLUEPIN 11 
#define PERSON 8
#define MULTIPLIER 2.43
int weightSample[20];
int i = 0;
int lastVal = 0;
int baseVal;
String input = "";
int currentMenu = 1;
int range = 5;
int storeWeight = 0;

struct Database
{
  String user;
  int weight;
  int color;
};
typedef struct Database database;

LinkedList<database*> profiles = LinkedList<database*>();
database *newProfile;
///////////when we remove an item make sure to delete the item
int numProfiles;


void setup()
{
  //setup listening on Bluetooth connection (to modify profile info from computer script)
  Serial3.begin(9600);

  //set up pin modes on arduino
  pinMode(PERSON, INPUT);
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  //set inital value of LED strip to white (r, g, b, max values)
  analogWrite(REDPIN, 255);
  analogWrite(GREENPIN, 255);
  analogWrite(BLUEPIN, 255);

  //get user profiles from flash memory
  //check if we have written to the flash before
  uint8_t firstRun = dueFlashStorage.read(0);
  //if we have not written to the memory location before, the byte value will be 255
  if(firstRun)
  {
    dueFlashStorage.write(0,0);    //write 0 to byte at address 0 to signal that we have written to the flash
    dueFlashStorage.write(4,0);    //write 0 to byte at address 1 to hold the number of profiles to get
    numProfiles = 0;
  }
  
  else
  {
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
  }
}



void loop()
{
  //Get input from computer (over bluetooth)
  if (Serial3.available() > 0)  // do nothing if no serial connection
  {
    //serial connection sends one byte (character) at a time, grab the byte and append to string buffer
    char state = Serial3.read();
    input.concat(state);

    //comma delimiter to signal end of message
    if (state == ',')
    {
      //messages sent from computer are in key value pairs separated by an '=' without spaces
      String inputKey = input.substring(0, input.indexOf('='));
      String inputVal = input.substring(input.indexOf('=')+1, input.indexOf(','));
      
      if(inputKey == "menu")
      {
        currentMenu = inputVal.toInt();
      }

      else if(inputKey == "name" && currentMenu == 1)
      {
        newProfile = new database;
        newProfile->user = inputVal;
      }

      else if(inputKey == "color")
      {
        newProfile->color = inputVal.toInt();
      }

      else if(inputKey == "weight")
      {
        storeWeight = inputVal.toInt();
      }

      else
      {
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
    int average;
    while(redo == 1)
    {
      average = 0;
      redo = 0;
      for(i = 0; i < 20; ++i)
      {
        weightSample[i] = analogRead(analogPin);
        average += weightSample[i];
        if ((((average / i) + range) < weightSample[i]) || (((average / i) - range) > weightSample[i]))
        {
          redo = 1;
          break;
        }
      }

      if (redo == 0)
      {
        lastVal = average;
      }
    }
    
    //compare lastVal to profiles
    if(storeWeight == 0)
    {
      //compare
      //set light
    }

    //store weight into new profile
    else if(storeWeight == 1 && currentMenu == 1)
    {
      newProfile->weight = average;
      //store into our profile database
      //modify flash
    }

    //delay loop wait for user to step off scale
    while(digitalRead(PERSON) == LOW)
    {
      delay(200);
    }

  }

  // Button on scale not pressed, nobody on the scale
  else
  {
    baseVal = analogRead(analogPin);
  }

  
}


void write2Flash()
{
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

