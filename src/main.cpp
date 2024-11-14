#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <WiFi.h>

#define timeSeconds 10

const char* ssid = "e307";
const char* password = "rockyCartoon544";
const int led = 26;
const int motionSensor = 27;
int buttonState = 0;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
boolean motion = false;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup() {
  Serial.begin(115200);

  pinMode(motionSensor, INPUT);  

  pinMode(led, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  // Display static text
  display.println(WiFi.localIP());
  display.display(); 


}

void loop() {
  buttonState = digitalRead(motionSensor);  
  if(buttonState == HIGH) {
    digitalWrite(led, HIGH);
    display.clearDisplay();
    display.println("Motion Detected");
    display.display();
  }else {
    digitalWrite(led, LOW);
    display.clearDisplay();
    display.println("No Motion Detected");
    display.display();
  }
}