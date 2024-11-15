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
int lastButtonState = LOW;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
int motion = 0;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

void setup() {
  Serial.begin(115200);

  pinMode(motionSensor, INPUT);  
  pinMode(led, OUTPUT);

  // Connect to Wi-Fi network
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println(WiFi.localIP());
  display.display(); 
}

void loop() {
  // Read motion sensor
  
  buttonState = digitalRead(motionSensor);  
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      digitalWrite(led, LOW);  // Turn off LED when no motion is detected
      display.clearDisplay();
      display.println("No Motion Detected");
      display.display();
      motion++; // Increment motion counter
    } else {
      digitalWrite(led, HIGH); // Turn on LED when motion is detected
      display.clearDisplay();
      display.println("Motion Detected");
      display.display();
      
    }
    lastButtonState = buttonState; // Update lastButtonState
  }

  // Handle web server
  WiFiClient client = server.available();
  if (client) {                            
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display motion status on the web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}</style></head>");
            client.println("<body><h1>ESP32 Motion Sensor</h1>");
            if (motion) {
                client.print("<p>Motion Count: <strong>");
                client.print(motion);
                client.println("</strong></p>");
            } else {
                client.println("<p>No motion detected</p>");
            }
            client.println("</body></html>");

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
  }
}
