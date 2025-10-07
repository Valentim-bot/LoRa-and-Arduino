#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

/* 
 *  Receiver Side Code
 * 
  Module SX1278 // Arduino UNO/NANO    
    Vcc         ->   3.3V
    MISO        ->   D12
    MOSI        ->   D11     
    SLCK        ->   D13
    Nss         ->   D10
    GND         ->   GND
 */
 
// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

int LED = 3;
String inString = "";    // String to hold input
int potValue = 0;
int pwmValue = 0;
bool ledState = false;  // Track the LED state (On or OFF)

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  // Initialize OLED display with I2C address 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Infinite loop if OLED initialization fails
  }

  display.display();
  delay(2000);  // Pause for 2 seconds to display startup message
  display.clearDisplay();  // Clear the initial display

  while (!Serial);
  Serial.println("LoRa Receiver");

  if (!LoRa.begin(433E6)) { // Initialize LoRa with frequency 433 MHz (or use 915E6 for 915 MHz)
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  // Try to parse the incoming LoRa packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) { 
    // Read packet
    while (LoRa.available()) {
      int inChar = LoRa.read();  // Read a character from LoRa
      inString += (char)inChar;  // Append character to inString
    }

    // Process the received string
    int commaIndex = inString.indexOf(',');
    if (commaIndex != -1) {
      String potString = inString.substring(0, commaIndex);
      String buttonString = inString.substring(commaIndex + 1);
      
      potValue = potString.toInt();  // Convert potentiometer value to integer
      bool buttonState = (buttonString == "1");  // Convert button state to boolean

      // Set PWM value based on button state
      if (buttonState) {
        pwmValue = 255;  // Set PWM to 255 when button is pressed
      } else {
        pwmValue = map(potValue, 0, 1023, 0, 255);  // Map potentiometer value to PWM range
      }

      // Determine LED state based on PWM value
      if (pwmValue > 0 || buttonState) {
        ledState = true;  // Turn on LED if potentiometer value is non-zero or button is pressed
      } else {
        ledState = false; // Turn off LED
      }

      inString = "";  // Clear the string for the next packet

      // Update the OLED display
      display.clearDisplay();
      display.setTextSize(2);  // Set text size to 2
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.print("PWM: ");
      display.println(pwmValue);  // Display the mapped PWM value
      display.setCursor(0, 30);  // Move cursor to next line
      display.print("State: ");
      display.println(ledState ? "ON" : "OFF");  // Display LED state
      display.display();  // Refresh the OLED display

      Serial.print("Potentiometer Value: ");
      Serial.print(potValue);
      Serial.print(" -> PWM Value: ");
      Serial.print(pwmValue);
      Serial.print(" LED State: ");
      Serial.println(ledState ? "ON" : "OFF");
    }
  }

  // Control the LED based on the LED state
  if (ledState) {
    
     
      analogWrite(LED, pwmValue);  // Set LED brightness based on potentiometer value
  } else {
   digitalWrite(LED, 0);  // Turn OFF the LED by setting brightness to 0
    }
   
}
