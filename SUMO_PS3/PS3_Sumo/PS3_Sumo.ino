#include <Ps3Controller.h>

// Pin untuk motor driver pertama (kiri)
const int LPWMPin1 = 13; // PWM pada ESP32 untuk kecepatan motor kiri
const int L_ENPin1 = 14; // GPIO pada ESP32 untuk mengaktifkan motor kiri
const int RPWMPin1 = 12;  // PWM pada ESP32 untuk kecepatan motor kiri
const int R_ENPin1 = 14; // GPIO pada ESP32 untuk mengaktifkan motor kiri

// Pin untuk motor driver kedua (kanan)
const int LPWMPin2 = 4; // PWM pada ESP32 untuk kecepatan motor kanan
const int L_ENPin2 = 0; // GPIO pada ESP32 untuk mengaktifkan motor kanan
const int RPWMPin2 = 2;  // PWM pada ESP32 untuk kecepatan motor kanan
const int R_ENPin2 = 15; // GPIO pada ESP32 untuk mengaktifkan motor kanan

void setup() {
  Serial.begin(115200);

  // Setup pin untuk motor driver
  pinMode(LPWMPin1, OUTPUT);
  pinMode(L_ENPin1, OUTPUT);
  pinMode(RPWMPin1, OUTPUT);
  pinMode(R_ENPin1, OUTPUT);
  pinMode(LPWMPin2, OUTPUT);
  pinMode(L_ENPin2, OUTPUT);
  pinMode(RPWMPin2, OUTPUT);
  pinMode(R_ENPin2, OUTPUT);

  // Setup PS3 controller
  Ps3.begin("d4:d4:da:5c:2b:6a");  // Ganti dengan alamat Bluetooth stik PS3 kamu
}

void loop() {
  if (!Ps3.isConnected()) {
    // Stik PS3 tidak terhubung, berhenti robot
    stopRobot();
    Serial.println("Stik PS3 tidak terhubung.");
    return;
  }

  // Baca nilai tombol arah
  bool upButton = Ps3.data.button.up;
  bool downButton = Ps3.data.button.down;
  bool leftButton = Ps3.data.button.left;
  bool rightButton = Ps3.data.button.right;

  // Kendalikan motor berdasarkan tombol arah stik PS3
  if (upButton) {
    // Maju
    moveForward(255, 0);  // Kecepatan maksimum
    Serial.println("Maju");
  } else if (downButton) {
    // Mundur
    moveBackward(0, 255);  // Kecepatan maksimum
    Serial.println("Mundur");
  } else if (leftButton) {
    // Belok kiri
    turnLeft(255, 0);  // Kecepatan maksimum
    Serial.println("Belok Kiri");
  } else if (rightButton) {
    // Belok kanan
    turnRight(0, 255);  // Kecepatan maksimum
    Serial.println("Belok Kanan");
  } else {
    // Berhenti jika tidak ada tombol arah ditekan
    stopRobot();
    Serial.println("Berhenti");
  }

  // Tambahkan delay untuk memberikan waktu bagi stik PS3 untuk memproses input
  delay(20);
}

void moveForward(int speedLeft, int speedRight) {
  analogWrite(LPWMPin1, abs(speedLeft));
  analogWrite(RPWMPin1, abs(speedRight));
  
  analogWrite(LPWMPin2, abs(speedLeft));
  analogWrite(RPWMPin2, abs(speedRight));
  
  digitalWrite(L_ENPin1, HIGH);
  digitalWrite(R_ENPin1, HIGH);
  
  digitalWrite(L_ENPin2, HIGH);
  digitalWrite(R_ENPin2, HIGH);
}

void moveBackward(int speedLeft, int speedRight) {
  analogWrite(LPWMPin1, abs(speedLeft));
  analogWrite(RPWMPin1, abs(speedRight));
  
  analogWrite(LPWMPin2, abs(speedLeft));
  analogWrite(RPWMPin2, abs(speedRight));
  
  digitalWrite(L_ENPin1, HIGH);
  digitalWrite(R_ENPin1, HIGH);
  
  digitalWrite(L_ENPin2, HIGH);
  digitalWrite(R_ENPin2, HIGH);
}

void turnLeft(int speedLeft, int speedRight) {
  analogWrite(LPWMPin1, abs(speedLeft));
  analogWrite(RPWMPin1, abs(speedRight));
  
  analogWrite(LPWMPin2, abs(speedLeft));
  analogWrite(RPWMPin2, abs(speedRight));
  
  digitalWrite(L_ENPin1, LOW);
  digitalWrite(R_ENPin1, LOW);
  
  digitalWrite(L_ENPin2, HIGH);
  digitalWrite(R_ENPin2, HIGH);
}

void turnRight(int speedLeft, int speedRight) {
  analogWrite(LPWMPin1, abs(speedRight));
  analogWrite(RPWMPin1, abs(speedLeft));
  
  analogWrite(LPWMPin2, abs(speedLeft));
  analogWrite(RPWMPin2, abs(speedRight));
  
  digitalWrite(L_ENPin1, HIGH);
  digitalWrite(R_ENPin1, HIGH);
  
  digitalWrite(L_ENPin2, LOW);
  digitalWrite(R_ENPin2, LOW);
}

void stopRobot() {
  analogWrite(LPWMPin1, 0);
  analogWrite(RPWMPin1, 0);
  analogWrite(LPWMPin2, 0);
  analogWrite(RPWMPin2, 0);
  
  digitalWrite(L_ENPin1, LOW);
  digitalWrite(R_ENPin1, LOW);
  
  digitalWrite(L_ENPin2, LOW);
  digitalWrite(R_ENPin2, LOW);
}