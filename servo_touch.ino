#include <Servo.h>

// Pin definitions
const int TOUCH_SENSOR_PIN = 2;       // TTP223 touch sensor pin
const int ORIENTATION_SENSOR_PIN = 3;  // Sensor indicating back vertical on wall (HIGH when vertical)
const int SERVO_PIN = 9;              // Servo signal pin

// Servo positions
const int INITIAL_ANGLE = 0;
const int TARGET_ANGLE = 90;          // 90 degrees clockwise from initial position

Servo myServo;

void setup() {
  pinMode(TOUCH_SENSOR_PIN, INPUT);
  pinMode(ORIENTATION_SENSOR_PIN, INPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(INITIAL_ANGLE);
}

void loop() {
  int touchState = digitalRead(TOUCH_SENSOR_PIN);
  int isVertical = digitalRead(ORIENTATION_SENSOR_PIN);

  // If back is vertical on a wall and touch sensor is touched (HIGH)
  if (isVertical == HIGH && touchState == HIGH) {
    myServo.write(TARGET_ANGLE);
  } else {
    myServo.write(INITIAL_ANGLE);
  }

  delay(50);
}
