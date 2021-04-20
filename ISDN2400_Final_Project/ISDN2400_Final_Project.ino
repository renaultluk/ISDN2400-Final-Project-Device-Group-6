#define M1IN1       12
#define M1IN2       11
#define M2IN1       10
#define M2IN2       9

float cruise_speed = 128.0;
float turn_speed = 128.0;

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

void forward() {
    motorDrive(cruise_speed, M1IN1, M1IN2);
    motorDrive(cruise_speed, M2IN1, M2IN2);    
}

void swerve_right() {}

void swerve_left() {}

void turn_left() {
    motorDrive(-turn_speed, M1IN1, M1IN2);
    motorDrive(turn_speed, M2IN1, M2IN2);
}

void turn_right() {
    motorDrive(turn_speed, M1IN1, M1IN2);
    motorDrive(-turn_speed, M2IN1, M2IN2);
}

void halt() {
    motorDrive(0, M1IN1, M1IN2);
    motorDrive(0, M2IN1, M2IN2);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available > 0) {
     String data = Serial.read();
     bluetooth_parse(data);
  }
}
