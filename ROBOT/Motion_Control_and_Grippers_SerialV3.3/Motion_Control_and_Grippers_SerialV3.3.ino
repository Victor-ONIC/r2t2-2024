/***********************************************************************
 * Motion_Control_and_Grippers_SerialV3.ino
 * Carte Arduino MEGA2560 R3
 * Sur Shield Grove MEGA2560
 * Author : MS/PG
 * Date   : 11/03/2024
 * Review : 13/03/2024 V2
 * Review : 17/03/2024 V3
 * Review : 19/03/2024 V3.1 (en for Stop command)
 * Review : 30/03/2024 V3.2 jack pin A6 et comm XBEE
 * Review : 020/04/2024 V3.3 Calibrate Grippers
 * 
 * Manage  the movements of the 3 Grippers on robot 2024.
 * Communication Protocol : 
 *   Main Order :
 *   - 'F' : Forward on axis 'X', 'Y' or 'Z' given distance in mm
 *   - 'B' : Backward on axis 'X', 'Y' or 'Z' given distance in mm
 *   - 'R' : Rotate on itself with angle in degrees.
 *   - 'S' : Emergency stop.
 *   - 'U' : Gripper UP. 
 *   - 'D' : Gripper DOWN.
 *   - 'C' : Gripper CLOSE.
 *   - 'O' : Gripper OPEN.
 *   
 *   
 *   Axis :
 *   - 'X' : steppers 2 and 3 run if main order is F or B else GripX_Stepper run
 *   - 'Y' : steppers 1 and 3 run if main order is F or B else GripY_Stepper run
 *   - 'Z' : steppers 1 and 2 run if main order is F or B else Grip_ZStepper run
 *   
 *   Examples : 
 *   'FX700' : Forward along X axis to 700mm
 *   'BY554' : Backward along Y axis to 554 mm
 *   'R180'  : Rotate 180° clockwise
 *   'R-180' : Rotate 180° anticlockwise
 *   'UX50' : Gripper X Up to 50mm..
 *   'CX'   : Close Gripper X.
 *   
 * Program based on AccelStepper library
 * http://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html
 * author  Mike McCauley (mikem@airspayce.com)
 * Copyright (C) 2009 Mike McCauley
 * $Id: ConstantSpeed.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $
 * 
 ************************************************************************/
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <AccelStepper.h>
#include <SoftwareSerial.h>
#include <VL53L0X.h> // Bibliothèque des ToF
#include <TCA9548A.h> //Bibliothèque du mux
// Set DEBUG to false to disable debug messages
#define DEBUG true
#define STEPPERINIT false

#define BAUDRATE 9600
#define MAXSPEED 6000 //V2 4000 V3
#define ACCELERATION 2000 //V2 1000 V3
#define MAXSPEED_Grip 6500 // Pour avoir un mouvement plus "fluide"
#define ACCELERATION_Grip 3500
#define DRIVER AccelStepper::DRIVER
#define JACK A6

#define STEPPER1_DIR_PIN 8  // STEPPER ON X Axis
#define STEPPER2_DIR_PIN 10 // STEPPER ON Y Axis
#define STEPPER3_DIR_PIN 12 //STEPPER ON Z Axis

#define STEPPER1_STEP_PIN 9
#define STEPPER2_STEP_PIN 11
#define STEPPER3_STEP_PIN 13

//Valide ou non les 3 moteurs de déplacement
#define STEPPER1_MOTION_EN_PIN 54 // (A0)
#define STEPPER2_MOTION_EN_PIN 55 // (A1)
#define STEPPER3_MOTION_EN_PIN 56 // (A2)

/* Grippers Steppers define*/
#define GRIPX_STEPPER_DIR_PIN A8  //(A8 sur Grove Shield MEGA)
#define GRIPY_STEPPER_DIR_PIN 64  //(A10 sur Grove Shield MEGA)
#define GRIPZ_STEPPER_DIR_PIN 66  //(A12 sur Grove Shield MEGA)
#define GRIPX_STEPPER_STEP_PIN A9  //(A9 sur Grove Shield MEGA)
#define GRIPY_STEPPER_STEP_PIN 65 //(A11 sur Grove Shield MEGA)
#define GRIPZ_STEPPER_STEP_PIN 67 //(A13 sur Grove Shield MEGA)

/* For Grippers limits*/
#define XLimit_PIN   2 
#define YLimit_PIN   3 
#define ZLimit_PIN   4 

/*For Stepper Grippers Enable*/
#define GRIPX_STEPPER_EN_PIN A3 //(A3)
#define GRIPY_STEPPER_EN_PIN 58 //(A4)
#define GRIPZ_STEPPER_EN_PIN 59 //(A5)

/*Grippers Servos frequency*/
#define SERVO_FREQ 50 // Fréquence 50Hz soit 20ms de période pour les servos
/* Connexion sur carte Adafruit PCA9585*/
#define X_Servo_PIN 0
#define Y_Servo_PIN 1
#define Z_Servo_PIN 2
#define OE_Servos_PIN 5

SoftwareSerial xbeeSerial(6, 7); // RX, TX

/* Intanciate Steppers For Motion*/
AccelStepper stepper1(DRIVER, STEPPER1_STEP_PIN, STEPPER1_DIR_PIN);
AccelStepper stepper2(DRIVER, STEPPER2_STEP_PIN, STEPPER2_DIR_PIN);
AccelStepper stepper3(DRIVER, STEPPER3_STEP_PIN, STEPPER3_DIR_PIN);

/* Intanciate Steppers For Grippers*/
AccelStepper GripX_stepper(DRIVER, GRIPX_STEPPER_STEP_PIN, GRIPX_STEPPER_DIR_PIN);
AccelStepper GripY_stepper(DRIVER, GRIPY_STEPPER_STEP_PIN, GRIPY_STEPPER_DIR_PIN);
AccelStepper GripZ_stepper(DRIVER, GRIPZ_STEPPER_STEP_PIN, GRIPZ_STEPPER_DIR_PIN);

/* PCA9585 I2C Board 16 Channels*/
// Adresse par défaut de la carte Adafruit  0x40
/* Intanciate Servos*/
Adafruit_PWMServoDriver Servo_Gripper = Adafruit_PWMServoDriver(0x40, Wire);

String order;
char mainOrder;
char axe;
long dist;
long height;// Hauteur de la pince(200pas=1 tour=8mm)
long steps;
int deg;
long angle_steps;
bool jo = false;


/*Instanciate Mux and ToF*/
TCA9548A multiplexeurI2C;
VL53L0X tofSensorGrabX;
VL53L0X tofSensorGrabY;
VL53L0X tofSensorGrabZ;
VL53L0X tofSensorCollisionXY; 
VL53L0X tofSensorCollisionYZ;
VL53L0X tofSensorCollisionZX;


void setup()
{
  Serial.begin(BAUDRATE);
  xbeeSerial.begin(BAUDRATE);
  #if DEBUG
  Serial.println("Start...");
  #endif

  delay(100);

  pinMode(JACK, INPUT_PULLUP);
  
  stepper1.setMaxSpeed(MAXSPEED);
  stepper1.setAcceleration(ACCELERATION);
  //stepper1.setCurrentPosition(0); // Set the current position to 0 steps

  
  stepper2.setMaxSpeed(MAXSPEED);
  stepper2.setAcceleration(ACCELERATION);
   //stepper2.setCurrentPosition(0);

  stepper3.setMaxSpeed(MAXSPEED);
  stepper3.setAcceleration(ACCELERATION);
  // stepper3.setCurrentPosition(0);
  
  
  /* Pins mode Grippers Steppers*/
  pinMode(XLimit_PIN, INPUT_PULLUP); // Active on Low Level
  pinMode(YLimit_PIN, INPUT_PULLUP);
  pinMode(ZLimit_PIN, INPUT_PULLUP);

  pinMode(GRIPX_STEPPER_DIR_PIN, OUTPUT); 
  pinMode(GRIPY_STEPPER_DIR_PIN, OUTPUT);
  pinMode(GRIPZ_STEPPER_DIR_PIN, OUTPUT);
  
  pinMode(GRIPX_STEPPER_STEP_PIN,OUTPUT);
  pinMode(GRIPY_STEPPER_STEP_PIN,OUTPUT);
  pinMode(GRIPZ_STEPPER_STEP_PIN,OUTPUT);

  /* Pins mode ENABLE Driver Grippers Steppers*/ 
  pinMode(GRIPX_STEPPER_EN_PIN,OUTPUT);
  pinMode(GRIPY_STEPPER_EN_PIN,OUTPUT);
  pinMode(GRIPZ_STEPPER_EN_PIN,OUTPUT);

  /* Pins mode ENABLE Driver Motion Steppers*/ 
  pinMode(STEPPER1_MOTION_EN_PIN,OUTPUT);
  pinMode(STEPPER2_MOTION_EN_PIN,OUTPUT);
  pinMode(STEPPER3_MOTION_EN_PIN,OUTPUT);

  /* ENABLE/DISABLE all servos*/
  pinMode(OE_Servos_PIN, OUTPUT);
  
  /* Grippers_Servos*/
  Servo_Gripper.begin();
  
  /* il faut régler la fréquence de 50Hz avec un oscilloscope*/
  Servo_Gripper.setOscillatorFrequency(26800000);// Réglé sur oscilloscope pour obtenir un période de 20.009ms!
  Servo_Gripper.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  /*Init Grippers Steppers*/
  GripX_stepper.setMaxSpeed(MAXSPEED_Grip);
  GripX_stepper.setAcceleration(ACCELERATION_Grip);
   
  GripY_stepper.setMaxSpeed(MAXSPEED_Grip);
  GripY_stepper.setAcceleration(ACCELERATION_Grip);
  //delay(1000);
  GripZ_stepper.setMaxSpeed(MAXSPEED_Grip);
  GripZ_stepper.setAcceleration(ACCELERATION_Grip); 
   

  /*Init Gripper to Open */
  Servo_Gripper.writeMicroseconds(X_Servo_PIN, 1000); // Ouverture pince X;
  delay(100);
  Servo_Gripper.writeMicroseconds(Y_Servo_PIN, 1000); // Ouverture pince Y;
  delay(100);  
  Servo_Gripper.writeMicroseconds(Z_Servo_PIN, 1000); // Ouverture pince Z;
  delay(100);
  /* Disable all Servos*/  
  digitalWrite(OE_Servos_PIN,HIGH);//Disable all servos
  #if DEBUG  
  Serial.print("Enable Servos :");
  Serial.println(digitalRead(OE_Servos_PIN));
  #endif 
  
//  // Calibrage zero position basse Grippers 
//  digitalWrite(GRIPX_STEPPER_DIR_PIN,LOW); // LOW descente/HIGH montée
//  digitalWrite(GRIPY_STEPPER_DIR_PIN,LOW);  
//  digitalWrite(GRIPZ_STEPPER_DIR_PIN,LOW);
//  
//  byte stop_switchX = digitalRead(XLimit_PIN);
//  byte stop_switchY = digitalRead(YLimit_PIN);
//  byte stop_switchZ = digitalRead(ZLimit_PIN);
//  
//  #if DEBUG
//  Serial.print(stop_switchX);
//  Serial.print(stop_switchY);
//  Serial.println(stop_switchZ);
//  #endif
//
//  // stop_switch at 0 if  XLimit_PIN is LOW 
//  // GRIPX_STEPPER ON
//  digitalWrite(GRIPX_STEPPER_EN_PIN,LOW);
//  while(stop_switchX)
//  {
//    digitalWrite(GRIPX_STEPPER_STEP_PIN, HIGH);
//    delayMicroseconds(600);
//    digitalWrite(GRIPX_STEPPER_STEP_PIN, LOW);
//    delayMicroseconds(600);
//    stop_switchX = digitalRead(XLimit_PIN);
//  }
//  // Initial high position X to solar panel
//  digitalWrite(GRIPX_STEPPER_DIR_PIN,HIGH);
//  for(int s; s<2400;s++){
//    digitalWrite(GRIPX_STEPPER_STEP_PIN, HIGH);
//    delayMicroseconds(600);
//    digitalWrite(GRIPX_STEPPER_STEP_PIN, LOW);
//    delayMicroseconds(600);
//  }
//  // GRIPX_STEPPER OFF(FREE)
//  digitalWrite(GRIPX_STEPPER_EN_PIN,HIGH);
//
//  // stop_switch at 0 if  YLimit_PIN is LOW 
//  // GRIPY_STEPPER ON
//  digitalWrite(GRIPY_STEPPER_EN_PIN,LOW);
//  while(stop_switchY)
//  {
//    digitalWrite(GRIPY_STEPPER_STEP_PIN, HIGH);
//    delayMicroseconds(600);
//    digitalWrite(GRIPY_STEPPER_STEP_PIN, LOW);
//    delayMicroseconds(600);
//    stop_switchY = digitalRead(YLimit_PIN); 
//  } 
//  delay(10);
//  // Initial high position Y to solar panel
//  digitalWrite(GRIPY_STEPPER_DIR_PIN,HIGH);
//  for(int s=0; s<1900;s++){
//    digitalWrite(GRIPY_STEPPER_STEP_PIN, HIGH);
//    delayMicroseconds(600);
//    digitalWrite(GRIPY_STEPPER_STEP_PIN, LOW);
//    delayMicroseconds(600);
//  }
//  // GRIPY_STEPPER OFF (FREE)
//  digitalWrite(GRIPY_STEPPER_EN_PIN,HIGH);
//  
//  // stop_switch at 0 if  ZLimit_PIN is LOW 
//  // GRIPZ_STEPPER ON
//  digitalWrite(GRIPZ_STEPPER_EN_PIN,LOW);
//  while(stop_switchZ)
//  {
//    digitalWrite(GRIPZ_STEPPER_STEP_PIN, HIGH);
//    delayMicroseconds(600);
//    digitalWrite(GRIPZ_STEPPER_STEP_PIN, LOW);
//    delayMicroseconds(600);
//    stop_switchZ = digitalRead(ZLimit_PIN); 
//  } 
//  // Initial high position Z to solar panel
//  digitalWrite(GRIPZ_STEPPER_DIR_PIN,HIGH);
//  for(int s=0; s<2100;s++){
//    digitalWrite(GRIPZ_STEPPER_STEP_PIN, HIGH);
//    delayMicroseconds(600);
//    digitalWrite(GRIPZ_STEPPER_STEP_PIN, LOW);
//    delayMicroseconds(600);
//  }
//  // GRIPZ_STEPPER OFF (FREE)
//  digitalWrite(GRIPZ_STEPPER_EN_PIN,HIGH);
//  
//  #if DEBUG
//  Serial.print("Disable Motion Stepper EN : ");
//  Serial.println(String(digitalRead(STEPPER1_MOTION_EN_PIN)) + " - " +  String(digitalRead(STEPPER2_MOTION_EN_PIN)) + " - "+ String(digitalRead(STEPPER3_MOTION_EN_PIN)));
//  #endif
//  disable_en_motion_stepper();
//
//  sendSteppersPositions();
//    #if DEBUG
//  Serial.print("Jack :");
//  Serial.println(digitalRead(JACK));
//  #endif
//  
//  while(digitalRead(JACK)); 
//
//  #if DEBUG
//  //Serial.println("Jack out...");
//  #endif
//  
 }



void loop()
{

  
  // if order received
  if (Serial.available()>0) {
    order = Serial.readStringUntil('\n');
    #if DEBUG
    Serial.print("order : ");
    Serial.println(order);
    #endif
    // Split order
    mainOrder = order[0];
    axe = order[1];

    // Reset stepper steps counters if not Stop Order
    if (mainOrder != 'S') resetSteppersPositions();

    switch (mainOrder) {
      case 'G': // ordre départ PAMIs
      xbeeSerial.println("G");
      #if DEBUG
      Serial.print("Message envoyé par XBeeRobot: ");
      Serial.print("G");
      #endif
      break;
      // Order to Forward or Backward Robot Motion
      case 'F':
      case 'B':
        // Compute steps to forward to do distance
        dist = order.substring(2).toInt();
        steps = long(dist * 1600 / 212);// Réglage sur Drivers au micropas au 1/8
        
        if (mainOrder == 'B') 
        {
          #if DEBUG
          Serial.println("Received 'B'");
          #endif
          steps = -steps;
        }
        else 
        {
          #if DEBUG
          Serial.println("Received 'F'");
          #endif
        }

          switch (axe) 
          {
          case 'X':
            // On X axis
            digitalWrite(STEPPER2_MOTION_EN_PIN,LOW);
            digitalWrite(STEPPER3_MOTION_EN_PIN,LOW);
            #if DEBUG
            Serial.print("Move To Axis X : ");
            #endif
            stepper2.move(-steps);
            stepper3.move(steps);
            break;

          case 'Y':
            // On Y axis
            digitalWrite(STEPPER1_MOTION_EN_PIN,LOW);
            digitalWrite(STEPPER3_MOTION_EN_PIN,LOW);
            #if DEBUG
            Serial.print("Move To Axis Y : ");
             #endif
            stepper1.move(steps);
            stepper3.move(-steps);
            break;

          case 'Z':
            // On Z axis
            digitalWrite(STEPPER1_MOTION_EN_PIN,LOW);
            digitalWrite(STEPPER2_MOTION_EN_PIN,LOW);
            #if DEBUG
            Serial.print("Move To Axis Z : ");
            #endif
            stepper1.move(-steps);
            stepper2.move(steps);
            break;
          }
        break;// du case F/B
      case 'R':
        #if DEBUG
        Serial.println("Received 'R'");
        #endif
        digitalWrite(STEPPER1_MOTION_EN_PIN,LOW);
        digitalWrite(STEPPER2_MOTION_EN_PIN,LOW);
        digitalWrite(STEPPER3_MOTION_EN_PIN,LOW);
        // Order to Rotate
        deg = order.substring(1).toInt();
        angle_steps = 25 * deg;
        
        #if DEBUG
        Serial.print("angle_steps :");
        Serial.println(angle_steps);
        #endif
        stepper1.move(angle_steps);
        stepper2.move(angle_steps);
        stepper3.move(angle_steps);
        break;

      case 'S':
        #if DEBUG
        Serial.println("Received 'S'");
        #endif
        stepper1.stop();
        stepper2.stop();
        stepper3.stop();
        break;
// Order to UP or DOWN Grippers
      case 'U':
      case 'D':
        // Compute steps to UP to do "height"
        height = order.substring(2).toInt();
        steps = int(height * 200/8); //C'est assez précis!
        if (mainOrder == 'D') 
        {
          #if DEBUG
          Serial.println("Received 'D'");
          #endif
          steps = -steps;
        }
        else 
        {
          #if DEBUG
          Serial.println("Received 'U'");
          #endif
        }
          switch (axe) 
            {
            case 'X':
            // Move on X axis
            #if DEBUG
            Serial.println("Move Gripper_Axis X");
            #endif
            GripX_stepper.move(steps);
            break;

            case 'Y':
            // Move on Y axis
            GripY_stepper.move(steps);
            #if DEBUG
            Serial.println("Move Gripper_Axis Y");
            #endif
            break;

            case 'Z':
            // Move on Z axis
            GripZ_stepper.move(steps);
            #if DEBUG
            Serial.println("Move Gripper_Axis Z");
            #endif
            break;
            }//Fin Case X, Y et Z
        break;// fin Case U et D
      case 'P': // Stop Grippers
          switch (axe) 
            {
            case 'X': //Stop Axe X
            // On X axis
            GripX_stepper.stop();
            #if DEBUG
            Serial.println("Gripper_Axis X Stopped");
            #endif
            break;

            case 'Y': //Stop Axe Y
            // On Y axis
            GripY_stepper.stop();
            #if DEBUG
            Serial.println("Gripper_Axis Y Stopped");
            #endif
            break;

            case 'Z': //Stop Axe Z
            // On Z axis
            GripZ_stepper.stop();
            break;
            #if DEBUG
            Serial.println("Gripper_Axis Z Stopped");
            #endif
            }//Fin Case X, Y et Z case 'S'
        break;// du Case 'S'
        
      case 'O':// Open Gripper
        #if DEBUG
         Serial.println("Received 'O'");
        #endif
          switch (axe) 
            {
            case 'X': //Open Gripper X
            // On X axis
            #if DEBUG
             Serial.println("Opening Gripper X requested");
            #endif
            break;

            case 'Y': //Open Gripper  Y
            // On Y axis
            #if DEBUG
             Serial.println("Opening Gripper Y requested ");
            #endif
            break;

            case 'Z': //Open Gripper Z
            // On Z axis
            #if DEBUG
             Serial.println("Opening Gripper Z requested");
            #endif
            break;
            }//Fin Case X, Y et Z du case 'O'
        break;// du Case 'O'
        
      case 'C':// Close Gripper
        #if DEBUG
         Serial.println("Received 'C'");
        #endif
          switch (axe) 
            {
            case 'X': //Close Gripper X
            // On X axis
            #if DEBUG
            Serial.println("Closing Gripper X requested");
            #endif
            break;
            
            case 'Y': //Close Gripper  Y
            // On Y axis
            #if DEBUG
             Serial.println("Closing Gripper Y requested");
            #endif
            break;

            case 'Z': //Close Gripper Z
            // On Z axis
            #if DEBUG
             Serial.println("Closing Gripper Z requested");
            #endif
            break;
            }//Fin Case X, Y et Z du case 'C'
        break;// du Case 'C'
    }
    delay(50);
  }

  switch (mainOrder) {
    
    case 'F':
    case 'B':
      if (axe == 'X') {
        if (!(stepper2.distanceToGo() == 0 && stepper3.distanceToGo() == 0)) {
          stepper2.run();
          //Serial.print(stepper2.currentPosition());
          stepper3.run();
          //Serial.print("/");
          //Serial.println(stepper3.currentPosition());

        }
        else {
          #if DEBUG
          Serial.println("Distance Reached ! : ");
          #endif
          sendSteppersPositions();
          mainOrder = '*';
        digitalWrite(STEPPER2_MOTION_EN_PIN,HIGH);
        digitalWrite(STEPPER3_MOTION_EN_PIN,HIGH);
        }
      }
      if (axe == 'Y') {
        if (!(stepper1.distanceToGo() == 0 && stepper3.distanceToGo() == 0)) {
          stepper1.run();
          stepper3.run();
        }
        else {
          #if DEBUG
          Serial.println("Distance Reached ! : ");
          #endif
          sendSteppersPositions();
          mainOrder = '*';
        digitalWrite(STEPPER1_MOTION_EN_PIN,HIGH);
        digitalWrite(STEPPER3_MOTION_EN_PIN,HIGH);
        }
      }
      if (axe == 'Z') {
        if (!(stepper2.distanceToGo() == 0 && stepper1.distanceToGo() == 0)) {
          stepper2.run();
          stepper1.run();
        }
        else {
          #if DEBUG
          Serial.println("Distance Reached ! : ");
          #endif
          sendSteppersPositions();
          mainOrder = '*';
          digitalWrite(STEPPER1_MOTION_EN_PIN,HIGH);
          digitalWrite(STEPPER2_MOTION_EN_PIN,HIGH);
        }
      }
      break;
    case 'R':
      if (!(stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0 && stepper3.distanceToGo() == 0)) {
        stepper1.run();
        stepper2.run();
        stepper3.run();
      }
      else {
        #if DEBUG
        Serial.println("Angle Reached ! : ");
        #endif
        sendSteppersPositions();
        mainOrder = '*';
        digitalWrite(STEPPER1_MOTION_EN_PIN,HIGH);
        digitalWrite(STEPPER2_MOTION_EN_PIN,HIGH);
        digitalWrite(STEPPER3_MOTION_EN_PIN,HIGH);
          }
      break;
    case 'S':
      #if DEBUG
      Serial.println("STOP : ");
       #endif
      sendSteppersPositions();
      resetSteppersPositions();
      delay(20);
      digitalWrite(STEPPER1_MOTION_EN_PIN,HIGH);
      digitalWrite(STEPPER2_MOTION_EN_PIN,HIGH);
      digitalWrite(STEPPER3_MOTION_EN_PIN,HIGH);
      mainOrder = '*';
      break;
    case 'U':
    case 'D':
      if (axe == 'X') 
        {
          if (!(GripX_stepper.distanceToGo() == 0)) 
          {
          digitalWrite(GRIPX_STEPPER_EN_PIN,LOW);
          GripX_stepper.run();
          }
          else 
          {
          #if DEBUG
           Serial.println("Height X Reached!");
          #endif
          delay(10);
          digitalWrite(GRIPX_STEPPER_EN_PIN,HIGH);
          sendGrippersPositions();
          mainOrder = '*';
          }
        }//if Axe X
      if (axe == 'Y') 
        {
          if (!(GripY_stepper.distanceToGo() == 0)) 
          {
          digitalWrite(GRIPY_STEPPER_EN_PIN,LOW);  
          GripY_stepper.run();
          }
          else 
          {
          #if DEBUG
           Serial.println("Height Y Reached!");
          #endif  
          digitalWrite(GRIPY_STEPPER_EN_PIN,HIGH);
          sendGrippersPositions();
          mainOrder = '*';
          }
        }//Fin Axe Y
        
      if (axe == 'Z') 
        {
          if (!(GripZ_stepper.distanceToGo() == 0)) 
          {
          digitalWrite(GRIPZ_STEPPER_EN_PIN,LOW);  
          GripZ_stepper.run();
          }
          else 
          {
          digitalWrite(GRIPZ_STEPPER_EN_PIN,HIGH);  
          sendGrippersPositions();
          mainOrder = '*';
          #if DEBUG
           Serial.println("Height Z Reached!");
          #endif
          }
        } //Fin Axe Z
      break; // du Case 'U' et'D'
    case 'O':
      if (axe == 'X') 
        {
          digitalWrite(OE_Servos_PIN, LOW);// Servo ON
          Servo_Gripper.writeMicroseconds(X_Servo_PIN, 1000); // Ouverture pince X;
          mainOrder = '*';
          delay(1000);
          digitalWrite(OE_Servos_PIN, HIGH);// Servo OFF
          #if DEBUG
           Serial.println("Open GripperX");
          #endif
          sendGrippersPositions();
         } //if Axe X
         
      if (axe == 'Y') 
        {
          digitalWrite(OE_Servos_PIN, LOW); // Servo ON
          Servo_Gripper.writeMicroseconds(Y_Servo_PIN, 1000); // Ouverture pince Y;
          mainOrder = '*';
          delay(1000);
          digitalWrite(OE_Servos_PIN, HIGH); //Servo OFF
          #if DEBUG
           Serial.println("Open GripperY!");
          #endif
          sendGrippersPositions();
          }//Fin Axe Y
       
      if (axe == 'Z') 
        {
          digitalWrite(OE_Servos_PIN, LOW);// Servo ON
          Servo_Gripper.writeMicroseconds(Z_Servo_PIN, 1000); // Ouverture pince Z;
          mainOrder = '*';
          delay(1000);
          digitalWrite(OE_Servos_PIN, HIGH); //Servo OFF
          #if DEBUG
           Serial.println("Open GripperZ!");
          #endif
          sendGrippersPositions();
          }//Fin Axe Z
      break; // du Case 'O'

    case 'C':
      if (axe == 'X') 
        {
          digitalWrite(OE_Servos_PIN, LOW);// Servo ON
          Servo_Gripper.writeMicroseconds(X_Servo_PIN, 1800); // Fermeture pince X;
          mainOrder = '*';
          delay(100);
          //digitalWrite(OE_Servos_PIN, HIGH); //Servo OFF
          #if DEBUG
           Serial.println("Closed GripperX");
          #endif
          sendGrippersPositions();
         } //if Axe X
         
      if (axe == 'Y') 
        {
          digitalWrite(OE_Servos_PIN, LOW);// Servo ON
          Servo_Gripper.writeMicroseconds(Y_Servo_PIN, 1800); // Fermeture pince Y;
          mainOrder = '*';
          delay(100);
          //digitalWrite(OE_Servos_PIN, HIGH); //Servo OFF
          #if DEBUG
           Serial.println("Closed GripperY!");
          #endif
          sendGrippersPositions();
          }//Fin Axe Y
       
      if (axe == 'Z') 
        {
          digitalWrite(OE_Servos_PIN, LOW);// Servo ON
          Servo_Gripper.writeMicroseconds(Z_Servo_PIN, 1800); // Fermeture pince Z;
          mainOrder = '*';
          delay(100);
          //digitalWrite(OE_Servos_PIN, HIGH); //Servo OFF
          #if DEBUG
           Serial.println("Closed GripperZ!");
          #endif
          sendGrippersPositions();
          }//Fin Axe Z
       break; // du Case 'C'    
  }
}// Fin loop




// Send message to Master with steps done for each steppers
void sendSteppersPositions() {
  Serial.print(stepper1.currentPosition());
  Serial.print("/");
  Serial.print(stepper2.currentPosition());
  Serial.print("/");
  Serial.println(stepper3.currentPosition());
}

// Reset steppers steps counters
void resetSteppersPositions() {
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);
  stepper3.setCurrentPosition(0);
}

// Reset grippers steps counters
void resetGrippersPositions() {
  GripX_stepper.setCurrentPosition(0);
  GripY_stepper.setCurrentPosition(0);
  GripZ_stepper.setCurrentPosition(0);
}

// Send message to Master with steps done for each gripper
void sendGrippersPositions() {
  Serial.print(GripX_stepper.currentPosition());
  Serial.print("/");
  Serial.print(GripY_stepper.currentPosition());
  Serial.print("/");
  Serial.println(GripZ_stepper.currentPosition());
}

void disable_en_motion_stepper(){
  // Motion STEPPER1, 2 and 3 OFF(FREE)
  digitalWrite(STEPPER1_MOTION_EN_PIN,HIGH);
  digitalWrite(STEPPER2_MOTION_EN_PIN,HIGH);
  digitalWrite(STEPPER3_MOTION_EN_PIN,HIGH);
}

void initialisationAllTof(){
  /* Fonction qui vérifie que les ToF soient bien branché et initialisé,
   * En considérant que les ToF grabX,grabY,grabZ sont branché sur (0,1,2)
   * et les ToF de collision XY,YZ,ZX sont branché sur (3,4,5)
   *  ATTENTION au premier téléversement le premier port I2C est beugué, il faut juste changer de prise I2C du Mux sur l'arduino
   */
  delay(1000);
  multiplexeurI2C.openChannel(0);
  if(!tofSensorGrabX.init()){
    Serial.println("Ne peut pas initialiser le telemetre de la pince x");
  }
  multiplexeurI2C.closeChannel(0);
  multiplexeurI2C.openChannel(1);
  if(!tofSensorGrabY.init()){
    Serial.println("Ne peut pas initialiser le telemetre de la pince y");
  }
  multiplexeurI2C.closeChannel(1);
  multiplexeurI2C.openChannel(2);
  if(!tofSensorGrabZ.init()){
    Serial.println("Ne peut pas initialiser le telemetre de la pince z");
  }
  multiplexeurI2C.closeChannel(2);
  multiplexeurI2C.openChannel(3);
  if(!tofSensorCollisionXY.init()){
    Serial.println("Ne peut pas initialiser le telemetre Collision XY");
  }
  multiplexeurI2C.closeChannel(3);
  multiplexeurI2C.openChannel(4);
  if(!tofSensorCollisionYZ.init()){
    Serial.println("Ne peut pas initialiser le telemetre Collision YZ");
  }
  multiplexeurI2C.closeChannel(4);
  multiplexeurI2C.openChannel(5);
  if(!tofSensorCollisionZX.init()){
    Serial.println("Ne peut pas initialiser le telemetre Collision ZX");
  }
  multiplexeurI2C.closeChannel(5);
  multiplexeurI2C.closeAll();
}

void ReadAllToF() {
  // Fonction qui va lire et afficher tout les telemetres et les prints dans le port série.
  Serial.println("LECTURE DES TOFS : ");
  multiplexeurI2C.openChannel(0);
  Serial.println(tofSensorGrabX.readRangeSingleMillimeters());
  multiplexeurI2C.closeChannel(0);
  multiplexeurI2C.openChannel(1);
  Serial.println(tofSensorGrabY.readRangeSingleMillimeters());
  multiplexeurI2C.closeChannel(1);
  multiplexeurI2C.openChannel(2);
  Serial.println(tofSensorGrabZ.readRangeSingleMillimeters());
  multiplexeurI2C.closeChannel(2);
  multiplexeurI2C.openChannel(3);
  Serial.println(tofSensorCollisionXY.readRangeSingleMillimeters());
  multiplexeurI2C.closeChannel(3);
  multiplexeurI2C.openChannel(4);
  Serial.println(tofSensorCollisionYZ.readRangeSingleMillimeters());
  multiplexeurI2C.closeChannel(4);
  multiplexeurI2C.openChannel(5);
  Serial.println(tofSensorCollisionYZ.readRangeSingleMillimeters());
  multiplexeurI2C.closeChannel(5);
  multiplexeurI2C.closeAll();
}

int ReadOneToF(int port){
  /*
  * Fonction retournant la valeur en millimètre de la distance mesurée par le télémètre
  * En paramètre on passe la valeur de la porte 0,1,2,3,4,5,6,7
  * 
  */
  int distance=0;
  switch(port){
    case 0 :
    multiplexeurI2C.openChannel(0);
    distance = tofSensorGrabX.readRangeSingleMillimeters();
    multiplexeurI2C.closeChannel(0);
    return distance;
    case 1 :
    multiplexeurI2C.openChannel(1);
    distance = tofSensorGrabY.readRangeSingleMillimeters();
    multiplexeurI2C.closeChannel(1);
    return distance;
    case 2 :
    multiplexeurI2C.openChannel(2);
    distance = tofSensorGrabZ.readRangeSingleMillimeters();
    multiplexeurI2C.closeChannel(2);
    return distance;
    case 3 :
    multiplexeurI2C.openChannel(3);
    distance = tofSensorCollisionXY.readRangeSingleMillimeters();
    multiplexeurI2C.closeChannel(3);
    return distance;
    case 4 :
    multiplexeurI2C.openChannel(4);
    distance = tofSensorCollisionYZ.readRangeSingleMillimeters();
    multiplexeurI2C.closeChannel(4);
    return distance;
    case 5 :
    multiplexeurI2C.openChannel(5);
    distance = tofSensorCollisionZX.readRangeSingleMillimeters();
    multiplexeurI2C.closeChannel(5);
    return distance;
    default : 
    multiplexeurI2C.closeAll();
    return distance;
  }
}
