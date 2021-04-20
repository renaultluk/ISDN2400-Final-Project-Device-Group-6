#define M1IN1       12
#define M1IN2       11
#define M2IN1       10
#define M2IN2       9

float cruise_speed = 128.0;
float turn_angle = 0.0;
float turn_speed = 128.0;
int[] LED_arr = [2,3,4,5];

void bluetooth_parse(String input) {
  String command;
  String value_str;
  float value;
  byte flag = 0;
  command = input.substring(0,2);
  value_str = input.substring(2);
  value = value_str.toFloat();

  if (command == "cs") {
      cruise_speed = value;
  } else if (command == "an") {
      turn_angle = value;
  }
}

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

  differential = V * sin((pi/2) - fabs(theta));
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


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  motorDrive(128, M1IN1, M1IN2);
  motorDrive(128, M2IN1, M2IN2);
  delay(100000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
     String data = Serial.readString();
     bluetooth_parse(data);
     update_speed(cruise_speed, turn_angle);
  }
}
