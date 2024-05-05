/*************************************************************************************************** 
*  Projet Robot 2024 Xbee_PAMI_VMS_V2.0
*  
*  Author :MS/PG
*  Review : 11/04/2024  Xbee_PAMI1_V3MS.ino Ajout du SR04
*  Review :             Fabrice Pellissier (fuctions Forward, Rotate, Run_PAMI, Stop_PAMI 
*  Review : 04/05/2024 Add interrupt management for stop and restart whith acceleration ramp
*  
*  Important global variables :
*    - dist : distance to forward in mm. 
*    - degR : angle in degrees to turn Rigth
*    - degL : angle in degrees to turn Left
*    
*  Functions :
*    - Forward(dist) : move forward to dist in mm
*    - RotateRight(degR) : turn right degR degrees
*    - RotateLeft(degL) : turn left degL degrees
*  
*  Usage : PAMI start when 'g' or 'd' character is received on Xbee module.
*          Strategy is based on a state digram. When a stage is done, the stage flag is incremented.
*          Global variable dist, degR, degL must be assigned before exiting the last stage.
*          Execution stops after 10s due to use of MsTimer2.
*
****************************************************************************************************/
#include <SoftwareSerial.h>
#include <AccelStepper.h>
#include <MsTimer2.h>


#define BAUDRATE 9600
#define MAXSPEED_PAMI 20000  // Pour avoir un mouvement plus "fluide"
#define ACCELERATION_PAMI 3000
#define DRIVER AccelStepper::DRIVER

#define DEBUG false

/* Stepper motor  Righ*/
#define MR_DIR_PIN 3
#define MR_STEP_PIN 4
#define MR_EN_PIN A0
/* Stepper motor Left*/
#define ML_DIR_PIN 5
#define ML_STEP_PIN 6
#define ML_EN_PIN A1

// To debug easily, uncomment the following line and use the serial terminal to send 'd' or 'g'
#define xbeeSerial Serial

// To use Xbee module, uncomment the following line
//SoftwareSerial xbeeSerial(8, 9);  // RX, TX

AccelStepper Right_stepper(DRIVER, MR_STEP_PIN, MR_DIR_PIN);
AccelStepper Left_stepper(DRIVER, ML_STEP_PIN, ML_DIR_PIN);

String Order;
char MainOrder;
int stage = 1;
char action;
const byte EchoBack = 2;  // EchoBack Interrupt
//const byte interruptPin2 = 7;  // EchoBack
long steps;
long currentPosition_R = 0;
long currentPosition_L = 0;
bool fin = false;
int dist = 800;
int degL = 90;
int degR = 90;
long angle_steps = 0;
bool stop = false;
bool noStop = false;

void setup() {
  Serial.begin(BAUDRATE);
  xbeeSerial.begin(BAUDRATE);
  pinMode(EchoBack, INPUT);

  pinMode(MR_EN_PIN, OUTPUT);
  pinMode(ML_EN_PIN, OUTPUT);
  disableEnMotionStepper();
  Right_stepper.setMaxSpeed(MAXSPEED_PAMI);
  Right_stepper.setAcceleration(ACCELERATION_PAMI);

  Left_stepper.setMaxSpeed(MAXSPEED_PAMI);
  Left_stepper.setAcceleration(ACCELERATION_PAMI);
  attachInterrupt(digitalPinToInterrupt(EchoBack), ObstacleDetected, CHANGE);

  // Stop while a character is available
  while (!xbeeSerial.available());
  
  // Read the character received on XBee
  Order = xbeeSerial.readStringUntil('\n');
  #if DEBUG
  Serial.print("Order : ");
  Serial.println(Order);
  #endif

  // Split order
  MainOrder = Order.charAt(0);

  // Start timer
  #if DEBUG
  Serial.println("Start RUN");
  #endif
  MsTimer2::set(10000, EndOfRun);
  MsTimer2::start();

}
/******************************************************************************/
void loop() {

  if(MainOrder == 'g') {
    if (!stop) {

      if(stage == 1){
        #if DEBUG
        Serial.print("stage :");
        Serial.println(stage);
        #endif
        
        Forward(dist);
        degR = 90;
      }

      if(stage == 2){
        #if DEBUG
        Serial.print("stage :");
        Serial.println(stage);
        #endif
        
        RotateLeft(degL);
        dist=400;
      }
      if(stage == 3){
        #if DEBUG
        Serial.print("stage :");
        Serial.println(stage);
        #endif
        
        Forward(dist);
        degL = 90;
      }
      if(stage == 4){
        #if DEBUG
        Serial.print("stage :");
        Serial.println(stage);
        #endif
        
        RotateRight(degR);
        dist=400;
      }
      if(stage == 5){
        #if DEBUG
        Serial.print("stage :");
        Serial.println(stage);
        #endif
        
        Forward(dist);
        degL = 90;
      }
      if(stage == 6){
        #if DEBUG
        Serial.print("stage :");
        Serial.println(stage);
        #endif
        stop = true;
        Serial.println("End of strategy !");
      }
      
    }
  }

  if(MainOrder == 'd') {
    if (!stop) {

      if(stage == 1){
        #if DEBUG
        Serial.print("stage :");
        Serial.println(stage);
        #endif
        
        Forward(dist);
        degR = 90;
      }

      if(stage == 2){
        #if DEBUG
        Serial.print("stage :");
        Serial.println(stage);
        #endif
        
        RotateRight(degR);
        dist=800;
      }
      
      if(stage == 3){
        #if DEBUG
        Serial.print("stage :");
        Serial.println(stage);
        #endif
        
        Forward(dist);
        dist=200;
      }
      
      if(stage == 4){
        #if DEBUG
        Serial.print("stage :");
        Serial.println(stage);
        #endif

        noStop = true;
        Forward(dist);
        //degL = 90;
      }

      if(stage == 5){
        #if DEBUG
        Serial.print("stage :");
        Serial.println(stage);
        #endif
        
        stop = true;
        Serial.println("End of strategy !");
      }
      
    }
  }
}  // Fin loop

/******************************************************************************
* 
* function : Forward()
* 
******************************************************************************/

void Forward(long dist) {
  action = 'F';
  steps = (dist * 1600 / 220);  // RÃ©glage sur Drivers au micropas au 1/8
  digitalWrite(MR_EN_PIN, LOW);
  digitalWrite(ML_EN_PIN, LOW);
  
  #if DEBUG
  Serial.print("Move Forward: ");
  Serial.print(dist);
  Serial.println("mm");
  #endif
  
  Right_stepper.move(-steps);
  Left_stepper.move(steps);
  while (!(Right_stepper.distanceToGo() == 0 && Left_stepper.distanceToGo() == 0)) {
    Right_stepper.run();
    Left_stepper.run();
  }

  if (digitalRead(EchoBack)) {
    if(!noStop) {
      #if DEBUG
      Serial.println("Stop Forward - Obstacle");
      #endif
      return 0;
    }
  }
  
  #if DEBUG
  Serial.println("Distance Reached ! : ");
  #endif
  
  sendSteppersPositions();
  resetSteppersPositions();

  digitalWrite(MR_EN_PIN, HIGH);
  digitalWrite(ML_EN_PIN, HIGH);
  stage++;
  return 0;
}

/*****************************************************************************
* 
* function : RotateRight()
* 
******************************************************************************/

void RotateRight(int degR) {
  action = 'R';
  digitalWrite(MR_EN_PIN, LOW);
  digitalWrite(ML_EN_PIN, LOW);
  // Order to Rotate
  angle_steps = 8.75 * degR;

  #if DEBUG
  Serial.print("angle_steps :");
  Serial.print(angle_steps);
  Serial.print(" degR :");
  Serial.println(degR);
  #endif
  
  Right_stepper.move(-angle_steps);
  Left_stepper.move(-angle_steps);
  while (!(Right_stepper.distanceToGo() == 0)) {
    Right_stepper.run();
    Left_stepper.run();
  }

  if (digitalRead(EchoBack)) {
    Serial.println("Stop Rotate Right - Obstacle");
    return 0;
  }
  
  #if DEBUG
  Serial.println("Angle Reached ! : ");
  #endif
  sendSteppersPositions();
  resetSteppersPositions();

  digitalWrite(MR_EN_PIN, HIGH);
  digitalWrite(ML_EN_PIN, HIGH);
  stage++;
  return 0;
}

/*****************************************************************************
* 
* function : RotateLeft()
* 
******************************************************************************/
void RotateLeft(int degL) {
  action = 'L';
  digitalWrite(MR_EN_PIN, LOW);
  digitalWrite(ML_EN_PIN, LOW);
  // Order to Rotate
  angle_steps = 8.75 * degL;

  #if DEBUG
  Serial.print("angle_steps :");
  Serial.print(angle_steps);
  Serial.print(" degL :");
  Serial.println(degL);
  #endif
  
  Left_stepper.move(angle_steps);
  Right_stepper.move(angle_steps);
  while (!(Left_stepper.distanceToGo() == 0)) {
    Right_stepper.run();
    Left_stepper.run();
  }

  if (digitalRead(EchoBack)) {
    #if DEBUG
    Serial.println("Stop Rotate Left - Obstacle");
    #endif
    
    return;
  }

  #if DEBUG
  Serial.println("Angle Reached ! : ");
  #endif
  
  sendSteppersPositions();
  resetSteppersPositions();

  digitalWrite(MR_EN_PIN, HIGH);
  digitalWrite(ML_EN_PIN, HIGH);
  stage++;
  return;
}

/*****************************************************************************
* 
* function : Stop_PAMI()
* 
******************************************************************************/
void StopPAMI() {
  #if DEBUG
  Serial.println("PAMI Stopped");
  #endif
  
  Right_stepper.stop();
  Left_stepper.stop();
  sendSteppersPositions();
  resetSteppersPositions();
  digitalWrite(MR_EN_PIN, HIGH);
  digitalWrite(ML_EN_PIN, HIGH);
}

/*****************************************************************************
* 
* function : disableEnMotionStepper()
* 
******************************************************************************/
void disableEnMotionStepper() {
  // Motion Right_STEPPER and Left_STEPPER OFF(FREE)
  digitalWrite(MR_EN_PIN, HIGH);
  digitalWrite(ML_EN_PIN, HIGH);
}
/*****************************************************************************
* 
* function : sendSteppersPositions()
* 
******************************************************************************/
void sendSteppersPositions() {
  currentPosition_R = Right_stepper.currentPosition();
  currentPosition_L = Left_stepper.currentPosition();
  
  #if DEBUG
  Serial.print(currentPosition_R);
  Serial.print("/");
  Serial.println(currentPosition_L);
  #endif
}

/*****************************************************************************
* 
* function : resetSteppersPositions()
* 
******************************************************************************/
void resetSteppersPositions() {
  Right_stepper.setCurrentPosition(0);
  Left_stepper.setCurrentPosition(0);
}


/*****************************************************************************
* 
* function : ObstacleDetected()
* 
******************************************************************************/
void ObstacleDetected()
{
  if(!noStop){
    int echoBackState = digitalRead(EchoBack);
    if (echoBackState) {
      StopPAMI();
      
      #if DEBUG
      Serial.println("obstace détecté");
      #endif
      
      stop = true;
      steps = steps - currentPosition_L;
      angle_steps = angle_steps - long(abs(currentPosition_L + currentPosition_R) / 2);
    }
    else
    {
      #if DEBUG
      Serial.println("horizon clair");
      Serial.print("Continue stage :");
      Serial.println(stage);
      Serial.print("Action :");
      Serial.println(action);
      #endif
      
      stop = false;
      if(action == 'F'){
        dist = long(steps * 220 / 1600);
      }
      if(action=='R'){
        degR = long(angle_steps / 8.75);
      }  
      if(action=='L'){
        degL = long(angle_steps / 8.75);
      }    
    }  
  }
  
}


/*****************************************************************************
* 
* function : EndOfRun()
* 
******************************************************************************/
void EndOfRun() {
  MsTimer2::stop();
//#if DEBUG
  Serial.println("End of RUN !");
//#endif
  StopPAMI();
  while(1);
}
