/*****************************************************************************
* tof_UNO_V1.3
* Carte Arduino UNO R3
* Sur Shield Grove
* Author : MS
* Review : 30/04/2024
* 
* Description : TOF management with VL53L0X Library from Polulu
* TOF use a I2C multiplexer whith TCA9548A Library from Jonathan Dempsay
* Use LED define to true for visual control (add 4 LED on A0 to A3)
* ReadOneTof(channel) return the distance measured by the tof on the channel in mm
* tof digital pin is HIGH when the distance measured is lower than dist_to_stop
* toInterrupt pin is HIGH when a tof is HIGH
* Each TOF level is connected to the MEGA
* 
******************************************************************************/


#include <VL53L0X.h> // Bibliothèque des ToF (POLULU)
#include <TCA9548A.h> //Bibliothèque du mux (Jonathan Dempsay)

/*Instanciate Mux and ToF*/
TCA9548A multiplexeurI2C;
VL53L0X tofSensorGrabX;
VL53L0X tofSensorGrabY;
VL53L0X tofSensorGrabZ;
VL53L0X tofSensorCollisionXY;
VL53L0X tofSensorCollisionYZ;
VL53L0X tofSensorCollisionZX;

#define DEBUG false
#define LED false

#define tofGX 2
#define tofGY 3
#define tofGZ 4

// Tofs Axis
#define tofXY 5
#define tofYZ 6
#define tofZX 7

// to interrupt Mega
#define toInterrupt 8

// Distance to obstacle in mm
#define dist_to_stop 120

int tof[6] = {tofGX, tofGY, tofGZ, tofXY, tofYZ, tofZX};

void setup() {

  Serial.begin(9600);
  Serial.println("Start...");
  multiplexeurI2C.begin(Wire);
  initialisationAllTof();

  // For Grippers
  pinMode(tofGX, OUTPUT);
  pinMode(tofGY, OUTPUT);
  pinMode(tofGZ, OUTPUT);

  // For Axis
  pinMode(tofXY, OUTPUT);
  pinMode(tofYZ, OUTPUT);
  pinMode(tofZX, OUTPUT);

  // to interrupt Mega
  pinMode(toInterrupt, OUTPUT);
  digitalWrite(toInterrupt, LOW);
  
  for (int i = 0; i < 6; i++) {
    digitalWrite(tof[i], LOW);
  }

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);

  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  for (int channel = 0; channel < 6; channel++) {
    int dist = ReadOneToF(channel);
    #if DEBUG
    Serial.println("Channel " + String(channel) + " : " + String(dist));
    #endif
    delay(10);
    if (dist < dist_to_stop) {
      digitalWrite(tof[channel], HIGH);
      #if LED
      ledON(tof[channel]);
      #endif
    }
    else {
      digitalWrite(tof[channel], LOW);
      #if LED
      ledOFF(tof[channel]);
      #endif
    }
    bool obstacle = digitalRead(tof[0]) | digitalRead(tof[1]) |
                    digitalRead(tof[2]) | digitalRead(tof[3]) |
                    digitalRead(tof[4]) | digitalRead(tof[5]);
    #if DEBUG
    Serial.print("Obstacle : ");
    Serial.println(obstacle);
    #endif
    digitalWrite(toInterrupt, obstacle);              
  }

}

void initialisationAllTof() {
  /* Fonction qui vérifie que les ToF soient bien branché et initialisé,
     En considérant que les ToF grabX,grabY,grabZ sont branché sur (0,1,2)
     et les ToF de collision XY,YZ,ZX sont branché sur (3,4,5)
      ATTENTION au premier téléversement le premier port I2C est beugué, il faut juste changer de prise I2C du Mux sur l'arduino
  */
  delay(1000);
  multiplexeurI2C.openChannel(0);
  if (!tofSensorGrabX.init()) {
    Serial.println("Ne peut pas initialiser le telemetre de la pince x");
  }
  else {
    Serial.println("Telemetre de la pince x : OK");
  }
  delay(10);

  multiplexeurI2C.closeChannel(0);
  multiplexeurI2C.openChannel(1);
  if (!tofSensorGrabY.init()) {
    Serial.println("Ne peut pas initialiser le telemetre de la pince y");
  }
  else {
    Serial.println("Telemetre de la pince y : OK");
  }
  delay(10);

  multiplexeurI2C.closeChannel(1);
  multiplexeurI2C.openChannel(2);
  if (!tofSensorGrabZ.init()) {
    Serial.println("Ne peut pas initialiser le telemetre de la pince z");
  }
  else {
    Serial.println("Telemetre de la pince z : OK");
  }

  delay(10);
  multiplexeurI2C.closeChannel(2);

  multiplexeurI2C.openChannel(3);
  if (!tofSensorCollisionXY.init()) {
    Serial.println("Ne peut pas initialiser le télèmetre de collision XY");
  }
  else {
    Serial.println("Télemètre de collision XY: OK");
  }
  delay(10);
  multiplexeurI2C.closeChannel(3);

  multiplexeurI2C.openChannel(4);
  if (!tofSensorCollisionYZ.init()) {
    Serial.println("Ne peut pas initialiser le télèmetre de collision YZ");
  }
  else {
    Serial.println("Télemètre de collision YZ: OK");
  }
  delay(10);
  multiplexeurI2C.closeChannel(4);

  multiplexeurI2C.openChannel(5);
  if (!tofSensorCollisionZX.init()) {
    Serial.println("Ne peut pas initialiser le télèmetre de collision ZX");
  }
  else {
    Serial.println("Télemètre de collision ZX: OK");
  }
  delay(10);
  multiplexeurI2C.closeAll();

}

int ReadOneToF(int port) {
  /*
    Fonction retournant la valeur en millimètre de la distance mesurée par le télémètre
    En paramètre on passe la valeur de la porte 0,1,2,3,4,5,6,7

  */
  int distance = 0;
  switch (port) {
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


  }
}

// LED for visual control
void ledON(int led) {
  switch (led) {
    case tofGX:
      digitalWrite(A0, HIGH);
      break;
    case tofGY:
      digitalWrite(A1, HIGH);
      break;
    case tofGZ:
      digitalWrite(A2, HIGH);
      break;
    case tofXY:
      digitalWrite(A3, HIGH);
      break;
    case tofYZ:
      digitalWrite(A1, HIGH);
      break;
    case tofZX:
      digitalWrite(A2, HIGH);
      break;

  }
}

void ledOFF(int led) {
  switch (led) {
    case tofGX:
      digitalWrite(A0, LOW);
      break;
    case tofGY:
      digitalWrite(A1, LOW);
      break;
    case tofGZ:
      digitalWrite(A2, LOW);
      break;
    case tofXY:
      digitalWrite(A3, LOW);
      break;
    case tofYZ:
      digitalWrite(A1, LOW);
      break;
    case tofZX:
      digitalWrite(A2, LOW);
      break;

  }
}
