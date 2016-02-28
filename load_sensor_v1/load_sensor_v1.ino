
int sensorValue1 = 0;
int sensorValue2 = 0;

void setup() {
  // put your setup code here, to run once:
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
sensorValue1 = analogRead(A0);
sensorValue2 = analogRead(A1);
float voltageDif=sensorValue1-sensorValue2;

Serial.print(sensorValue1);
Serial.print("\t");
Serial.print(sensorValue2);
Serial.print("\t");
Serial.print(voltageDif);
Serial.print("\n");
delay(200);
}
