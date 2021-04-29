#include <ArduinoBLE.h>

#define M1IN1       12
#define M1IN2       11
#define M2IN1       2
#define M2IN2       3

float cruise_speed = 128.0;
float turn_angle = 0.0;
float turn_speed = 128.0;
int LED_arr[4] = {4,5,6,7};

void motorDrive(int power, int in1, int in2) {
  if (power >= 0) {
    analogWrite(in2, 0);
    analogWrite(in1, power);
    } else {
    analogWrite(in1, 0);
    analogWrite(in2, abs(power));
    }
}

void update_speed(float V, float theta) {
  float V_outer, V_inner;
  float differential;

  differential = V * sin(1.570796327 - fabs(theta));
  V_inner = differential;
  V_outer = 2*V - differential;
  
  if (theta > 0) {
    motorDrive(V_outer, M1IN1, M1IN2);
    motorDrive(V_inner, M2IN1, M2IN2);
  } else {
    motorDrive(V_inner, M1IN1, M1IN2);
    motorDrive(V_outer, M2IN1, M2IN2);
  }
}

void state_update(float V, float theta) {
  if (V == 0) {
      for (int i=0; i<4; i++) {
        digitalWrite(LED_arr[i], LOW);
      }
  } else {
      if (fabs(theta) < 0.05) {
        for (int i=0; i<4; i++) {
          digitalWrite(LED_arr[i], HIGH);
        }
      } else {
          if (theta < 0) {
            digitalWrite(LED_arr[0], HIGH);
            digitalWrite(LED_arr[1], HIGH);
            digitalWrite(LED_arr[2], LOW);
            digitalWrite(LED_arr[3], LOW);
          } else {
            digitalWrite(LED_arr[0], LOW);
            digitalWrite(LED_arr[1], LOW);
            digitalWrite(LED_arr[2], HIGH);
            digitalWrite(LED_arr[3], HIGH);
          }
      }
  }
}

BLEService ledService("19B10010-E8F2-537E-4F6C-D104768A1214");

BLEStringCharacteristic speedCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 20);
BLEStringCharacteristic angleCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 20);

void setup() {
  Serial.begin(9600);
  
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  BLE.setLocalName("ISDN2400 Group 6");
  BLE.setDeviceName("ISDN2400 Group 6");

  BLE.setAdvertisedService(ledService);

  ledService.addCharacteristic(speedCharacteristic);
  ledService.addCharacteristic(angleCharacteristic);

  BLE.addService(ledService);

  speedCharacteristic.writeValue("0");
  angleCharacteristic.writeValue("0");
  
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
  
  motorDrive(128, M1IN1, M1IN2);
  motorDrive(128, M2IN1, M2IN2);

  for (int i=0; i<4; i++) {
    pinMode(LED_arr[i], OUTPUT);
  }
}

void loop() {
  BLE.poll();
  
  if (speedCharacteristic.written() || angleCharacteristic.written()) {
    String tmpStr1 = speedCharacteristic.value();
    cruise_speed = tmpStr1.toFloat() - 256;
    String tmpStr2 = angleCharacteristic.value();
    turn_angle = (tmpStr2.toFloat() - 60)/180*3.141592;
    Serial.print(speedCharacteristic.value());
    Serial.print("\t");
    Serial.println(angleCharacteristic.value());    
    update_speed(cruise_speed, turn_angle);
    state_update(cruise_speed,  turn_angle);
  }
}
