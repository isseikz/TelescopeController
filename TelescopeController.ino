// Telescope controller
// by isseikz

#define BLK 12
#define GRN 14
#define RED 27
#define BLU 26

#define THRESHOLD 40

#define STEPANGLE 0.9 // [deg/step] 

bool isMovingForward  = false;
bool isMovingBackward = false;

float dps = 2; // [deg/s]
float delayTime = 1000/4 / (dps/STEPANGLE);

void swtMoveForward(){
  isMovingForward = true;
}

void swtMoveBackward(){
  isMovingBackward = true;
}

void changeStepper(int STATE_BLK, int STATE_GRN, int STATE_RED, int STATE_BLU){
  digitalWrite(BLK,STATE_BLK);
  digitalWrite(GRN,STATE_GRN);
  digitalWrite(RED,STATE_RED);
  digitalWrite(BLU,STATE_BLU);
}

void moveForward(){
//  BLK → GRN
//  changeStepper(HIGH, LOW, LOW, LOW);
  digitalWrite(BLK,HIGH);
  digitalWrite(GRN,LOW);
  digitalWrite(RED,LOW);
  digitalWrite(BLU,LOW);
  delay(delayTime);
//  RED → BLU
//  changeStepper(LOW, LOW, HIGH, LOW);
  digitalWrite(BLK,LOW);
  digitalWrite(GRN,LOW);
  digitalWrite(RED,HIGH);
  digitalWrite(BLU,LOW);
  delay(delayTime);
//  GRN → BLK
//  changeStepper(LOW, HIGH, LOW, LOW);
  digitalWrite(BLK,LOW);
  digitalWrite(GRN,HIGH);
  digitalWrite(RED,LOW);
  digitalWrite(BLU,LOW);
  delay(delayTime);
//  BLU → RED
//  changeStepper(LOW, LOW, LOW, HIGH);
  digitalWrite(BLK,LOW);
  digitalWrite(GRN,LOW);
  digitalWrite(RED,LOW);
  digitalWrite(BLU,HIGH);
  delay(delayTime);
}

void moveBackward(){
//  BLU → RED
  changeStepper(LOW, LOW, LOW, HIGH);
  delay(delayTime);
//  GRN → BLK
  changeStepper(LOW, HIGH, LOW, LOW);
  delay(delayTime);
//  RED → BLU
  changeStepper(LOW, LOW, HIGH, LOW);
  delay(delayTime);
//  BLK → GRN
  changeStepper(HIGH, LOW, LOW, LOW);
  delay(delayTime);
}

void stopMotion(){
  changeStepper(LOW, LOW, LOW, LOW);
  delay(delayTime);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Telescope Controller!");

  pinMode(BLK, OUTPUT);
  pinMode(GRN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BLU, OUTPUT);
  
  touchAttachInterrupt(T8, swtMoveForward,  THRESHOLD);
  touchAttachInterrupt(T9, swtMoveBackward, THRESHOLD);

}

void loop() {
  if(isMovingForward){
    Serial.println("moveForward...");
    isMovingForward = false;
    isMovingBackward = false;
    moveForward();
  }else if(isMovingBackward){
    Serial.println("moveBackward...");
    isMovingForward = false;
    isMovingBackward = false;
    moveBackward();
  }else{
    isMovingForward = false;
    isMovingBackward = false;
    stopMotion();
  }

}
