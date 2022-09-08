// include libraries
#include <AFMotor.h> // Adafruit Motor Shield v1
#include <SoftwareSerial.h> // Allows serial communication on digital pins of Arduino Board

// define bluetooth comms pins
SoftwareSerial bluetoothSerial(9,10); // RX, TX

// define each motor
AF_DCMotor frontLeft(2);
AF_DCMotor frontRight(1);
AF_DCMotor rearLeft(3);
AF_DCMotor rearRight(4);

// define motor array
AF_DCMotor motors[] = {frontLeft, frontRight, rearRight, rearLeft}; // start front left, move clockwise

// define variable for receiving bluetooth instruction
char instruction;

// define variable for follow mode (drone in swarm)
bool follow = false;

// define max speed
int MAX = 255;

void setup() {
  // set baud rate for bluetooth comms
  bluetoothSerial.begin(9600);
  Serial.begin(9600);
}

void loop() { 
  // get current instruction - either bluetooth direct control or swarm instruction via serial from mqtt
  
  // check for bluetooth instruction
  if (bluetoothSerial.available() > 0)
  {
    if(!follow){ // check to see if rover is latched as a drone in a swarm
      instruction = bluetoothSerial.read(); // if not in drone mode accept bluetooth input 
      Serial.println(instruction); // if not in follow mode rover is leader and will broadcast every move 
    }
  }

  // check serial data from mqtt
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    // the conditional below is for testing purposes, in actual swarm instruction is just Serial.read() without first checking input
    // for test purposes single instructions are broadcast from keyboard via mqtt, so no movement is seen as motors don't activate for long enough
    // in actual swarm, instructions continually broadcast and null value for stop must be accepted
    if(input == 'Q' or input == 'F' or input == 'B' or input == 'L' or input == 'R'){
      instruction = input;
      follow = true; // latches drone
    }
    Serial.println(instruction);
    
    if(instruction == 'Q'){
      follow = false; // unlatch from following mode
    }
  }
  
  // initialise stop
  Stop();
  
  // identify instruction function and execute
  switch(instruction){
    case 'F':
      Forward();
      break;
    case 'B':
      Back();
      break;
    case 'L':
      Left();
      break;
    case 'R':
      Right();
      break;
    default:
      Stop();
      break;
  }
}

void Stop(){
  SetAll(0, RELEASE);
}

void Forward(){
 SetAll(MAX, FORWARD);
}

void Back(){
  SetAll(MAX, BACKWARD);
}

void Left(){
  SetSpeedAll(MAX);
  SetLeft(BACKWARD);
  SetRight(FORWARD);
}

void Right(){
  SetSpeedAll(MAX);
  SetLeft(FORWARD);
  SetRight(BACKWARD);
}

void SetLeft(int direction){
  frontLeft.run(direction);
  rearLeft.run(direction);
}

void SetRight(int direction){
  frontRight.run(direction);
  rearRight.run(direction);
}

void SetAll(int speed, int direction){
  for(int i = 0; i < 4; i++){
    motors[i].setSpeed(speed);
    motors[i].run(direction);
  }
}

void SetSpeedAll(int speed){
  for(int i = 0; i < 4; i++){
    motors[i].setSpeed(speed);
  }
}

/*
void testAllMotors(){
  testMotor(frontLeft);
  testMotor(frontRight);
  testMotor(rearLeft);
  testMotor(rearRight);
}

void testMotor(AF_DCMotor motor){
  motor.setSpeed(200);
  motor.run(FORWARD);
  delay(2000);
  motor.run(RELEASE);
  delay(1000);
  motor.run(BACKWARD);
  delay(2000);
  motor.run(RELEASE);
}
*/
