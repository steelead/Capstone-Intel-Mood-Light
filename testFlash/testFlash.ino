#include <DueFlashStorage.h>
DueFlashStorage dueFlashStorage;

struct Database
{
  char *user;
  int weight;
  int color;
};
typedef struct Database database;

database newProfile;
database profiles[5];
database flashProfiles[5];
int numProfiles;

unsigned int currentAddress;

void setup() {
  Serial.begin(9600);
  delay(5000);
  Serial.println("5 sec left");
  delay(5000);
  // put your setup code here, to run once:
  profiles[0].user = "Bob";
  profiles[0].weight = 150;
  profiles[0].color = 4;

  profiles[1].user = "Mr. Tilt";
  profiles[1].weight = 700;
  profiles[1].color = 2;

  profiles[2].user = "Shazbot";
  profiles[2].weight = 130;
  profiles[2].color = 3;

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
    memcpy(b2, &profiles[i], sizeof(database));
    dueFlashStorage.write(currentAddress, b2, sizeof(database));
    currentAddress += (sizeof(database) * 4);
  }
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
  database tempProfile[5];
  for(i = 0; i < numProfiles; ++i)
  {
    byte *b2 = dueFlashStorage.readAddress(currentAddress);
    memcpy(&tempProfile[i], b2, sizeof(database));
    Serial.print("tempProfile[");
    Serial.print(i);
    Serial.print("]: user = ");
    Serial.print(tempProfile[i].user);
    Serial.print(", weight = ");
    Serial.print(tempProfile[i].weight);
    Serial.print(", color = ");
    Serial.println(tempProfile[i].color);
    currentAddress += (sizeof(database) * 4);
  }
  
  delay(5000);

}
