//import all required libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
//Import i2C LCD libraries
#include <LiquidCrystal_I2C.h>
int lcdColumns = 16;
int lcdRows = 2;
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
const char* ssid = "xxxxxx";
const char* password = "xxxxxx";
//Define API ke
String openWeatherMapApiKey = "191673ed63f3a3efxxxxxx";

String city = "xxxxx"; //specify your city
String countryCode = "xxxx"; //specify your country code

unsigned long last_time = 0;
unsigned long timer_delay = 10000;

String jsonBuffer;

void setup() {
  Serial.begin(115200);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WIFI...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("First set of readings will appear after 10 seconds");
}

void loop() {
  // Send an HTTP GET request
  if ((millis() - last_time) > timer_delay) {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;

      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);

      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      Serial.println(myObject["main"]["temp"]);// Reading in kelvin
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
      lcd.clear();                            //LCD Printing Temperature
      lcd.print("T:");
      lcd.print(myObject["main"]["temp"]);
      lcd.print("k");
      lcd.print(" ");
      lcd.print("H:");                       //LCD Printing Humidity
      lcd.print(myObject["main"]["humidity"]);
      lcd.print("%");
      lcd.setCursor(0, 1);                    //Printing Pressure
      lcd.print("P:");
      lcd.print(myObject["main"]["pressure"]);
      lcd.print(" hPa");
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    last_time = millis();
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
