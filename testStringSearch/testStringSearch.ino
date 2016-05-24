void setup() {
  // put your setup code here, to run once:
  char *str = "Mader faker gentalman";
  char search = 'a';

  char *index;

  Serial.begin(9600);

  Serial.print("looking for '");
  Serial.print(search);
  Serial.print("' in ");
  Serial.print(str);
  Serial.println("'.");

  index = strchr(str,(int)search);
  while(index != NULL)
  {
    Serial.print("found at ");
    Serial.println(index-str+1);
    index = strchr(index+1,(int)search);
  }
  

}

void loop() {
  // put your main code here, to run repeatedly:

}
