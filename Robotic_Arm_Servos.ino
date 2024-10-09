#include <Servo.h> // Library for servo motor control

// Joystick analog input pins
const int JOYSTICK_1_X_PIN = A0; // Joystick 1 X-axis
const int JOYSTICK_1_Y_PIN = A1; // Joystick 1 Y-axis
const int JOYSTICK_2_Y_PIN = A2; // Joystick 2 Y-axis
const int JOYSTICK_BUTTON_PIN = 2; // Joystick button pin

// Servo pins
const int BASE_SERVO_PIN = 9;
const int SHOULDER_SERVO_PIN = 10;
const int ELBOW_SERVO_PIN = 11;

// Servo angles limits
const int BASE_MIN_ANGLE = 0;
const int BASE_MAX_ANGLE = 180;
const int SHOULDER_MIN_ANGLE = 0;
const int SHOULDER_MAX_ANGLE = 180;
const int ELBOW_MIN_ANGLE = 0;
const int ELBOW_MAX_ANGLE = 180;

// Create servo objects
Servo baseServo;
Servo shoulderServo;
Servo elbowServo;

// Previous joystick positions
int prevJoystick1X = 0;
int prevJoystick1Y = 0;
int prevJoystick2Y = 0;

// Flag to indicate if the servos are in hold mode
bool holdMode = false;
bool buttonState = false;
bool lastButtonState = false;

// Function to map joystick values to servo angles
int mapJoystickToAngle(int joystickValue, int minAngle, int maxAngle)
{
  return map(joystickValue, 0, 1023, minAngle, maxAngle);
}

void setup()
{
  // Attach servo motors to pins
  baseServo.attach(BASE_SERVO_PIN);
  shoulderServo.attach(SHOULDER_SERVO_PIN);
  elbowServo.attach(ELBOW_SERVO_PIN);

  // Set initial servo angles
  baseServo.write(BASE_MIN_ANGLE);
  shoulderServo.write(SHOULDER_MIN_ANGLE);
  elbowServo.write(ELBOW_MIN_ANGLE);

  // Store initial joystick positions
  prevJoystick1X = analogRead(JOYSTICK_1_X_PIN);
  prevJoystick1Y = analogRead(JOYSTICK_1_Y_PIN);
  prevJoystick2Y = analogRead(JOYSTICK_2_Y_PIN);

  // Configure joystick button pin as input
  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
}

void loop()
{
  // Read joystick values
  int joystick1X = analogRead(JOYSTICK_1_X_PIN);
  int joystick1Y = analogRead(JOYSTICK_1_Y_PIN);
  int joystick2Y = analogRead(JOYSTICK_2_Y_PIN);

  // Check if joystick positions are within the dead zone threshold
  const int deadZoneThreshold = 10; // Adjust this value if needed

  // Read the current state of the button
  buttonState = digitalRead(JOYSTICK_BUTTON_PIN);

  if (!holdMode) {
    // Servo control when not in hold mode
    if (abs(joystick1X - prevJoystick1X) > deadZoneThreshold) {
      int baseAngle = mapJoystickToAngle(joystick1X, BASE_MIN_ANGLE, BASE_MAX_ANGLE);
      baseServo.write(baseAngle);
      prevJoystick1X = joystick1X;
    }
    if (abs(joystick1Y - prevJoystick1Y) > deadZoneThreshold) {
      int shoulderAngle = mapJoystickToAngle(joystick1Y, SHOULDER_MIN_ANGLE, SHOULDER_MAX_ANGLE);
      shoulderServo.write(shoulderAngle);
      prevJoystick1Y = joystick1Y;
    }
    if (abs(joystick2Y - prevJoystick2Y) > deadZoneThreshold) {
      int elbowAngle = mapJoystickToAngle(joystick2Y, ELBOW_MIN_ANGLE, ELBOW_MAX_ANGLE);
      elbowServo.write(elbowAngle);
      prevJoystick2Y = joystick2Y;
    }

    // Check if the button state has changed
    if (buttonState != lastButtonState && buttonState == LOW) {
      holdMode = true;
    }
  } else {
    // Servo hold mode
    if (buttonState != lastButtonState && buttonState == LOW) {
      holdMode = false;
    }
  }

  lastButtonState = buttonState;

  delay(50); // Delay for smooth control (adjust if needed)
}
