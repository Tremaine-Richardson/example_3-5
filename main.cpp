//=====[Libraries]=============================================================

#include "arm_book_lib.h"
#include "mbed.h"

//=====[Defines]===============================================================

#define NUMBER_OF_SENSORS 4

//=====[Declaration and initialization of public global objects]===============

DigitalIn ignitionButton(BUTTON1);
DigitalIn passengerSeat(D2);
DigitalIn passengerBelt(D3);
DigitalIn driverSeat(D4);
DigitalIn driverBelt(D5);

DigitalOut canStartLED(LED1);
DigitalOut ignitionLED(LED2);

DigitalInOut sirenPin(PE_10);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

//=====[Declaration and initialization of public global variables]=============

bool canStartState = OFF;
bool ignitionPushed = OFF;
bool prevDriverSeat = OFF;

int allOn[NUMBER_OF_SENSORS] = {1, 1, 1, 1};
int sensorsOn[NUMBER_OF_SENSORS] = {0, 0, 0, 0};

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit();
void outputsInit();

void sensorUpdate();
void ignitionActivated();

void uartEnd();

bool areEqual();

//=====[Main function, the program entry point after power on or reset]========

int main() {
  inputsInit();
  outputsInit();
  while (ignitionPushed == OFF) {
    sensorUpdate();
    ignitionActivated();
  }
}

//=====[Implementations of public functions]===================================

void inputsInit() {
  ignitionButton.mode(PullDown);
  passengerSeat.mode(PullDown);
  passengerBelt.mode(PullDown);
  driverSeat.mode(PullDown);
  driverBelt.mode(PullDown);
  sirenPin.mode(OpenDrain);
  sirenPin.input();
}

void outputsInit() {
  canStartLED = OFF;
  ignitionLED = OFF;
}

void sensorUpdate() {
  sensorsOn[0] = passengerSeat;
  sensorsOn[1] = passengerBelt;
  sensorsOn[2] = driverSeat;
  sensorsOn[3] = driverBelt;
  if ( prevDriverSeat == OFF) {
      if ( driverSeat ) {
            uartUsb.write( "Welcome to enhanced alarm system model 218-W24\r\n", 48);
            prevDriverSeat = !prevDriverSeat;
      }
  }
  else {
      if ( !driverSeat ) {
          prevDriverSeat = !prevDriverSeat;
      }
  }
  if (areEqual()) {
    canStartState = ON;
  } else {
    canStartState = OFF;
  }
  canStartLED = canStartState;
}

void ignitionActivated() {
  if (ignitionButton) {
    if (canStartState) {
      ignitionLED = ON;
      canStartLED = OFF;
    } else {
      sirenPin.output();
      sirenPin = LOW;
    }
    uartEnd();
    ignitionPushed = ON;
  }
}

void uartEnd() {
  if (canStartState) {
    uartUsb.write("Engine Started\r\n", 16);
  } else {
    uartUsb.write("Ignition inhibited for the following reasons:\r\n", 47);
    if (sensorsOn[0] == 0) {
      uartUsb.write("Passenger seat not occupied\r\n", 29);
    }
    if (sensorsOn[1] == 0) {
      uartUsb.write("Passenger seatbelt not fastened\r\n", 33);
    }
    if (sensorsOn[2] == 0) {
      uartUsb.write("Driver seat not occupied\r\n", 26);
    }
    if (sensorsOn[3] == 0) {
      uartUsb.write("Driver seatbelt not fastened\r\n", 30);
    }
  }
}

bool areEqual() {
  int i;

  for (i = 0; i < NUMBER_OF_SENSORS; i++) {
    if (allOn[i] != sensorsOn[i]) {
      return false;
    }
  }

  return true;
}
