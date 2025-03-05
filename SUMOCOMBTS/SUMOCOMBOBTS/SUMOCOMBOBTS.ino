#include <Bluepad32.h>

const uint8_t targetMacAddress[] = {0xC8, 0xD9, 0xFB, 0x3F, 0x00, 0x00};
ControllerPtr myControllers[BP32_MAX_GAMEPADS];
ControllerPtr connectedController = nullptr;

// Definisi pin motor driver BTS7960
#define LEFT_MOTOR_RIS 13   // Right Input Signal Motor Kiri
#define LEFT_MOTOR_LIS 12   // Left Input Signal Motor Kiri
#define LEFT_MOTOR_REN 14   // Enable Right Motor Kiri
#define LEFT_MOTOR_LEN 27   // Enable Left Motor Kiri
#define LEFT_MOTOR_RPWM 15  // PWM Right Motor Kiri
#define LEFT_MOTOR_LPWM 16  // PWM Left Motor Kiri

#define RIGHT_MOTOR_RIS 17  // Right Input Signal Motor Kanan
#define RIGHT_MOTOR_LIS 18  // Left Input Signal Motor Kanan
#define RIGHT_MOTOR_REN 19  // Enable Right Motor Kanan
#define RIGHT_MOTOR_LEN 21  // Enable Left Motor Kanan
#define RIGHT_MOTOR_RPWM 22 // PWM Right Motor Kanan
#define RIGHT_MOTOR_LPWM 23 // PWM Left Motor Kanan

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
    analogWrite(LEFT_MOTOR_LPWM, 200);

    digitalWrite(RIGHT_MOTOR_RIS, LOW);
    digitalWrite(RIGHT_MOTOR_LIS, LOW);
    analogWrite(RIGHT_MOTOR_RPWM, 200);
    analogWrite(RIGHT_MOTOR_LPWM, 0);
}

// Fungsi untuk menggerakkan motor belok kanan
void turnRight() {
    digitalWrite(LEFT_MOTOR_RIS, LOW);
    digitalWrite(LEFT_MOTOR_LIS, LOW);
    analogWrite(LEFT_MOTOR_RPWM, 200);
    analogWrite(LEFT_MOTOR_LPWM, 0);

    digitalWrite(RIGHT_MOTOR_RIS, LOW);
    digitalWrite(RIGHT_MOTOR_LIS, LOW);
    analogWrite(RIGHT_MOTOR_RPWM, 0);
    analogWrite(RIGHT_MOTOR_LPWM, 200);
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

// Fungsi untuk memproses input gamepad
void processGamepad(ControllerPtr ctl) {
    uint8_t dpad = ctl->dpad();
    uint16_t buttons = ctl->buttons();
    stopMotors();  // Berhenti dulu semua motor
    
    // === Kontrol Motor ===
    if (dpad == 0x01) {  // Atas
        if (buttons & 0x0001) {  // Jika tombol cross ditekan
            moveForward(255);  // Mode turbo maju (kecepatan maksimal)
            Serial.println("Mode Turbo Maju Aktif");
        } else {
            moveForward(200);  // Mode normal maju
            Serial.println("Mode Normal Maju Aktif");
        }
    } else if (dpad == 0x02) {  // Bawah
        if (buttons & 0x0001) {  // Jika tombol cross ditekan
            moveBackward(255);  // Mode turbo mundur (kecepatan maksimal)
            Serial.println("Mode Turbo Mundur Aktif");
        } else {
            moveBackward(200);  // Mode normal mundur
            Serial.println("Mode Normal Mundur Aktif");
        }
    } else if (dpad == 0x08) {  // Kiri
        turnLeft();  // Mode normal kiri
        Serial.println("Mode Normal Kiri Aktif");
    } else if (dpad == 0x04) {  // Kanan
        turnRight();  // Mode normal kanan
        Serial.println("Mode Normal Kanan Aktif");
    } else {
        stopMotors();          // Jika tidak ada input, berhenti
        Serial.println("Semua Motor Berhenti");
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

// Fungsi untuk menampilkan informasi controller
void printControllerInfo(ControllerPtr ctl) {
    ControllerProperties properties = ctl->getProperties();
    Serial.printf("=== Controller Information ===\n");
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
        addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    Serial.printf("===========================\n");
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
            printControllerInfo(ctl);
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
    delay(50);
}