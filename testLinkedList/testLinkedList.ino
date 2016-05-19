#include <DueFlashStorage.h>
DueFlashStorage dueFlashStorage;
#include <LinkedList.h>

struct Database
{
  char *user;
  int weight;
  int color;
};
typedef struct Database database;

database newProfile;
LinkedList<database*> profiles = LinkedList<database*>();
int numProfiles;

unsigned int currentAddress;

void setup() {
  Serial.begin(9600);
  delay(5000);
  Serial.println("5 sec left");
  delay(5000);
  database *newProfile = new database;
  // put your setup code here, to run once:
  newProfile->user = "Bob";
  newProfile->weight = 150;
  newProfile->color = 4;
  profiles.add(newProfile);

  newProfile = new database;
  newProfile->user = "Mr. Tilt";
  newProfile->weight = 700;
  newProfile->color = 2;
  profiles.add(newProfile);

  newProfile = new database;
  newProfile->user = "Shazbot";
  newProfile->weight = 130;
  newProfile->color = 3;
  profiles.add(newProfile);

  numProfiles = 3;

  dueFlashStorage.write(0, 0);
  byte b3[sizeof(int)];
  memcpy(b3, &numProfiles, sizeof(int));
  dueFlashStorage.write(4, b3, sizeof(int));
  
  currentAddress = 4;
  currentAddress += (sizeof(int) * 4);
  int i;
  for(i = 0; i < numProfiles; ++i)
  {
    byte b2[sizeof(database)];
    memcpy(b2, profiles.get(i), sizeof(database));
    dueFlashStorage.write(currentAddress, b2, sizeof(database));
    currentAddress += (sizeof(database) * 4);
  }

  database *tempProfile;
  for(i = numProfiles - 1; i >= 0; --i)
  {
    tempProfile = profiles.remove(i);
    delete tempProfile;
  }

  Serial.print(profiles.size());
    
}

void loop() {
  //get number of profiles
  byte *b = dueFlashStorage.readAddress(4);
  memcpy(&numProfiles, b, sizeof(int));
  Serial.print("numProfiles = ");
  Serial.println(numProfiles);

  //get the profiles
  int i;
  unsigned int currentAddress = 4;
  currentAddress += (sizeof(int) * 4);
  database * tempProfile;
  for(i = 0; i < numProfiles; ++i)
  {
    tempProfile = new database;
    byte *b2 = dueFlashStorage.readAddress(currentAddress);
    memcpy(tempProfile, b2, sizeof(database));
    profiles.add(tempProfile);
    currentAddress += (sizeof(database) * 4);
  }

  for(i = 0; i < numProfiles; ++i)
  {
    Serial.print("profiles[");
    Serial.print(i);
    Serial.print("]: user = ");
    Serial.print(profiles.get(i)->user);
    Serial.print(", weight = ");
    Serial.print(profiles.get(i)->weight);
    Serial.print(", color = ");
    Serial.println(profiles.get(i)->color);
  }

  Serial.print("Added new profiles: ");
  Serial.println(profiles.size());
  
  for(i = numProfiles - 1; i >= 0; --i)
  {
    tempProfile = profiles.remove(i);
    delete tempProfile;
  }

  Serial.println("Removed all(?) profiles: ");
  Serial.println(profiles.size());
  delay(5000);
}
