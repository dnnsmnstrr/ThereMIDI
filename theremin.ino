int button = 8;
int taste = HIGH;
int previous = HIGH;

int triggerPin = 2;
int echoPin = 3;
int distance = 0;
int result = 0;
int last = 0;
int smoothVal = 0;

int joyX = 0; //Pitch Bend
int joyY = 0; //CC1 und CC20
int pitchBend = 0;
int controllerWert = 0;
int lastVal = 0;
int lastMIDI = 0;
int lastPitch = 0;

int joyButton = LOW;
int buttonAlt = LOW;
int schalter = 0;

const int laser = 5;

unsigned long previousMillis = 0;
const long interval = 100;

void setup() {
  Serial.begin(19200);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(button, INPUT);
  pinMode(7, INPUT_PULLUP);
  pinMode(laser, OUTPUT);
  digitalWrite(laser, HIGH);


}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    theremin();
  }
  
  joystickAbfragen();
  buttonAbfragen();
}

void theremin() {
  taste = digitalRead(8);
  
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  distance = pulseIn(echoPin, HIGH);
  //smoothVal = 0.7 * smoothVal + 0.3 * distance;
  result = distance/60 + 20; //higher value raises height
  
  //trigger activated
  if(result < 120 && taste == LOW && previous == HIGH){
    sendMIDI(144, result, 100);
    last = result;
    //digitalWrite(laser, LOW);
  }
  //changed note while active
  else if(taste == LOW && previous == LOW && result != last){
    sendMIDI(144, last, 0);
    sendMIDI(144, result, 100);
    last = result;
  }
  //deactivate
  else if(taste == HIGH && previous == LOW && last != 0){
    sendMIDI(144, last, 0);
    //digitalWrite(laser, HIGH);
  }
  previous = taste; 
  

  //Serial.println(result);
  //delay(60);
}
void joystickAbfragen() {
 joyX = analogRead(A0);
 if (joyX > 530) {
 pitchBend = map(joyX,530,1023,65,127);
 sendMIDI(224,0,pitchBend);
 }
 if (joyX < 500) {
 pitchBend = map(joyX,0,500,0,63);
 sendMIDI(224,0,pitchBend);
 }
 else if ((joyX > 500 && joyX < 530) && pitchBend != 64) {
 pitchBend = 64;
 sendMIDI(224,0,pitchBend);
 }
 lastPitch = joyX;
 
 joyY = analogRead(A1);
 if (joyY > 530 && lastVal != joyY) {
 controllerWert = map(joyY,530,1023,0,127);
 sendMIDI(176,1,controllerWert);
 }
 else if (joyY < 500 && lastVal != joyY) {
 controllerWert = map(joyY,500,0,0,127);
 sendMIDI(176,11,127-controllerWert);
 }
 else if((joyY < 500 && joyY > 550) && lastVal != 0 ){
  controllerWert = 0;
  if(lastMIDI > 1){
    sendMIDI(176,11,0);
  }
  else {
    sendMIDI(176,1,0);
 }

 }
 lastVal = controllerWert;
}


void buttonAbfragen() {
 button = digitalRead(7);
 int button2 = digitalRead(8);

 if(button2 == HIGH){
  digitalWrite(laser, HIGH);
 }
 else{
  digitalWrite(laser, LOW);
 }

 if (button == LOW && buttonAlt == HIGH) {
 if(schalter == 0){
 sendMIDI(176, 64, 127);
 schalter = 1;
 }
 else {
 sendMIDI(176, 64, 0);
 schalter = 0;
 }
 }
 buttonAlt = button;
}

void sendMIDI(int statusByte, int dataByte1, int dataByte2) {
  Serial.write(statusByte);
  Serial.write(dataByte1);
  lastMIDI = dataByte1;
  Serial.write(dataByte2);
}
