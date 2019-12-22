/////////////////
//  ThereMIDI  //
/////////////////

// Ultrasound sensor
const int ultrasoundPin = 2;
const int echoPin = 3;
int distance = 0; // analog distance value
int result = 0;   // calculated distance ( distance/60+shift)
int smoothVal = 0;

// Joystick
const int joystickTriggerPin = 8;
int joystickTrigger = HIGH;
int previous = HIGH;
int joyButton = LOW; // previous joystick button state
int joyX = 0; //Pitch Bend
int joyY = 0; //CC1 and CC20
int pitchBend = 0;
int controllerValue = 0;

// Button
const int buttonPin = 7;
int button = HIGH; // pin for joystick button
int lastButton = LOW; //previous button state
int toggle = 0;

const int laser = 5;

int lastVal = 0;
int lastMIDI = 0;
int lastPitch = 0;
int lastResult = 0;
unsigned long previousMillis = 0;
const long interval = 100; // milliseconds to wait before updating

const bool debug = false; 

void setup() {
  Serial.begin(19200); //set baud-rate in hairless-midi to this value
  pinMode(ultrasoundPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(joystickTriggerPin, INPUT);
  pinMode(7, INPUT_PULLUP);
  pinMode(laser, OUTPUT);
  digitalWrite(laser, HIGH);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (debug) {
      Serial.println(lastMIDI);
      Serial.println(lastPitch);
      Serial.println(lastVal);
    }
    theremin(); 
  }
  
  checkJoystick();
  checkButtons();
}

void theremin() {
   

  joystickTrigger = digitalRead(8);

  // send out ping from ultrasound sensor
  digitalWrite(ultrasoundPin, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasoundPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasoundPin, LOW);

  distance = pulseIn(echoPin, HIGH);
//  Serial.println(distance);
  //smoothVal = 0.7 * smoothVal + 0.3 * distance;
  int shift = 20;
  result = distance/60 + shift; //higher value raises height
//  Serial.println(result);

  //trigger activated
  if(result < 120 && joystickTrigger == LOW && previous == HIGH){
    sendMIDI(144, result, 100);
    lastResult = result;
  }
  //changed note while active
  else if(joystickTrigger == LOW && previous == LOW && result != lastResult){
    sendMIDI(144, lastResult, 0);
    sendMIDI(144, result, 100);
    lastResult = result;
  }
  //deactivate
  else if(joystickTrigger == HIGH && previous == LOW && lastResult != 0){
    sendMIDI(144, lastResult, 0);
  }
  previous = joystickTrigger; 
}

void checkJoystick() {
  //handle joystick x-position
  joyX = analogRead(A0);
  
  if (joyX > 550) {
    pitchBend = map(joyX,550,1023,65,127);
    sendMIDI(224,0,pitchBend);
  }
  if (joyX < 450) {
    pitchBend = map(joyX,0,450,0,63);
    sendMIDI(224,0,pitchBend);
  }
  else if ((joyX > 450 && joyX < 550) && pitchBend != 64) {
    pitchBend = 64;
    sendMIDI(224,0,pitchBend);
  }
  lastPitch = joyX;
  
  //handle joystick y-position
  joyY = analogRead(A1);
  if (joyY > 580 && lastVal != joyY) {
    controllerValue = map(joyY,580,1023,0,127);
    sendMIDI(176,1,controllerValue); // send to the effect channel
  }
  else if (joyY < 450 && lastVal != joyY) {
    controllerValue = map(joyY,450,0,0,127);
    sendMIDI(176,11,127-controllerValue); // send to the volume channel
  }
  else if((joyY > 450 && joyY < 580) && lastVal != 0 ){
    controllerValue = 0;
    if(lastMIDI > 1){
     sendMIDI(176,11,0);
    }
    else {
     sendMIDI(176,1,0);
    }
  }
  lastVal = controllerValue;
}


void checkButtons() {
  button = digitalRead(buttonPin);
  joystickTrigger = digitalRead(joystickTriggerPin);

  //handle laser state
  if(joystickTrigger == HIGH){
    digitalWrite(laser, HIGH);
  }
  else{
    digitalWrite(laser, LOW);
  }

  if (button == LOW && lastButton == HIGH) {
    if(toggle == 0){
      sendMIDI(176, 64, 127);
      toggle = 1;
    }
    else {
      sendMIDI(176, 64, 0);
      toggle = 0;
    }
  }
  lastButton = button;
}

void sendMIDI(int statusByte, int dataByte1, int dataByte2) {
  if(!debug){
    Serial.write(statusByte);
    Serial.write(dataByte1);
    lastMIDI = dataByte1;
    Serial.write(dataByte2);
  }
}
