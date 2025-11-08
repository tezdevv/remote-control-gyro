#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Servo.h>

#define SB 2
#define OB 3

SoftwareSerial btSerial(11, 12);
Adafruit_MPU6050 mpu;
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

int sbc = 0;
int obc = 0;
int onPage = 0;
int statePage = 0;

int stateIntro = 0;

unsigned long lastGyroTime = 0;
const unsigned long gyroInterval = 50; // ms
bool gyroModeActive = false;

unsigned long lastDebounceTimeSB = 0;
unsigned long lastDebounceTimeOB = 0;
const unsigned long debounceDelay = 250;

Servo servoAtas, servoTengah, servoBawah;

int accX;
int accY;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(100000);

  if (!mpu.begin()) {
    Serial.println("Gagal menemukan chip MPU6050. Periksa koneksi dan alamat I2C.");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  delay(100);

  Serial.println("MPU6050 siap digunakan.");

  u8g2.begin();
  btSerial.begin(38400);

  servoAtas.attach(4);
  servoAtas.attach(5);
  servoAtas.attach(6);

  pinMode(SB, INPUT_PULLUP);
  pinMode(OB, INPUT_PULLUP);

  Serial.println("OK");
}

const char* intro1 = "</>";
const char* intro2 = "TZDV";
const char* intro3 = "PRESS R BUTTON FOR CHOOSE MODE";

const char* strBLc = "> Bluetooth";
const char* strWRc = "> Wire";
const char* strBLv = "Bluetooth(v)";
const char* strWRv = "Wire(v)";

// void loop() {
//   sensors_event_t a, g, temp;
//   mpu.getEvent(&a, &g, &temp);

//   accX = map((int)a.acceleration.x, -10, 10, 180, 0);
//   accY = map((int)a.acceleration.y, -10, 10, 180, 0);

//   int sAtas = map(accY, 180, 0, 0, 180);
//   int sTengah = map(accX, 90, 0, 0, 100);
//   int sBawah = map(accY, 180, 0, 0, 180);

//   if (sAtas == 90 || sBawah == 90) {
//     servoAtas.write(90);
//     servoTengah.write(sTengah);
//   } else {
//     if (sTengah >= 80) {
//       servoAtas.write(sAtas);
//     } else if (sTengah >= 0 && sTengah < 80) {
//       servoBawah.write(sBawah);
//     }
//   }

//   u8g2.clearBuffer();
//   u8g2.setFont(u8g2_font_10x20_tr);
//   u8g2.drawStr(2, 29, "manut");
//   u8g2.sendBuffer();

//   Serial.print("atas = " );
//   Serial.print(sAtas);
//   Serial.print("tengah = " );
//   Serial.println(sTengah);
//   delay(200);
// }

void loop() {
  if (stateIntro == 0) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_logisoso28_tr);
    u8g2.drawStr(8, 29, intro1);
    u8g2.sendBuffer();
    delay(500);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_logisoso28_tr);
    u8g2.drawStr(8, 29, intro2);
    u8g2.sendBuffer();
    delay(500);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(8, 29, intro3);
    u8g2.sendBuffer();
    delay(500);
    stateIntro = 1;
  } else if (stateIntro == 1) {
    int readSB = !digitalRead(SB);
    int readOB = !digitalRead(OB);

    unsigned long currentTime = millis();

    if (readOB == 1 && (currentTime - lastDebounceTimeOB) > debounceDelay) {
      lastDebounceTimeOB = currentTime;

      if (statePage == 0) {
        onPage += 1;
        if (onPage > 1) onPage = 0;
        if (onPage == 0) {
          txtBLc();
          Serial.println("Bluetooth");
        } else {
          txtWRc();
          Serial.println("Wire");
        }
      } else if (statePage == 1) {
        gyroModeActive = false;
        if (onPage == 0) {
          for (int i = 0; i <= 15; i++){
            btSerial.print('n');
            delay(10);
          }
          txtBLc();
          Serial.println("Bluetooth");
        } else {
          mode deffault WR
          txtWRc();
          Serial.println("Wire");
        }
        statePage = 0;
      }
    }

    if (readSB == 1 && (currentTime - lastDebounceTimeSB) > debounceDelay) {
      lastDebounceTimeSB = currentTime;

      if (statePage == 0) {
        gyroModeActive = true;
        if (onPage == 0) {
          txtBLv();
          blGyro();
          Serial.println("Bluetooth V");
        } else {
          txtWRv();
          Serial.println("Wire V");
        }
        statePage = 1;
      }
    }

    if (gyroModeActive) {
      blGyro();
    }
  }
}


void blGyro() {
  unsigned long currentTime = millis();
  if (currentTime - lastGyroTime < gyroInterval) return;

  lastGyroTime = currentTime;

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  Serial.println("MPU OK");

  accX = map((int)a.acceleration.x, -10, 10, 180, 0);
  accY = map((int)a.acceleration.y, -10, 10, 180, 0);
  
  char move;

  if (accX < 60) {
    if (accY < 40) {
      move = 'r';
    } else if (accY > 140) {
      move = 'l';
    } else {
      move = 'f';
    }
  } else if (accX > 130) {
    if (accY < 40) {
      move = 'r';
    } else if (accY > 140) {
      move = 'l';
    } else {
      move = 'b';
    }
  } else if (accX >= 60 && accX <= 130) {
    if (accY < 40) {
      move = 'r';
    } else if (accY > 140) {
      move = 'l';
    } else {
      move = 'n';
    }
  }

  btSerial.print(move);
  Serial.print("Dikirim: ");
  Serial.println(move);
}

void wire() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  accX = map((int)a.acceleration.x, -10, 10, 180, 0);
  accY = map((int)a.acceleration.y, -10, 10, 180, 0);

  int sAtas = map(accY, 180, 0, 0, 90);
  int sTengah = map(accX, 180, 0, 0, 180);
  // int sBawah= map(accY,,,,)

  if (accX < 60) {
  } else if (accX > 130) {
    if (accY < 40) {
      move = 'r';
    } else if (accY > 140) {
      move = 'l';
    } else {
      move = 'b';
    }
  }
}

void wireGyro() {
 
}

void txtBLc() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_10x20_tr);
  u8g2.drawStr(2, 29, strBLc);
  u8g2.sendBuffer();
}

void txtBLv() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_10x20_tr);
  u8g2.drawStr(2, 29, strBLv);
  u8g2.sendBuffer();
}

void txtWRv() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_10x20_tr);
  u8g2.drawStr(2, 29, strWRv);
  u8g2.sendBuffer();
}

void txtWRc() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_10x20_tr);
  u8g2.drawStr(2, 29, strWRc);
  u8g2.sendBuffer();
}