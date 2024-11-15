/**
 * @file MotionSensorWebServer.ino
 * @brief An ESP32-based motion detection system with an OLED display and a web interface.
 *
 * This program uses an ESP32 to monitor a motion sensor, control an LED, and display the 
 * motion status on an OLED screen and a web page. It also counts the number of motion events.
 *
 * Dependencies:
 * - Adafruit_GFX library
 * - Adafruit_SSD1306 library
 * - HTTPClient library
 * - WiFi library
 */

/**
 * @brief Wi-Fi credentials
 */
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <WiFi.h>

const char* ssid = "e307";        ///< Wi-Fi SSID
const char* password = "rockyCartoon544"; ///< Wi-Fi Password

#define timeSeconds 10 ///< Time interval in seconds for motion detection

/**
 * @brief GPIO pins
 */
const int led = 26;           ///< GPIO pin for the LED
const int motionSensor = 27;  ///< GPIO pin for the motion sensor

/**
 * @brief OLED display dimensions
 */
#define SCREEN_WIDTH 128      ///< OLED display width in pixels
#define SCREEN_HEIGHT 32      ///< OLED display height in pixels

// Global variables
unsigned long now = millis();  ///< Current time in milliseconds
unsigned long lastTrigger = 0; ///< Last time motion was detected
boolean startTimer = false;    ///< Flag to start the timer
int motion = 0;                ///< Motion counter
int buttonState = 0;           ///< Current state of the motion sensor
int lastButtonState = LOW;     ///< Last recorded state of the motion sensor

/**
 * @brief OLED display object
 * @details Connected via I2C with default address 0x3C.
 */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/**
 * @brief Web server object
 */
WiFiServer server(80);

/**
 * @brief HTTP request header storage
 */
String header;

/**
 * @brief Sets up the ESP32, OLED display, and Wi-Fi connection.
 */
void setup() {
    Serial.begin(115200);

    pinMode(motionSensor, INPUT);
    pinMode(led, OUTPUT);

    // Initialize Wi-Fi
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    server.begin();

    // Initialize OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    delay(2000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 10);
    display.println(WiFi.localIP());
    display.display();
}

/**
 * @brief Main loop to handle motion detection, OLED display updates, and web server requests.
 */
void loop() {
  // Read motion sensor
  buttonState = digitalRead(motionSensor);
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
        digitalWrite(led, LOW);  // Turn off LED
        display.clearDisplay();
        display.println("No Motion Detected");
        display.display();
        motion++;  // Increment motion counter
    } else {
        digitalWrite(led, HIGH); // Turn on LED
        display.clearDisplay();
        display.println("Motion Detected");
        display.display();
    }
    lastButtonState = buttonState;
  }

  // Handle web server requests
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

                  // Generate HTML content
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
