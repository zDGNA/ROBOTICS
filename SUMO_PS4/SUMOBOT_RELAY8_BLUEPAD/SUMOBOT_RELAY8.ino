#include <Bluepad32.h>

const uint8_t targetMacAddress[] = {0xD4, 0xDC, 0xFB, 0x3F, 0x00, 0x00};
ControllerPtr myControllers[BP32_MAX_GAMEPADS];
ControllerPtr connectedController = nullptr;

// Definisi pin relay motor
#define relay1 12  // Motor 1 Forward
#define relay2 13  // Motor 1 Backward
#define relay3 14  // Motor 2 Forward
#define relay4 15  // Motor 2 Backward
#define relay5 32  // Motor 1 Forward (slow)
#define relay6 25  // Motor 1 Backward (slow)
#define relay7 26  // Motor 2 Forward (slow)
#define relay8 23  // Motor 2 Backward (slow)

// Fungsi untuk menghentikan semua motor
void stopMotors() {
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay3, HIGH);
    digitalWrite(relay4, HIGH);
    digitalWrite(relay5, HIGH);
    digitalWrite(relay6, HIGH);
    digitalWrite(relay7, HIGH);
    digitalWrite(relay8, HIGH);
}

// Fungsi untuk menggerakkan motor maju
void moveForward() {
    digitalWrite(relay1, LOW);
    digitalWrite(relay5, HIGH);
    digitalWrite(relay3, LOW);
    digitalWrite(relay4, HIGH);
}

// Fungsi untuk menggerakkan motor mundur
void moveBackward() {
    digitalWrite(relay1, HIGH);
    digitalWrite(relay5, LOW); /// 2 KE 5
    digitalWrite(relay3, HIGH);
    digitalWrite(relay4, LOW);
}

// Fungsi untuk menggerakkan motor belok kiri
void turnLeft() {
    digitalWrite(relay1, HIGH);
    digitalWrite(relay5, LOW);
    digitalWrite(relay3, LOW);
    digitalWrite(relay4, HIGH);
}

// Fungsi untuk menggerakkan motor belok kanan
void turnRight() {
    digitalWrite(relay1, LOW);
    digitalWrite(relay5, HIGH);
    digitalWrite(relay3, HIGH);
    digitalWrite(relay4, LOW);
}

// Fungsi untuk menggerakkan motor maju (slow)
void moveForwardSlow() {
    digitalWrite(relay2, HIGH);
    digitalWrite(relay6, LOW);
    digitalWrite(relay7, LOW);
    digitalWrite(relay8, HIGH);
}

// Fungsi untuk menggerakkan motor mundur (slow)
void moveBackwardSlow() {
    // digitalWrite(relay2, HIGH);
    // digitalWrite(relay6, LOW); 
    // digitalWrite(relay7, HIGH);
    // digitalWrite(relay8, LOW); ///8 UBAH KE 7 DAN SEBALIKNYA
}

// Fungsi untuk menggerakkan motor belok kiri (slow)
void turnLeftSlow() {
    // digitalWrite(relay2, HIGH);
    // digitalWrite(relay6, LOW);
    // digitalWrite(relay7, LOW);
    // digitalWrite(relay8, HIGH);
}

// Fungsi untuk menggerakkan motor belok kanan (slow)
void turnRightSlow() {
    // digitalWrite(relay2, LOW);
    // digitalWrite(relay6, HIGH);
    // digitalWrite(relay7, HIGH);
    // digitalWrite(relay8, LOW);
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
            moveForwardSlow();  // Aktifkan mode slow maju
            Serial.println("Mode Slow Maju Aktif");
        } else {
            moveForward();  // Mode normal maju
            Serial.println("Mode Normal Maju Aktif");
        }
    } else if (dpad == 0x02) {  // Bawah
        if (buttons & 0x0001) {  // Jika tombol cross ditekan
            moveBackwardSlow();  // Aktifkan mode slow mundur
            Serial.println("Mode Slow Mundur Aktif");
        } else {
            moveBackward();  // Mode normal mundur
            Serial.println("Mode Normal Mundur Aktif");
        }
    } else if (dpad == 0x08) {  // Kiri
        if (buttons & 0x0001) {  // Jika tombol cross ditekan
            turnLeftSlow();  // Aktifkan mode slow kiri
            Serial.println("Mode Slow Kiri Aktif");
        } else {
            turnLeft();  // Mode normal kiri
            Serial.println("Mode Normal Kiri Aktif");
        }
    } else if (dpad == 0x04) {  // Kanan
        if (buttons & 0x0001) {  // Jika tombol cross ditekan
            turnRightSlow();  // Aktifkan mode slow kanan
            Serial.println("Mode Slow Kanan Aktif");
        } else {
            turnRight();  // Mode normal kanan
            Serial.println("Mode Normal Kanan Aktif");
        }
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
    // Dapatkan properti controller
    ControllerProperties properties = ctl->getProperties();
    // Tampilkan informasi controller
    Serial.printf("=== Controller Information ===\n");
    // Tampilkan MAC address
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
        addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    Serial.printf("===========================\n");
}

// Fungsi untuk menangani koneksi controller
void onConnectedController(ControllerPtr ctl) {
    // Dapatkan MAC address controller yang mencoba terhubung
    const uint8_t* controllerMac = ctl->getProperties().btaddr;  // Ganti bdaddr dengan btaddr
    // Cek apakah MAC address sesuai
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
    // Jika MAC address sesuai, proses koneksi seperti biasa
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

// Fungsi untuk menginisialisasi semua relay dalam keadaan mati
void initializeRelays() {
    // Inisialisasi semua relay sebagai OUTPUT
    pinMode(relay1, OUTPUT);
    pinMode(relay2, OUTPUT);
    pinMode(relay3, OUTPUT);
    pinMode(relay4, OUTPUT);
    pinMode(relay5, OUTPUT);
    pinMode(relay6, OUTPUT);
    pinMode(relay7, OUTPUT);
    pinMode(relay8, OUTPUT);
    // Pastikan semua relay dalam keadaan mati saat startup
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay3, HIGH);
    digitalWrite(relay4, HIGH);
    digitalWrite(relay5, HIGH);
    digitalWrite(relay6, HIGH);
    digitalWrite(relay7, HIGH);
    digitalWrite(relay8, HIGH);
    delay(100);  // Tunggu sebentar untuk memastikan semua relay mati
    Serial.println("Semua relay telah diinisialisasi dalam keadaan MATI");
}

// Fungsi setup
void setup() {
    Serial.begin(115200);
    // Inisialisasi relay terlebih dahulu sebelum sistem lainnya
    initializeRelays();
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.enableVirtualDevice(false);
}

// Fungsi loop utama
void loop() {
    BP32.update();
    processControllers();
    delay(50);
}