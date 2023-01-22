#include "functions.h"

unsigned long startTimeMeasurement;
unsigned long startTimeSensor;
unsigned long measureInterval = 1500; // interval in milliseconds to measure the voltage every 1500ms
unsigned long sensorInterval = 500; // interval to wait for the battery to pass before reading sensorvalue again

const int batGood = 1;
const int batCharge = 2;
const int batWaste = 3;
const int lastIndex = 4;
const int arraySize = 5;

const double correction = 0.2; // correction for the measured voltage to get the accurate voltage of the battery
int batteries[] = {4, 4, 4, 4, 4};

// variables to set the boundaries for all containers
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

  startTimeMeasurement = millis();
  startTimeSensor = millis();
}

void loop() {

  //variables to set boundries sorting voltage, adjustable by user


  int state = 1;
  //  const variables to set in array to be able to switch through cases

  digitalWrite(7, HIGH); // turning on motor
  while (true) { // checking if all containers are in place

    double voltage = measuringVoltage();
    voltage = voltage - correction;
    if (voltage < 4.30 && (millis() - startTimeMeasurement >= measureInterval)) { // checking if there is a battery and the timer has past the interval to make sure one measurement is done
      startTimeMeasurement = millis();  
      if (voltage >= VGOOD) {
        for (int i = 0; i < arraySize; i++) { //adding good battery to array
          if (batteries[i] == 4) {
            batteries[i] = batGood;
            i = arraySize;
          }
        } //writing value of true to the first index with value false
      } else if (voltage >= VCHARGE && voltage < VGOOD) {
        for (int i = 0; i < arraySize; i++) {
          if (batteries[i] == 4) { // adding charge battery to array
            batteries[i] = batCharge;
            i = arraySize;
          }
        }
      } else if (voltage <= VWASTE) {
        for (int i = 0; i < arraySize; i++) {
          if (batteries[i] == 4) { // adding waste battery to array
            batteries[i] = batWaste;
            i = arraySize;
          }
        }
      }

      if (!digitalRead(5) && (millis() - startTimeSensor >= sensorInterval)) {
        startTimeSensor = millis();
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
            digitalWrite(2, LOW); // opening hole for good batteries (switching off solenoid)
            digitalWrite(3, LOW); // opening hole for charge batteries (switching off solenoid)
            break;
        }
      }
    }
  }
  //  Serial.println("Missing containers"); error indicating containers are missing
}
