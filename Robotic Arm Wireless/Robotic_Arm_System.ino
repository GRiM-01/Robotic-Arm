#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

RF24 radio(9, 10); // CE, CSN pins
const byte address[6] = "00001"; // Address for communication

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address, number of columns, number of rows

struct DataPacket {
  float joystickX_1;
  float joystickY_1;
  float joystickY_2;
  float horizontalDistance;
  float verticalDistance;
};

DataPacket data;

const int transmitLedPin = 5; // Pin for the transmit LED
const int buttonPin = 2; // Pin for the button (replace with the actual pin number)

bool lastButtonState = HIGH; // Variable to store the last button state
bool currentButtonState; // Variable to store the current button state
bool armPositionChanged = false; // Flag to indicate if the arm position has changed

void setup() {
  Serial.begin(9600);
  
  pinMode(transmitLedPin, OUTPUT); // Set the transmit LED pin as an output
  
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.print("Horz:");
  lcd.setCursor(0, 1);
  lcd.print("Vert:");
  
  radio.begin();
  radio.openWritingPipe(address);
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_LOW);
  
  digitalWrite(transmitLedPin, LOW); // Initially turn off the transmit LED
  pinMode(buttonPin, INPUT); // Set the button pin as input
}

void loop() {
  data.joystickX_1 = analogRead(A0); // Read joystick X-axis value
  data.joystickY_1 = analogRead(A1); // Read joystick Y-axis value
  data.joystickY_2 = analogRead(A2); // Read joystick Y-axis value
  
  // Check the button state
  currentButtonState = digitalRead(buttonPin);
  
  // Compare the current button state with the last button state
  if (currentButtonState != lastButtonState) {
    // If the button state has changed, update the last position flag
    armPositionChanged = true;
  }
  
  lastButtonState = currentButtonState; // Update the last button state
  
  radio.stopListening();
  bool transmissionStatus = radio.write(&data, sizeof(data)); // Store the transmission status
  radio.startListening();
  
  if (transmissionStatus) {
    digitalWrite(transmitLedPin, HIGH); // Turn on the transmit LED if data is transmitted
  } else {
    digitalWrite(transmitLedPin, LOW); // Turn off the transmit LED if data is not transmitted
  }
  
  delay(500); // Delay for 500 milliseconds (adjust as needed)
  
  if (radio.available()) {
    radio.read(&data, sizeof(data));
    
    lcd.setCursor(5, 0);
    lcd.print("       cm");
    lcd.setCursor(5, 0);
    lcd.print(data.horizontalDistance);
    
    lcd.setCursor(5, 1);
    lcd.print("       cm");
    lcd.setCursor(5, 1);
    lcd.print(data.verticalDistance);
    
    Serial.print("Received Horizontal Distance: ");
    Serial.println(data.horizontalDistance);
    Serial.print("Received Vertical Distance: ");
    Serial.println(data.verticalDistance);
  }
  
  delay(1500); // Delay for 1500 milliseconds (adjust as needed)
}
