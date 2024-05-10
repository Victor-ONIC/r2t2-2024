/***********************************************************************
 * Motion_Control_and_Grippers_SerialV3.6.ino
 * Carte Arduino MEGA2560 R3
 * Sur Shield Grove MEGA2560
 * Author : MS/PG
 * Date   : 11/03/2024
 * Review : 13/03/2024 V2
 * Review : 17/03/2024 V3
 * Review : 19/03/2024 V3.1 (en for Stop command)
 * Review : 30/03/2024 V3.2 jack pin A6 et comm XBEE
 * Review : 02/04/2024 V3.3 Calibrate Grippers
 * Review : 15/04/2024 V3.4 Add T command to restart after stop
 *                          Add timer run
 * Review : 22/04/2024 V3.5 Add tof (evitement et plantes)
 * Review : 27/04/2024 V3.5.1 Change and define Interrupts PINs For TOF Gripper and Motion
 * Review : 02/05/2024 V3.6   Use with tof_UNO_V1.3 which gives forInterrupt signal
 *                            composed of a logical OR combination of all tofs
 * Review : 06/05/2024 V3.7 Fix bad claw grip strength causing reboot
                            Add PAMI calls
 * Review : 06/05/2024 V3.71 Add redundancy to call PAMI
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
 *   - 'd' : Pami order to L-1 Station
 *   - 'g' : Pami order to L-3 Station
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
 * RUN - 
 * Debug false
 * Timeron true
 * Calibrate true
 * TOF true
 * 
 * jack activé
 * setup X seule est en bas
 * 
 ************************************************************************/
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <AccelStepper.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>

// Set DEBUG to false to disable debug messages
#define DEBUG true
#define TIMERON true
#define CALIBRATE false
#define TOF false

#define BAUDRATE 9600
#define MAXSPEED 6000 //V2 4000 V3
#define ACCELERATION 2000 //V2 1000 V3
#define MAXSPEED_Grip 6500 // Pour avoir un mouvement plus "fluide"
#define ACCELERATION_Grip 3500
#define DRIVER AccelStepper::DRIVER
#define JACK A6

#define STEPPER1_DIR_PIN 6  // STEPPER ON X Axis
#define STEPPER2_DIR_PIN 8 // STEPPER ON Y Axis
#define STEPPER3_DIR_PIN 10 //STEPPER ON Z Axis

#define STEPPER1_STEP_PIN 7
#define STEPPER2_STEP_PIN 9
#define STEPPER3_STEP_PIN 11

//Valide ou non les 3 moteurs de déplacement
#define STEPPER1_MOTION_EN_PIN 54 // (A0)
#define STEPPER2_MOTION_EN_PIN 55 // (A1)
#define STEPPER3_MOTION_EN_PIN 56 // (A2)

/* Grippers Steppers define*/
#define GRIPX_STEPPER_DIR_PIN  62  //(A8 sur Grove Shield MEGA)
#define GRIPY_STEPPER_DIR_PIN  64  //(A10 sur Grove Shield MEGA)
#define GRIPZ_STEPPER_DIR_PIN  66  //(A12 sur Grove Shield MEGA)
#define GRIPX_STEPPER_STEP_PIN 63  //(A9 sur Grove Shield MEGA)
#define GRIPY_STEPPER_STEP_PIN 65  //(A11 sur Grove Shield MEGA)
#define GRIPZ_STEPPER_STEP_PIN 67 //(A13 sur Grove Shield MEGA)

/* For Grippers limits*/
#define XLimit_PIN   42 // (vert) Modif pour récupére INT4
#define YLimit_PIN   43 // (blanc/gris) Modif pour récupére INT5
#define ZLimit_PIN   4 

/*For Stepper Grippers Enable*/
#define GRIPX_STEPPER_EN_PIN 57//(A3)
#define GRIPY_STEPPER_EN_PIN 58 //(A4)
#define GRIPZ_STEPPER_EN_PIN 59 //(A5)

/*Grippers Servos frequency*/
#define SERVO_FREQ 50 // Fréquence 50Hz soit 20ms de période pour les servos
/* Connexion sur carte Adafruit PCA9585*/
#define X_Servo_PIN 0
#define Y_Servo_PIN 1
#define Z_Servo_PIN 2
/* Connexion sur MEGA2560*/
#define OE_Servos_PIN 5

/* From Grippers TOF Pins(Gestion par INT Externe) */
#define tofGX 2  // D2 sur UNO
#define tofGY 3  // D3 sur UNO
#define tofGZ 12 // D4 sur UNO

/* From  Axis TOF Pins(Gestion par PortChange interrupt */
#define tofXY 13  // D5 sur UNO
#define tofYZ 14  // D6 sur UNO
#define tofZX 15  // D7 sur UNO

#define forInterrupt 19


SoftwareSerial xbeeSerial(68,69); // RX(A14), TX(A15) V3.5.1

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
char mainOrder = '*';
char old_mainOrder = '*';
char axe;
char old_axe;
long dist;
long height;// Hauteur de la pince(200pas=1 tour=8mm)
long steps;
int deg;
long remaining_steps;
long angle_steps;
volatile int count = 0;
bool _end=false;
char pami;
bool return_from_stop = false;

void setup()
{
  Serial.begin(BAUDRATE);
  xbeeSerial.begin(BAUDRATE);
  #if DEBUG
  Serial.println("Start...");
  #endif

  #if DEBUG
  Serial.println("TOF Initialisation..");
  #endif
  pinMode(tofGX, INPUT);
  pinMode(tofGY, INPUT);
  pinMode(tofGZ, INPUT);
  // For Axis X,Y, Z
  pinMode(tofXY, INPUT);
  pinMode(tofYZ, INPUT);
  pinMode(tofZX, INPUT);

  pinMode(forInterrupt, INPUT_PULLUP);

  #if TOF
  attachInterrupt(digitalPinToInterrupt(forInterrupt), obstacleDetected, CHANGE);
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

  disable_en_motion_stepper();
  
  #if DEBUG
  Serial.println("OK !");
  #endif

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

  #if CALIBRATE
  // Calibrage zero position basse Grippers 
  digitalWrite(GRIPX_STEPPER_DIR_PIN,LOW); // LOW descente/HIGH montée
  digitalWrite(GRIPY_STEPPER_DIR_PIN,LOW);  
  digitalWrite(GRIPZ_STEPPER_DIR_PIN,LOW);
  
  byte stop_switchX = digitalRead(XLimit_PIN);
  byte stop_switchY = digitalRead(YLimit_PIN);
  byte stop_switchZ = digitalRead(ZLimit_PIN);
  
  #if DEBUG
  Serial.print(stop_switchX);
  Serial.print(stop_switchY);
  Serial.println(stop_switchZ);
  #endif

  // stop_switch at 0 if  XLimit_PIN is LOW 
  // GRIPX_STEPPER ON
  digitalWrite(GRIPX_STEPPER_EN_PIN,LOW);
  while(stop_switchX)
  {
    digitalWrite(GRIPX_STEPPER_STEP_PIN, HIGH);
    delayMicroseconds(600);
    digitalWrite(GRIPX_STEPPER_STEP_PIN, LOW);
    delayMicroseconds(600);
    stop_switchX = digitalRead(XLimit_PIN);
  }
  // Initial high position X to solar panel
  digitalWrite(GRIPX_STEPPER_DIR_PIN,HIGH);
  for(int s; s<500;s++){
    digitalWrite(GRIPX_STEPPER_STEP_PIN, HIGH);
    delayMicroseconds(600);
    digitalWrite(GRIPX_STEPPER_STEP_PIN, LOW);
    delayMicroseconds(600);
  }
  // GRIPX_STEPPER OFF(FREE)
  digitalWrite(GRIPX_STEPPER_EN_PIN,HIGH);

  // stop_switch at 0 if  YLimit_PIN is LOW 
  // GRIPY_STEPPER ON
  digitalWrite(GRIPY_STEPPER_EN_PIN,LOW);
  while(stop_switchY)
  {
    digitalWrite(GRIPY_STEPPER_STEP_PIN, HIGH);
    delayMicroseconds(600);
    digitalWrite(GRIPY_STEPPER_STEP_PIN, LOW);
    delayMicroseconds(600);
    stop_switchY = digitalRead(YLimit_PIN); 
  } 
  delay(10);
  // Initial high position Y to solar panel
  digitalWrite(GRIPY_STEPPER_DIR_PIN,HIGH);
  for(int s=0; s<1900;s++){
    digitalWrite(GRIPY_STEPPER_STEP_PIN, HIGH);
    delayMicroseconds(600);
    digitalWrite(GRIPY_STEPPER_STEP_PIN, LOW);
    delayMicroseconds(600);
  }
  // GRIPY_STEPPER OFF (FREE)
  digitalWrite(GRIPY_STEPPER_EN_PIN,HIGH);
  
  // stop_switch at 0 if  ZLimit_PIN is LOW 
  // GRIPZ_STEPPER ON
  digitalWrite(GRIPZ_STEPPER_EN_PIN,LOW);
  while(stop_switchZ)
  {
    digitalWrite(GRIPZ_STEPPER_STEP_PIN, HIGH);
    delayMicroseconds(600);
    digitalWrite(GRIPZ_STEPPER_STEP_PIN, LOW);
    delayMicroseconds(600);
    stop_switchZ = digitalRead(ZLimit_PIN); 
  } 
  // Initial high position Z to solar panel
  digitalWrite(GRIPZ_STEPPER_DIR_PIN,HIGH);
  for(int s=0; s<2100;s++){
    digitalWrite(GRIPZ_STEPPER_STEP_PIN, HIGH);
    delayMicroseconds(600);
    digitalWrite(GRIPZ_STEPPER_STEP_PIN, LOW);
    delayMicroseconds(600);
  }
  #endif

  // GRIPX_STEPPER OFF(FREE)
  digitalWrite(GRIPX_STEPPER_EN_PIN,HIGH);
  // GRIPY_STEPPER OFF(FREE)
  digitalWrite(GRIPY_STEPPER_EN_PIN,HIGH);
  // GRIPZ_STEPPER OFF (FREE)
  digitalWrite(GRIPZ_STEPPER_EN_PIN,HIGH);
  
  #if DEBUG
  Serial.print("Disable Motion Stepper EN : ");
  Serial.println(String(digitalRead(STEPPER1_MOTION_EN_PIN)) + " - " +  String(digitalRead(STEPPER2_MOTION_EN_PIN)) + " - "+ String(digitalRead(STEPPER3_MOTION_EN_PIN)));
  #endif
  

  sendSteppersPositions();
    #if DEBUG
  Serial.print("Jack :");
  Serial.println(digitalRead(JACK));
  #endif
  
  while(digitalRead(JACK)); 

  #if DEBUG
  Serial.println("Jack out...");
  #endif

  #if TIMERON
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(end_of_run);
  #endif
  
 }


void loop()
{
  // if order received
  if (Serial.available()>0 || mainOrder == 'S' || mainOrder == 'T') {
    if(mainOrder!='S' && mainOrder!='T'){
      order = Serial.readStringUntil('\n');
      #if DEBUG
      Serial.print("order : ");
      Serial.println(order);
      #endif
      // Split order
      mainOrder = order[0];
      axe = order[1];
    }
    

    // Reset stepper steps counters if not Stop Order
    if (mainOrder != 'S') resetSteppersPositions();

    switch (mainOrder) {
      case 'g': // ordre départ PAMIs
        pami = 'g';
        sendGrippersPositions();
        #if DEBUG
        Serial.print("PAMI = ");
        Serial.println(pami);
        #endif
        break;
      case 'd': // ordre départ PAMIs
        pami = 'd';
        sendGrippersPositions();
        #if DEBUG
        Serial.print("PAMI = ");
        Serial.println(pami);
        #endif
        break;
      // Order to Forward or Backward Robot Motion
      case 'F': 
      case 'B': 
        old_mainOrder = mainOrder;
        old_axe = axe;
        // Compute steps to forward to do distance
        dist = order.substring(2).toInt();
        steps = long(dist * 1600 / 212);// Réglage sur Drivers au micropas au 1/8
        go();
        break;// du case F/B
        
      case 'R':
          // Order to Rotate
        #if DEBUG
        Serial.println("Received 'R'");
        #endif
        old_mainOrder = 'R';
        deg = order.substring(1).toInt();
        angle_steps = 25 * deg;
        rotation();
        break;

      case 'S':
        #if DEBUG
        Serial.println("Reveived 'S'");
        #endif
        stepper1.stop();
        stepper2.stop();
        stepper3.stop();
        break;

      case 'T':
        // Order To finish the movement
        #if DEBUG
        Serial.println("Reveived 'T' ... ");
        Serial.print("Old Main Order : ");
        Serial.println(old_mainOrder);
        #endif
        if(old_mainOrder == 'R'){
          mainOrder = old_mainOrder;
          #if DEBUG
          Serial.print("Remaining steps = ");
          Serial.println(remaining_steps);
          #endif
          angle_steps = remaining_steps;
          rotation();
        }
        else if(old_mainOrder == 'F' || old_mainOrder == 'B'){
          mainOrder = old_mainOrder;
          axe = old_axe;
          #if DEBUG
          Serial.print("Axe : ");
          Serial.println(axe);
          Serial.print("Remaining steps = ");
          Serial.println(remaining_steps);
          #endif
          steps = remaining_steps;
          
          go();
        }
        
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
          Servo_Gripper.writeMicroseconds(X_Servo_PIN, 1775); // Fermeture pince X;
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
          Servo_Gripper.writeMicroseconds(Z_Servo_PIN, 1750); // Fermeture pince Z;
          mainOrder = '*';
          delay(100);
          //digitalWrite(OE_Servos_PIN, HIGH); //Servo OFF
          #if DEBUG
           Serial.println("Closed GripperZ!");
          #endif
          sendGrippersPositions();
          }//Fin Axe Z
       break; // du Case 'C'    
    
    case 'F':
    case 'B':
      if (axe == 'X') {
        if (!(stepper2.distanceToGo() == 0 && stepper3.distanceToGo() == 0)) {
            stepper2.run();
            stepper3.run();
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

      delay(20);
      int stepper1Pos = stepper1.currentPosition();
      int stepper2Pos = stepper2.currentPosition();
      int stepper3Pos = stepper3.currentPosition();
      if(old_mainOrder == 'F' || old_mainOrder == 'B' || old_mainOrder == 'R')
      {
        if(old_mainOrder == 'R'){
          remaining_steps = angle_steps - stepper1Pos;
          #if DEBUG
          Serial.print("STOP : remaining steps = ");
          Serial.println(remaining_steps);
          #endif
        }
        else{
          remaining_steps = abs(abs(steps) - long((abs(stepper1Pos)+abs(stepper2Pos)+abs(stepper3Pos))/2));
          steps = remaining_steps;
          #if DEBUG
          Serial.print("STOP : remaining steps = ");
          Serial.println(remaining_steps);
          #endif
        }
      
      }
      //sendSteppersPositions();//Comment for use during run
      resetSteppersPositions();
      
      digitalWrite(STEPPER1_MOTION_EN_PIN,HIGH);
      digitalWrite(STEPPER2_MOTION_EN_PIN,HIGH);
      digitalWrite(STEPPER3_MOTION_EN_PIN,HIGH);
      mainOrder = '*';
      break;
    
  }
}// Fin loop

void go(){
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
}

void rotation(){
  digitalWrite(STEPPER1_MOTION_EN_PIN,LOW);
  digitalWrite(STEPPER2_MOTION_EN_PIN,LOW);
  digitalWrite(STEPPER3_MOTION_EN_PIN,LOW); 
  #if DEBUG
  Serial.print("angle_steps :");
  Serial.println(angle_steps);
  #endif
  stepper1.move(angle_steps);
  stepper2.move(angle_steps);
  stepper3.move(angle_steps);
}


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

void end_of_run(void)
{
  #if DEBUG
  Serial.println(count);
  #endif
  count++;
  if (count >= 10 && count<1000) {
    delay(20);
    while(!_end){
      xbeeSerial.print(pami);
      delay(20);
      xbeeSerial.print(pami);
      delay(20);
      xbeeSerial.print(pami);
      delay(20);
      xbeeSerial.print(pami);
      delay(20);
      _end = true;
      count = 1000;
    }
  }

  if(count>1010){
    #if DEBUG
    Serial.print("End");
    #endif
    while(1);
  }
}

void obstacleDetected(){
  // Obstacle decteted
  if(digitalRead(forInterrupt)){

    int FX = digitalRead(tofGX);
    int FY = digitalRead(tofGY);
    int FZ = digitalRead(tofGZ);
    int BX = digitalRead(tofYZ);
    int BY = digitalRead(tofZX);
    int BZ = digitalRead(tofXY);
    
    #if DEBUG
    Serial.print("obstacleDetected() : ");
    Serial.print(mainOrder);
    Serial.print(" on axe ");
    Serial.println(axe);

    Serial.print(FX);
    Serial.print(FY);
    Serial.print(FZ);
    Serial.print(BX);
    Serial.print(BY);
    Serial.println(BZ);
    #endif
    
    if(FX && mainOrder=='F' && axe == 'X'){
      #if DEBUG
      Serial.println("tof GripperX : High");
      #endif
      mainOrder = 'S';
      return_from_stop=true;
    }
    if(FY && mainOrder=='F' && axe == 'Y'){
      #if DEBUG
      Serial.println("tof GripperY : High");
      #endif
      mainOrder = 'S';
      return_from_stop=true;
    }
    if(FZ && mainOrder=='F' && axe == 'Z'){
      #if DEBUG
      Serial.println("tof GripperZ : High");
      #endif
      mainOrder = 'S';
      return_from_stop=true;
    }
    if(BZ && mainOrder=='B' && axe == 'Z'){
      #if DEBUG
      Serial.println("tof /Z : High");
      #endif
      mainOrder = 'S';
      return_from_stop=true;
    }
    if(BX && mainOrder=='B' && axe == 'X'){
      #if DEBUG
      Serial.println("tof /X : High");
      #endif
      mainOrder = 'S';
      return_from_stop=true;
    }
    if(BY && mainOrder=='B' && axe == 'Y'){
      #if DEBUG
      Serial.println("tof /Y : High");
      #endif
      mainOrder = 'S';
      return_from_stop=true;
    }
  }
  // Horizon is clean : Continue the initial movment
  else{
    #if DEBUG
    Serial.println("Horizon is clear");
    #endif
    if(return_from_stop) mainOrder = 'T';
    return_from_stop=false;
  }
}
