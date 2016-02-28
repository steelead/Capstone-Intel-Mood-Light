void setup() {
Serial.begin(9600);
 pinMode(9,OUTPUT);
 
 digitalWrite(9,HIGH);
  delay(1000);
  digitalWrite(9,LOW);
  delay(1000);
 digitalWrite(9,HIGH);
  delay(1000);
  digitalWrite(9,LOW);
  delay(1000);

}

void loop() {
int buttonRead1=analogRead(A0);
Serial.println(buttonRead1);
delay(200);

}
