#define DEBUG       0

#define THRESHOLD_DISTANCE   125
#define TRIGGER_PIN          D5
#define TRIGGER_DELAY        100
#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial tfSerial(D1, D2); // RX, TX

int dist;                     //actual distance measurements of LiDAR
int strength;                 //signal strength of LiDAR
int check;                    //save check value
int i;
int uart[9];                   //save data measured by LiDAR
const int HEADER = 0x59;      //frame header of data package

uint32_t lastUpdateMillis = 0;



void processData() {
  if (dist > 0 && dist < THRESHOLD_DISTANCE) {
    digitalWrite(TRIGGER_PIN, LOW);
    if (DEBUG) { Serial.println("Object detected"); }
  } else {
    digitalWrite(TRIGGER_PIN, HIGH);
  }
}

void checkDistance() {
  if (tfSerial.available()) {
    if (tfSerial.read() == HEADER) {       //assess data package frame header 0x59
      uart[0] = HEADER;
      if (tfSerial.read() == HEADER){      //assess data package frame header 0x59
        uart[1] = HEADER;
        for (i = 2; i < 9; i++){ uart[i] = tfSerial.read(); }         //save data in array
        check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
        if (uart[8] == (check & 0xff)) {       //verify the received data as per protocol
          dist = uart[2] + uart[3] * 256;     //calculate distance value
          if (DEBUG) { Serial.println("Distance = " + String(dist)); }                 //output measure distance value of LiDAR
          processData();
        }
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  tfSerial.begin(115200);

  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, HIGH);
}

void loop() {
  checkDistance();
}

