#include <Bluepad32.h>

const uint8_t targetMacAddress[] = {0xAC, 0xD7, 0xFB, 0x3F, 0x00, 0x00};
ControllerPtr myControllers[BP32_MAX_GAMEPADS];
ControllerPtr connectedController = nullptr;

// Variabel untuk pola gerakan otomatis
bool isPatternRunning = false;
uint8_t currentStep = 0;
unsigned long lastTime = 0;

// Variabel untuk pola gerakan kedua
bool isPattern2Running = false;
uint8_t currentStep2 = 0;
unsigned long lastTime2 = 0;

// Definisi pin motor driver BTS7960
#define LEFT_MOTOR_RIS 13   // Right Input Signal Motor Kiri
#define LEFT_MOTOR_LIS 12   // Left Input Signal Motor Kiri
#define LEFT_MOTOR_REN 14   // Enable Right Motor Kiri
#define LEFT_MOTOR_LEN 27   // Enable Left Motor Kiri
#define LEFT_MOTOR_RPWM 26  // PWM Right Motor Kiri 
#define LEFT_MOTOR_LPWM 25  // PWM Left Motor Kiri

#define RIGHT_MOTOR_RIS 15  // Right Input Signal Motor Kanan
#define RIGHT_MOTOR_LIS 2  // Left Input Signal Motor Kanan
#define RIGHT_MOTOR_REN 4  // Enable Right Motor Kanan
#define RIGHT_MOTOR_LEN 16  // Enable Left Motor Kanan
#define RIGHT_MOTOR_RPWM 17 // PWM Right Motor Kanan
#define RIGHT_MOTOR_LPWM 5 // PWM Left Motor Kanan

// Fungsi untuk menghentikan semua motor
void stopMotors() {
    analogWrite(LEFT_MOTOR_RPWM, 0);
    analogWrite(LEFT_MOTOR_LPWM, 0);
    analogWrite(RIGHT_MOTOR_RPWM, 0);
    analogWrite(RIGHT_MOTOR_LPWM, 0);
}

// Fungsi untuk menggerakkan motor maju dengan kecepatan tertentu
void moveForward(uint8_t speed = 255) {
    digitalWrite(LEFT_MOTOR_RIS, LOW);   
    digitalWrite(LEFT_MOTOR_LIS, LOW);   
    analogWrite(LEFT_MOTOR_RPWM, speed); 
    analogWrite(LEFT_MOTOR_LPWM, 0);     

    digitalWrite(RIGHT_MOTOR_RIS, LOW);  
    digitalWrite(RIGHT_MOTOR_LIS, LOW);  
    analogWrite(RIGHT_MOTOR_RPWM, speed);
    analogWrite(RIGHT_MOTOR_LPWM, 0);    
}

// Fungsi untuk menggerakkan motor mundur dengan kecepatan tertentu
void moveBackward(uint8_t speed = 255) {
    digitalWrite(LEFT_MOTOR_RIS, LOW);   
    digitalWrite(LEFT_MOTOR_LIS, LOW);   
    analogWrite(LEFT_MOTOR_RPWM, 0);     
    analogWrite(LEFT_MOTOR_LPWM, speed); 

    digitalWrite(RIGHT_MOTOR_RIS, LOW);  
    digitalWrite(RIGHT_MOTOR_LIS, LOW);  
    analogWrite(RIGHT_MOTOR_RPWM, 0);    
    analogWrite(RIGHT_MOTOR_LPWM, speed);
}

// Fungsi untuk menggerakkan motor belok kiri
void turnLeft() {
    digitalWrite(LEFT_MOTOR_RIS, LOW);
    digitalWrite(LEFT_MOTOR_LIS, LOW);
    analogWrite(LEFT_MOTOR_RPWM, 0);
    analogWrite(LEFT_MOTOR_LPWM, 128);

    digitalWrite(RIGHT_MOTOR_RIS, LOW);
    digitalWrite(RIGHT_MOTOR_LIS, LOW);
    analogWrite(RIGHT_MOTOR_RPWM, 128);
    analogWrite(RIGHT_MOTOR_LPWM, 0);
}

// Fungsi untuk menggerakkan motor belok kanan
void turnRight() {
    digitalWrite(LEFT_MOTOR_RIS, LOW);
    digitalWrite(LEFT_MOTOR_LIS, LOW);
    analogWrite(LEFT_MOTOR_RPWM, 128);
    analogWrite(LEFT_MOTOR_LPWM, 0);

    digitalWrite(RIGHT_MOTOR_RIS, LOW);
    digitalWrite(RIGHT_MOTOR_LIS, LOW);
    analogWrite(RIGHT_MOTOR_RPWM, 0);
    analogWrite(RIGHT_MOTOR_LPWM, 128);
}

// Fungsi untuk menggerakkan motor dengan kecepatan 20%
void moveForwardVerySlow() {
    digitalWrite(LEFT_MOTOR_RIS, LOW);   
    digitalWrite(LEFT_MOTOR_LIS, LOW);   
    analogWrite(LEFT_MOTOR_RPWM, 51); 
    analogWrite(LEFT_MOTOR_LPWM, 0);     

    digitalWrite(RIGHT_MOTOR_RIS, LOW);  
    digitalWrite(RIGHT_MOTOR_LIS, LOW);  
    analogWrite(RIGHT_MOTOR_RPWM, 51);
    analogWrite(RIGHT_MOTOR_LPWM, 0);    
}

// Fungsi untuk menggerakkan motor mundur dengan kecepatan 20%
void moveBackwardVerySlow() {
    digitalWrite(LEFT_MOTOR_RIS, LOW);   
    digitalWrite(LEFT_MOTOR_LIS, LOW);   
    analogWrite(LEFT_MOTOR_RPWM, 0);     
    analogWrite(LEFT_MOTOR_LPWM, 51); 

    digitalWrite(RIGHT_MOTOR_RIS, LOW);  
    digitalWrite(RIGHT_MOTOR_LIS, LOW);  
    analogWrite(RIGHT_MOTOR_RPWM, 0);    
    analogWrite(RIGHT_MOTOR_LPWM, 51);
}

// Fungsi untuk menggerakkan motor belok kiri dengan kecepatan sangat lambat
void turnLeftVerySlow() {
    digitalWrite(LEFT_MOTOR_RIS, LOW);
    digitalWrite(LEFT_MOTOR_LIS, LOW);
    analogWrite(LEFT_MOTOR_RPWM, 0);
    analogWrite(LEFT_MOTOR_LPWM, 51);

    digitalWrite(RIGHT_MOTOR_RIS, LOW);
    digitalWrite(RIGHT_MOTOR_LIS, LOW);
    analogWrite(RIGHT_MOTOR_RPWM, 51);
    analogWrite(RIGHT_MOTOR_LPWM, 0);
}

// Fungsi untuk menggerakkan motor belok kanan dengan kecepatan sangat lambat
void turnRightVerySlow() {
    digitalWrite(LEFT_MOTOR_RIS, LOW);
    digitalWrite(LEFT_MOTOR_LIS, LOW);
    analogWrite(LEFT_MOTOR_RPWM, 51);
    analogWrite(LEFT_MOTOR_LPWM, 0);

    digitalWrite(RIGHT_MOTOR_RIS, LOW);
    digitalWrite(RIGHT_MOTOR_LIS, LOW);
    analogWrite(RIGHT_MOTOR_RPWM, 0);
    analogWrite(RIGHT_MOTOR_LPWM, 51);
}

// Fungsi untuk menampilkan debug
void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
        ctl->index(),          // Controller Index
        ctl->dpad(),           // D-pad
        ctl->buttons(),        // bitmask of pressed buttons
        ctl->axisX(),          // (-511 - 512) left X Axis
        ctl->axisY(),          // (-511 - 512) left Y axis
        ctl->axisRX(),         // (-511 - 512) right X axis
        ctl->axisRY(),         // (-511 - 512) right Y axis
        ctl->brake(),          // (0 - 1023): brake button
        ctl->throttle(),       // (0 - 1023): throttle (AKA gas) button
        ctl->miscButtons(),    // bitmask of pressed "misc" buttons
        ctl->gyroX(),          // Gyro X
        ctl->gyroY(),          // Gyro Y
        ctl->gyroZ(),          // Gyro Z
        ctl->accelX(),         // Accelerometer X
        ctl->accelY(),         // Accelerometer Y
        ctl->accelZ()          // Accelerometer Z
    );
}

// Fungsi untuk menjalankan pola gerakan
void runMovementPattern() {
    if (!isPatternRunning) return;
    
    unsigned long currentTime = millis();
    
    switch(currentStep) {
        case 0: // Belok Kanan selama 3 detik
            if (currentTime - lastTime <= 100) {
                turnRight();
            } else {
                currentStep++;
                lastTime = currentTime;
            }
            break;
            
        case 1: // Maju selama 2.5 detik
            if (currentTime - lastTime <= 650) {
                moveForward(75);
            } else {
                currentStep++;
                lastTime = currentTime;
            }
            break;
            
        case 2: // Belok Kiri selama 3 detik
            if (currentTime - lastTime <= 190) {
                turnLeft();
            } else {
                currentStep++;
                lastTime = currentTime;
            }
            break;
            
        case 3: // Maju dengan kecepatan 75% selama 2 detik
            if (currentTime - lastTime <= 690) {
                moveForward(100);
            } else {
                currentStep++;
                lastTime = currentTime;
            }
            break;
            
        case 4: // Belok Kiri selama 3 detik
            if (currentTime - lastTime <= 200) {
                turnLeft();
            } else {
                currentStep++;
                lastTime = currentTime;
            }
            break;
            
        case 5: // Maju dengan kecepatan 75% selama 2 detik
            if (currentTime - lastTime <= 900) {
                moveForward(100);
            } else {
                currentStep++;
                lastTime = currentTime;
            }
            break;
            
        case 6: // Selesai pola
            stopMotors();
            Serial.println("Pola gerakan selesai");
            isPatternRunning = false;
            currentStep = 0;
            break;
    }
}

// Fungsi untuk menjalankan pola gerakan kedua (mirror)
void runMovementPattern2() {
    if (!isPattern2Running) return;
    
    unsigned long currentTime = millis();
    
    switch(currentStep2) {
        case 0: // Belok Kiri selama 3 detik
            if (currentTime - lastTime2 <= 100) {
                turnLeft();
            } else {
                currentStep2++;
                lastTime2 = currentTime;
            }
            break;
            
        case 1: // Maju selama 2.5 detik
            if (currentTime - lastTime2 <= 650) {
                moveForward(75);
            } else {
                currentStep2++;
                lastTime2 = currentTime;
            }
            break;
            
        case 2: // Belok Kanan selama 3 detik
            if (currentTime - lastTime2 <= 190) {
                turnRight();
            } else {
                currentStep2++;
                lastTime2 = currentTime;
            }
            break;
            
        case 3: // Maju dengan kecepatan 75% selama 2 detik
            if (currentTime - lastTime2 <= 690) {
                moveForward(100);
            } else {
                currentStep2++;
                lastTime2 = currentTime;
            }
            break;
            
        case 4: // Belok Kanan selama 3 detik
            if (currentTime - lastTime2 <= 200) {
                turnRight();
            } else {
                currentStep2++;
                lastTime2 = currentTime;
            }
            break;
            
        case 5: // Maju dengan kecepatan 75% selama 2 detik
            if (currentTime - lastTime2 <= 900) {
                moveForward(100);
            } else {
                currentStep2++;
                lastTime2 = currentTime;
            }
            break;
            
        case 6: // Selesai pola
            stopMotors();
            Serial.println("Pola gerakan kedua selesai");
            isPattern2Running = false;
            currentStep2 = 0;
            break;
    }
}

// Fungsi untuk memproses controller
void processGamepad(ControllerPtr ctl) {
    uint8_t dpad = ctl->dpad();
    uint16_t buttons = ctl->buttons();
    stopMotors();  
    
    // === Kontrol Motor ===
    if (dpad == 0x01) {  // Atas
        if (buttons & 0x0004) {  // Jika tombol rectangle ditekan
            moveForwardVerySlow();  // Mode sangat lambat maju (20%)
            Serial.println("Mode Sangat Lambat Maju Aktif");
        } else if (buttons & 0x0001) {  // Jika tombol cross ditekan
            moveForward(255);  // Mode turbo maju (100%)
            Serial.println("Mode Turbo Maju Aktif");
        } else {
            moveForward(200);  // Mode normal maju (78%)
            Serial.println("Mode Normal Maju Aktif");
        }
    } else if (dpad == 0x02) {  // Bawah
        if (buttons & 0x0004) {  // Jika tombol rectangle ditekan
            moveBackwardVerySlow();  // Mode sangat lambat mundur (20%)
            Serial.println("Mode Sangat Lambat Mundur Aktif");
        } else if (buttons & 0x0001) {  // Jika tombol cross ditekan
            moveBackward(255);  // Mode turbo mundur (100%)
            Serial.println("Mode Turbo Mundur Aktif");
        } else {
            moveBackward(200);  // Mode normal mundur (78%)
            Serial.println("Mode Normal Mundur Aktif");
        }
    } else if (dpad == 0x08) {  // Kiri
        if (buttons & 0x0004) {  // Jika tombol rectangle ditekan
            turnLeftVerySlow();  // Mode sangat lambat belok kiri (12.5%)
            Serial.println("Mode Sangat Lambat Belok Kiri Aktif");
        } else {
            turnLeft();  // Mode normal belok kiri
            Serial.println("Mode Normal Belok Kiri Aktif");
        }
    } else if (dpad == 0x04) {  // Kanan
        if (buttons & 0x0004) {  // Jika tombol rectangle ditekan
            turnRightVerySlow();  // Mode sangat lambat belok kanan (12.5%)
            Serial.println("Mode Sangat Lambat Belok Kanan Aktif");
        } else {
            turnRight();  // Mode normal belok kanan
            Serial.println("Mode Normal Belok Kanan Aktif");
        }
    } else {
        // Periksa kombinasi R1 + Circle untuk memulai pola pertama
        if ((buttons & 0x0020) && (buttons & 0x0002)) {
            if (!isPatternRunning) {
                isPatternRunning = true;
                currentStep = 0;
                lastTime = millis();  // Mulai penghitungan waktu saat R1 + Circle ditekan
                Serial.println("Memulai pola gerakan otomatis...");
            }
        } 
        // Periksa kombinasi L1 + circle untuk memulai pola kedua
        else if ((buttons & 0x0010) && (buttons & 0x0002)) {
            if (!isPattern2Running) {
                isPattern2Running = true;
                currentStep2 = 0;
                lastTime2 = millis();  // Mulai penghitungan waktu saat L1 + Square ditekan
                Serial.println("Memulai pola gerakan kedua otomatis...");
            }
        } 
        // Periksa tombol segitiga untuk cancel pattern
        else if (buttons & 0x0008) {
            if (isPatternRunning || isPattern2Running) {
                stopMotors();
                isPatternRunning = false;
                isPattern2Running = false;
                currentStep = 0;
                currentStep2 = 0;
                Serial.println("Pattern dibatalkan!");
            }
        } else {
            stopMotors();          
            Serial.println("Semua Motor Berhenti");
        }
    }
}

// Fungsi untuk memproses controller
void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

// Fungsi untuk menangani koneksi controller
void onConnectedController(ControllerPtr ctl) {
    const uint8_t* controllerMac = ctl->getProperties().btaddr;
    
    bool isAuthorized = true;
    for (int i = 0; i < 6; i++) {
        if (controllerMac[i] != targetMacAddress[i]) {
            isAuthorized = false;
            break;
        }
    }
    
    if (!isAuthorized) {
        Serial.println("WARNING: Unauthorized controller trying to connect!");
        Serial.printf("Controller MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
            controllerMac[0], controllerMac[1], controllerMac[2],
            controllerMac[3], controllerMac[4], controllerMac[5]);
        Serial.printf("Expected MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
            targetMacAddress[0], targetMacAddress[1], targetMacAddress[2],
            targetMacAddress[3], targetMacAddress[4], targetMacAddress[5]);
        ctl->disconnect();
        return;
    }

    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            myControllers[i] = ctl;
            connectedController = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not find empty slot");
    }
}

// Fungsi untuk menangani disconnect controller
void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            if (connectedController == ctl) {
                connectedController = nullptr;
                Serial.println("INFO: Connected controller has been reset");
            }
            break;
        }
    }
    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

// Fungsi untuk menginisialisasi semua motor dalam keadaan mati
void initializeMotors() {
    pinMode(LEFT_MOTOR_RIS, OUTPUT);
    pinMode(LEFT_MOTOR_LIS, OUTPUT);
    pinMode(LEFT_MOTOR_REN, OUTPUT);
    pinMode(LEFT_MOTOR_LEN, OUTPUT);
    pinMode(LEFT_MOTOR_RPWM, OUTPUT);
    pinMode(LEFT_MOTOR_LPWM, OUTPUT);

    pinMode(RIGHT_MOTOR_RIS, OUTPUT);
    pinMode(RIGHT_MOTOR_LIS, OUTPUT);
    pinMode(RIGHT_MOTOR_REN, OUTPUT);
    pinMode(RIGHT_MOTOR_LEN, OUTPUT);
    pinMode(RIGHT_MOTOR_RPWM, OUTPUT);
    pinMode(RIGHT_MOTOR_LPWM, OUTPUT);

    digitalWrite(LEFT_MOTOR_REN, HIGH);
    digitalWrite(LEFT_MOTOR_LEN, HIGH);
    digitalWrite(RIGHT_MOTOR_REN, HIGH);
    digitalWrite(RIGHT_MOTOR_LEN, HIGH);

    analogWrite(LEFT_MOTOR_RPWM, 0);
    analogWrite(LEFT_MOTOR_LPWM, 0);
    analogWrite(RIGHT_MOTOR_RPWM, 0);
    analogWrite(RIGHT_MOTOR_LPWM, 0);

    delay(50);  
    Serial.println("Semua motor telah diinisialisasi dalam keadaan MATI");
}

// Fungsi setup
void setup() {
    Serial.begin(115200);
    initializeMotors();
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.enableVirtualDevice(false);
}

// Fungsi loop utama
void loop() {
    BP32.update();
    processControllers();
    
    if (isPatternRunning) {
        runMovementPattern();
    }
    if (isPattern2Running) {
        runMovementPattern2();
    }
    
    delay(50);
}