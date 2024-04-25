/* Projet Robot 2024
 * Ce code permet reÃ§u par le PAMI permet qu'il dÃ©marre son RUN
 * dÃ¨s rÃ©ception de l'ordre G(Go) Ã©mis par le Robot.
 * Version du 11 avril 2024
 * Xbee_PAMI2_V2.1.ino.Ajout du SR04
 * Author :MS/PG
 */
#include <SoftwareSerial.h>
#include <AccelStepper.h>
#include <MsTimer2.h>


#define BAUDRATE 9600
#define MAXSPEED_PAMI3 6000 // Pour avoir un mouvement plus "fluide"
#define ACCELERATION_PAMI3 1500
#define DRIVER AccelStepper::DRIVER
#define DEBUG true
/* Stepper motor  Righ*/
#define MR_DIR_PIN 3
#define MR_STEP_PIN 4
#define MR_EN_PIN A0
/* Stepper motor Left*/
#define ML_DIR_PIN 5
#define ML_STEP_PIN 6
#define ML_EN_PIN A1
#define DRIVER AccelStepper::DRIVER

SoftwareSerial xbeeSerial(8, 9);  // RX, TX
AccelStepper Right_stepper(DRIVER, MR_STEP_PIN, MR_DIR_PIN);
AccelStepper Left_stepper(DRIVER, ML_STEP_PIN, ML_DIR_PIN);

String Order;
char MainOrder;
const byte interruptPin = 2;  // EchoBack
bool fin = false;

void setup() {
  Serial.begin(BAUDRATE);
  xbeeSerial.begin(BAUDRATE);
  pinMode(interruptPin, INPUT);
  pinMode(MR_EN_PIN, OUTPUT);
  pinMode(ML_EN_PIN, OUTPUT);
  /* Disable Stepper Drivers*/
  digitalWrite(MR_EN_PIN, HIGH);  // Low Level Enable
  digitalWrite(ML_EN_PIN, HIGH);  // Low Level Enable
  Right_stepper.setMaxSpeed(MAXSPEED_PAMI3);
  Right_stepper.setAcceleration(ACCELERATION_PAMI3);

  Left_stepper.setMaxSpeed(MAXSPEED_PAMI3);
  Left_stepper.setAcceleration(ACCELERATION_PAMI3);
  disable_en_motion_stepper();
}

void loop() {
  if (xbeeSerial.available() > 0) {
    // Lire le message reçu via XBee
    Order = xbeeSerial.readStringUntil('\n');
#if DEBUG
    Serial.print("Order : ");
    Serial.println(Order);
#endif

    // Split order
    MainOrder = Order.charAt(0);
    while (!(MainOrder == 'g' || MainOrder == 'd'));  // Attente ordre départ PAMI3
#if DEBUG
    Serial.println("Message reçu par XBee_PAMI3!");
#endif
    MsTimer2::set(10000, Run_PAMI2);
    MsTimer2::start();
#if DEBUG
    Serial.println("Start_Run_PAMI3!");
#endif
    if(!fin) {
      if(MainOrder == 'g') {
        Forward(100);
        if(digitalRead(interruptPin) == LOW) {
          Rotate(-90);
          Forward(900);
        }
      }
      else if(MainOrder == 'd') {
        Forward(100);
        if(digitalRead(interruptPin) == LOW) {
          Rotate(90);
          Forward(900);
        }
      }
      fin = true;
    }
  }  // Fin serial
} // Fin loop


void Run_PAMI2() {
  MsTimer2::stop();
#if DEBUG
  Serial.println("Stop_Run_PAMI2!");
#endif
  Stop_PAMI2();
}

// Send message to Master with steps done for each steppers
void sendSteppersPositions() {
  Serial.print(Right_stepper.currentPosition());
  Serial.print("/");
  Serial.println(Left_stepper.currentPosition());
}

void Forward(long dist) {
  long steps = (dist * 1600 / 220);  // RÃ©glage sur Drivers au micropas au 1/8
  digitalWrite(MR_EN_PIN, LOW);
  digitalWrite(ML_EN_PIN, LOW);
#if DEBUG
  Serial.print("Move Forward: ");
  Serial.print(dist);
  Serial.println("mm");
#endif
  Right_stepper.move(-steps);
  Left_stepper.move(steps);

  do {
    if (digitalRead(interruptPin) == LOW) {
      Right_stepper.run();
      Left_stepper.run();
    }
  } while (!(Right_stepper.distanceToGo() == 0 && Left_stepper.distanceToGo() == 0) && digitalRead(interruptPin) == LOW);
  if (!(Right_stepper.distanceToGo() == 0 && Left_stepper.distanceToGo() == 0)) {
    delay(1000);
    Right_stepper.move(0);
    Left_stepper.move(0);
    digitalWrite(MR_EN_PIN, HIGH);
    digitalWrite(ML_EN_PIN, HIGH);
    if (digitalRead(interruptPin) == LOW) {
      long distance = Left_stepper.distanceToGo() * 220 / 1600;
      Serial.print("To go 1: ");
      Serial.print(distance);
      Serial.println("mm");
      //Forward(distance);
    }
    else{
      Stop_PAMI2();
      return;
    //   long distance = (steps-Left_stepper.distanceToGo())*220 /1600;
    //   Serial.print("To go 2: ");
    //   Serial.print(distance);
    //   Serial.println("mm");
    //   Rotate(180);
    //   Forward(distance);
    }
  }
#if DEBUG
  Serial.println("Distance Reached ! : ");
#endif
  sendSteppersPositions();
  MainOrder = '*';
  digitalWrite(MR_EN_PIN, HIGH);
  digitalWrite(ML_EN_PIN, HIGH);
}

void Backward(long dist) {
  long steps = (dist * 1600 / 220);  // RÃ©glage sur Drivers au micropas au 1/8
  digitalWrite(MR_EN_PIN, LOW);
  digitalWrite(ML_EN_PIN, LOW);
#if DEBUG
  Serial.print("Move Backward: ");
  Serial.print(dist);
  Serial.println("mm");
#endif
  Right_stepper.move(steps);
  Left_stepper.move(-steps);
  do {
    if (digitalRead(interruptPin) == LOW) {
      Right_stepper.run();
      Left_stepper.run();
    }
  } while (!(Right_stepper.distanceToGo() == 0 && Left_stepper.distanceToGo() == 0));
#if DEBUG
  Serial.println("Distance Reached ! : ");
#endif
  sendSteppersPositions();
  MainOrder = '*';
  digitalWrite(MR_EN_PIN, HIGH);
  digitalWrite(ML_EN_PIN, HIGH);
}

void Rotate(int deg) {
  digitalWrite(MR_EN_PIN, LOW);
  digitalWrite(ML_EN_PIN, LOW);
  // Order to Rotate
  long angle_steps = 8.75 * deg;

#if DEBUG
  Serial.print("angle_steps :");
  Serial.println(angle_steps);
#endif
  Right_stepper.move(angle_steps);
  Left_stepper.move(angle_steps);
  do {
    Right_stepper.run();
    Left_stepper.run();
  } while (!(Right_stepper.distanceToGo() == 0 && Left_stepper.distanceToGo() == 0));
#if DEBUG
  Serial.println("Angle Reached ! : ");
#endif
  sendSteppersPositions();
  MainOrder = '*';
  digitalWrite(MR_EN_PIN, HIGH);
  digitalWrite(ML_EN_PIN, HIGH);
}

void RotateLeft(int deg) {
  digitalWrite(MR_EN_PIN, LOW);
  digitalWrite(ML_EN_PIN, LOW);
  // Order to Rotate
  long angle_steps = 8.75 * deg;

#if DEBUG
  Serial.print("angle_steps :");
  Serial.println(angle_steps);
#endif
  Right_stepper.move(-angle_steps*2);
  do {
    Right_stepper.run();
  } while (!(Right_stepper.distanceToGo() == 0 && Left_stepper.distanceToGo() == 0));
#if DEBUG
  Serial.println("Angle Reached ! : ");
#endif
  sendSteppersPositions();
  MainOrder = '*';
  digitalWrite(MR_EN_PIN, HIGH);
  digitalWrite(ML_EN_PIN, HIGH);
}

void RotateRight(int deg) {
  digitalWrite(MR_EN_PIN, LOW);
  digitalWrite(ML_EN_PIN, LOW);
  // Order to Rotate
  long angle_steps = 8.75 * deg;

#if DEBUG
  Serial.print("angle_steps :");
  Serial.println(angle_steps);
#endif
  Left_stepper.move(angle_steps*2);
  do {
    Left_stepper.run();
  } while (!(Right_stepper.distanceToGo() == 0 && Left_stepper.distanceToGo() == 0));
#if DEBUG
  Serial.println("Angle Reached ! : ");
#endif
  sendSteppersPositions();
  MainOrder = '*';
  digitalWrite(MR_EN_PIN, HIGH);
  digitalWrite(ML_EN_PIN, HIGH);
}

void Stop_PAMI2() {
#if DEBUG
  Serial.println("PAMI3 Stopped");
#endif
  Right_stepper.stop();
  Left_stepper.stop();
  sendSteppersPositions();
  resetSteppersPositions();
  delay(20);
  digitalWrite(MR_EN_PIN, HIGH);
  digitalWrite(ML_EN_PIN, HIGH);
}
void disable_en_motion_stepper() {
  // Motion Right_STEPPER and Left_STEPPER OFF(FREE)
  digitalWrite(MR_EN_PIN, HIGH);
  digitalWrite(ML_EN_PIN, HIGH);
}

// Reset steppers steps counters
void resetSteppersPositions() {
  Right_stepper.setCurrentPosition(0);
  Left_stepper.setCurrentPosition(0);
}
