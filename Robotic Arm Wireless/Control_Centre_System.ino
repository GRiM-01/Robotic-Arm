#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(10, 53); // CE, CSN pins
const byte address[6] = "00001"; // Address for communication

Servo servo1;
Servo servo2;
Servo servo3;

const int trigPinHorizontal = 2; // Trig pin for horizontal ultrasonic sensor
const int echoPinHorizontal = 3; // Echo pin for horizontal ultrasonic sensor
const int trigPinVertical = 4; // Trig pin for vertical ultrasonic sensor
const int echoPinVertical = 5; // Echo pin for vertical ultrasonic sensor

const int buttonPin = 6; // Push button pin
bool isButtonPressed = false; // Button state
bool buttonClickedOnce = false; // Flag to track single click
bool buttonClickedTwice = false; // Flag to track double click
unsigned long buttonClickTime = 0; // Variable to store button click time
const unsigned long doubleClickInterval = 300; // Maximum interval between clicks for a double click

const int redLedPin = 11; // Red LED pin
const int greenLedPin = 12; // Green LED pin

struct DataPacket {
  float joystickX_1;
  float joystickY_1;
  float joystickY_2;
  float horizontalDistance;
  float verticalDistance;
};

DataPacket data;
float finalDistance = 0.0;

void setup() {
  Serial.begin(9600);
  
  servo1.attach(7); // Servo1 pin
  servo2.attach(8); // Servo2 pin
  servo3.attach(9); // Servo3 pin
  
  pinMode(trigPinHorizontal, OUTPUT);
  pinMode(echoPinHorizontal, INPUT);
  pinMode(trigPinVertical, OUTPUT);
  pinMode(echoPinVertical, INPUT);
  
  pinMode(buttonPin, INPUT_PULLUP); // Set button pin as input with internal pull-up resistor
  
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(data));
    
    // Check if button is clicked once or twice
    if (buttonClickedOnce) {
      data.horizontalDistance = measureDistance(trigPinHorizontal, echoPinHorizontal);
      data.verticalDistance = 0.0; // Set vertical distance to zero
      
      digitalWrite(greenLedPin, HIGH); // Turn on green LED
      digitalWrite(redLedPin, LOW); // Turn off red LED
    } else if (buttonClickedTwice) {
      data.verticalDistance = measureDistance(trigPinVertical, echoPinVertical);
      data.horizontalDistance = 0.0; // Set horizontal distance to zero
      
      digitalWrite(greenLedPin, LOW); // Turn off green LED
      digitalWrite(redLedPin, HIGH); // Turn on red LED
    } else {
      data.horizontalDistance = 0.0; // Set horizontal distance to zero
      data.verticalDistance = 0.0; // Set vertical distance to zero
      
      digitalWrite(greenLedPin, LOW); // Turn off green LED
      digitalWrite(redLedPin, LOW); // Turn off red LED
    }
    
    radio.stopListening();
    radio.write(&data, sizeof(data));
    radio.startListening();
    
    moveServos(data.joystickX_1, data.joystickY_1, data.joystickY_2);
    
    // Update final distance based on button state
    if (buttonClickedOnce) {
      finalDistance = data.horizontalDistance;
    } else if (buttonClickedTwice) {
      finalDistance = data.verticalDistance;
    } else {
      finalDistance = 0.0; // No distance measured
    }

    // Display final distance
    Serial.print("Final Distance: ");
    Serial.println(finalDistance);

    Serial.print("Received Joystick X: ");
    Serial.println(data.joystickX_1);
    Serial.print("Received Joystick Y: ");
    Serial.println(data.joystickY_1);
    Serial.print("Received Joystick Y: ");
    Serial.println(data.joystickY_2);
    Serial.print("Horizontal Distance: ");
    Serial.println(data.horizontalDistance);
    Serial.print("Vertical Distance: ");
    Serial.println(data.verticalDistance);
  }
  
  // Check the state of the button
  if (digitalRead(buttonPin) == LOW) {
    if (!isButtonPressed) {
      // Button pressed for the first time
      isButtonPressed = true;
      buttonClickTime = millis();
    } else {
      // Button pressed while it was already pressed
      unsigned long elapsedTime = millis() - buttonClickTime;
      if (elapsedTime <= doubleClickInterval) {
        // Double click detected
        buttonClickedTwice = true;
        buttonClickedOnce = false;
        isButtonPressed = false;
      } else {
        // Time between clicks exceeded the double click interval
        // Treat it as a new single click
        buttonClickTime = millis();
      }
    }
  } else {
    // Button released
    if (isButtonPressed) {
      unsigned long elapsedTime = millis() - buttonClickTime;
      if (elapsedTime <= doubleClickInterval) {
        // Single click detected
        buttonClickedOnce = true;
        buttonClickedTwice = false;
      }
    }
    isButtonPressed = false;
  }
}

float measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(3);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2; // Speed of sound in air = 34,000 cm/s
  
  return distance;
}

void moveServos(float joystickX_1, float joystickY_1, float joystickY_2) {
  int servo1Angle = map(joystickX_1, 0, 1023, 0, 180);
  int servo2Angle = map(joystickY_1, 0, 1023, 0, 180);
  int servo3Angle = map(joystickY_2, 0, 1023, 0, 180);  // You can modify this angle as needed
  
  servo1.write(servo1Angle);
  servo2.write(servo2Angle);
  servo3.write(servo3Angle);
}