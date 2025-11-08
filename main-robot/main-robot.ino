#include <SoftwareSerial.h>
SoftwareSerial module_bluetooth(11, 12);  // RX | TX

#define r1 7
#define r2 4
#define enR 6

#define l1 2
#define l2 3
#define enL 5

float rPwm = 90;
int lPwm = 90;

void setup() {
  module_bluetooth.begin(38400);
  Serial.begin(9600);

  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(l1, OUTPUT);
  pinMode(l2, OUTPUT);

  pinMode(enR, OUTPUT);
  pinMode(enL, OUTPUT);
}

void loop() {
  if (module_bluetooth.available()) {
    char c = module_bluetooth.read();
    if (c == 'f') {
      forward(rPwm, lPwm);
    } else if (c == 'b') {
      backward(rPwm, lPwm);
    } else if ( c == 'r') {
      right(rPwm, lPwm);
    } else if ( c == 'l') {
      left(rPwm, lPwm);
    } else if (c == 'n') {
      stop();
    }
  }
}

void forward(int rPwm, int lPwm) {
  digitalWrite(r1,HIGH);
  digitalWrite(r2,LOW);
  digitalWrite(l1,HIGH);
  digitalWrite(l2,LOW);
  analogWrite(enR, rPwm);
  analogWrite(enL, lPwm);
}

void stop() {
  digitalWrite(r1,LOW);
  digitalWrite(r2,LOW);
  digitalWrite(l1,LOW);
  digitalWrite(l2,LOW);
  analogWrite(enR, 0);
  analogWrite(enL, 0);
}

void backward(int rPwm, int lPwm) {
  digitalWrite(r1,LOW);
  digitalWrite(r2,HIGH);
  digitalWrite(l1,LOW);
  digitalWrite(l2,HIGH);
  analogWrite(enR, rPwm);
  analogWrite(enL, lPwm);
}

void right(int rPwm, int lPwm) {
  digitalWrite(r1,LOW);
  digitalWrite(r2,HIGH);
  digitalWrite(l1,HIGH);
  digitalWrite(l2,LOW);
  analogWrite(enR, rPwm);
  analogWrite(enL, lPwm);
}

void left(int rPwm, int lPwm) {
  digitalWrite(r1,HIGH);
  digitalWrite(r2,LOW);
  digitalWrite(l1,LOW);
  digitalWrite(l2,HIGH);
  analogWrite(enR, rPwm);
  analogWrite(enL, lPwm);
}