#include "functions.h"

unsigned long startTime;
unsigned long startTime2;
unsigned long interval = 1500; // interval in milliseconds
unsigned long interval2 = 500;
const int batGood = 1;
const int batCharge = 2;
const int batWaste = 3;
const int arraySize = 5;
const int lastIndex = 4;
const double correction = 0.2;
int batteries[] = {4, 4, 4, 4, 4};

double VGOOD = 3.90;
double VCHARGE = 3.40;
double VWASTE = 3.30;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2, OUTPUT); // solenoid GOOD
  pinMode(3, OUTPUT); // solenoid CHARGE
  pinMode(5, INPUT); // inductiesensor GOOD
  pinMode(7, OUTPUT); // motor
  pinMode(9, INPUT); // check for battery
  pinMode(10, INPUT); // check for conatiners, microswitch NO
  pinMode(A4, INPUT); // measuring voltage

  startTime = millis();
  startTime2 = millis();
}

void loop() {

  //variables to set boundries sorting voltage, adjustable by user


  int state = 1;
  //  const variables to set in array to be able to switch through cases

  digitalWrite(7, HIGH); // turning on motor
  while (true) { // checking if all containers are in place

    double voltage = measuringVoltage();
    voltage = voltage - correction;
    if (voltage < 4.30 && (millis() - startTime >= interval)) {
      startTime = millis();
      Serial.println(voltage);
      if (voltage >= VGOOD) {
        for (int i = 0; i < arraySize; i++) {
          if (batteries[i] == 4) {
            batteries[i] = batGood;
            i = arraySize;
//            Serial.println("good");
          }
        } //writing value of true to the first index with value false
      } else if (voltage >= VCHARGE && voltage < VGOOD) {
        for (int i = 0; i < arraySize; i++) {
          if (batteries[i] == 4) {
            batteries[i] = batCharge;
            i = arraySize;
          }
        }
      } else if (voltage <= VWASTE) {
        for (int i = 0; i < arraySize; i++) {
          if (batteries[i] == 4) {
            batteries[i] = batWaste;
            i = arraySize;
          }
        }
      }

      if (!digitalRead(5) && (millis() - startTime2 >= interval2)) { //inductiesensor (millis() - Time0 > 500 )
        Serial.println("gezien");
        startTime2 = millis();
        state = batteries[0];
        switch (state) {
          case 1:
            digitalWrite(2, LOW); // opening hole for good batteries (switching off solenoid)
            digitalWrite(3, LOW); // opening hole for charge batteries (switching off solenoid)
            for (int i = 0; i < lastIndex; i++) {
              batteries[i] = batteries[i + 1];
            }
            batteries[lastIndex] = 4;
            break;
          case 2:
            digitalWrite(2, HIGH); // closing hole for good batteries, switching solenoid on , so batteries can pass
            digitalWrite(3, LOW); // opening hole for charge batteries, switching off solenoid
            for (int i = 0; i < lastIndex; i++) {
              batteries[i] = batteries[i + 1];
            }
            batteries[lastIndex] = 4;
            break;
          case 3:
            digitalWrite(2, HIGH); // closing hole for good batteries, switching solenoid on so batteries can pass
            digitalWrite(3, HIGH); // closing hole for charge batteries, switching solenoid on so batteries can pass
            for (int i = 0; i < lastIndex; i++) {
              batteries[i] = batteries[i + 1];
            }
            batteries[lastIndex] = 4;
            break;
          case 4:
            digitalWrite(2, HIGH); // opening hole for good batteries (switching off solenoid)
            digitalWrite(3, HIGH); // opening hole for charge batteries (switching off solenoid)
            break;
        }
      }
    }
  }
  //  Serial.println("Missing containers"); error indicating containers are missing
}
