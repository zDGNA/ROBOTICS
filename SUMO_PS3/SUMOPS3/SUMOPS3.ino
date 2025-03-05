#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEHIDDevice.h>
#include <BLE2902.h>

// Define the pins for motor control
int lpwm_1 = 27;
int rpwm_1 = 14;
int lpwm_2 = 12;
int rpwm_2 = 13;

BLEHIDDevice* hid;
BLECharacteristic* inputReport;
BLEServer* server;

void setup() {
  Serial.begin(115200);

  // Set up motor control pins
  pinMode(lpwm_1, OUTPUT);
  pinMode(rpwm_1, OUTPUT);
  pinMode(lpwm_2, OUTPUT);
  pinMode(rpwm_2, OUTPUT);

  // Create BLE HID device
  BLEDevice::init("d4:d4:da:5c:2b:6a");
  server = BLEDevice::createServer();
  hid = new BLEHIDDevice(server);
  inputReport = hid->inputReport(1);  // Use report ID 1
  hid->manufacturer()->setValue("Manufacturer");
  hid->pnp(0x01, 0xE502, 0xA111, 0x0210);

  // Start BLE
  hid->begin();
  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->setAppearance(HID_KEYBOARD);
  advertising->start();
}

void loop() {
  // Your main code goes here
  // Read PS3 controller input and update motor control accordingly

  // Example: Read analog stick values
  int yAxisValue = analogRead(A0); // Replace with actual analog pin for Y-axis
  int xAxisValue = analogRead(A1); // Replace with actual analog pin for X-axis

  // Process controller input and update motor control
  processControllerInput(yAxisValue, xAxisValue);
}

void processControllerInput(int yAxisValue, int xAxisValue) {
  // Your logic for motor control based on controller input goes here
  // Example logic:
  if (yAxisValue <= 100) {
    // Move forward
    moveForward();
  } else if (yAxisValue >= 900) {
    // Move backward
    moveBackward();
  } else if (xAxisValue <= 100) {
    // Turn left
    turnLeft();
  } else if (xAxisValue >= 900) {
    // Turn right
    turnRight();
  } else {
    // Stop
    stopMotors();
  }
}

void moveForward() {
  // Motor control logic for forward movement
  digitalWrite(rpwm_1, HIGH);
  digitalWrite(lpwm_1, LOW);
  digitalWrite(rpwm_2, HIGH);
  digitalWrite(lpwm_2, LOW);
}

void moveBackward() {
  // Motor control logic for backward movement
  digitalWrite(rpwm_1, LOW);
  digitalWrite(lpwm_1, HIGH);
  digitalWrite(rpwm_2, LOW);
  digitalWrite(lpwm_2, HIGH);
}

void turnLeft() {
  // Motor control logic for turning left
  digitalWrite(rpwm_1, HIGH);
  digitalWrite(lpwm_1, LOW);
  digitalWrite(rpwm_2, LOW);
  digitalWrite(lpwm_2, HIGH);
}

void turnRight() {
  // Motor control logic for turning right
  digitalWrite(rpwm_1, LOW);
  digitalWrite(lpwm_1, HIGH);
  digitalWrite(rpwm_2, HIGH);
  digitalWrite(lpwm_2, LOW);
}

void stopMotors() {
  // Motor control logic for stopping
  digitalWrite(rpwm_1, LOW);
  digitalWrite(lpwm_1, LOW);
  digitalWrite(rpwm_2, LOW);
  digitalWrite(lpwm_2, LOW);
}
