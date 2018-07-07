// Telescope controller by isseikz(https://github.com/isseikz)
// Project: https://github.com/isseikz/TelescopeController

#define BLK 12
#define GRN 14
#define RED 27
#define BLU 26

#define THRESHOLD 40

#define STEPANGLE 0.9 // [deg/step] 

bool isMovingForward  = false;
bool isMovingBackward = false;

int packetCnt;
char packet;
char packets[30];
bool readingSignal = false;

float dps = 3; // [deg/s]
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

  packetCnt = 0;
}

void loop() {
  if(Serial.available()>0){
    packet = Serial.read();
    packets[packetCnt] = packet;

    if(packetCnt >= 4){
      if(readingSignal & packets[packetCnt-1] == '\r' & packets[packetCnt] == '\n'){
        packetCnt = 0;
        
        byte flag = packets[4];
        int dataSize = packets[5];

        byte data[dataSize];
        for(int i=0;i<dataSize;i++){
          data[i] = packets[6+i];
        }

        if(flag == 0x10){
          if(data[0]==0x00){
              for(int i=0;i<data[1];i++){
                moveForward();
              }
          }else if(data[0] == 0x01){
              for(int i=0;i<data[1];i++){
                moveBackward();
              }
          }
        }

        readingSignal = false;
        packetCnt = 0;
      }else if(packets[packetCnt -3] == 0x11 & packets[packetCnt-2] == 0x22 & packets[packetCnt-1] == 0x33 & packets[packetCnt] == 0x44){
        packetCnt = 3;
        readingSignal = true;
      }
    }
    packetCnt ++;
  }
  
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
