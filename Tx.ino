#include <SPI.h>
#include <LoRa.h>

int pot = A0;          // Analog pin for the potentiometer
int buttonPin = 4;    // Define the pin for the pushbutton
bool buttonState = false; // State of the pushbutton

void setup() {
  Serial.begin(9600);
  pinMode(pot, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);  // Initialize pushbutton pin as input with pull-up resistor

  while (!Serial);  
  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6)) { // or 915E6, the MHz speed of your module
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  int potValue = analogRead(pot);  // Read the potentiometer value
  buttonState = (digitalRead(buttonPin) == LOW); // Read the pushbutton state

  // Format the data as "potValue,buttonState"
  String dataToSend = String(potValue) + "," + String(buttonState);

  Serial.print("Sending Data: ");
  Serial.println(dataToSend);

  // Start LoRa packet
  LoRa.beginPacket();  
  LoRa.print(dataToSend);
  LoRa.endPacket();

  delay(50);  // Adjust delay as needed
}
