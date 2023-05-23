/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

#include <WiFi.h>

const char* ssid     = "정상인";
const char* password = "12345678";
int light=100;
WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    ledcAttachPin(13, 0);
    ledcAttachPin(12, 1);
    ledcAttachPin(14, 2);
    ledcSetup(0, 5000, 8);
    ledcSetup(1, 5000, 8);
    ledcSetup(2, 5000, 8);
    /*pinMode(13,OUTPUT);
    pinMode(12,OUTPUT);
    pinMode(14,OUTPUT);*/
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}

int value = 0;
int LED_numder=0;

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("LED_R <a href=\"/A\">on</a> <br>");
            client.print("LED_R <a href=\"/B\">off</a> <br>");
            client.print("LED_B <a href=\"/C\">on</a> <br>");
            client.print("LED_B <a href=\"/D\">off</a> <br>");
            client.print("LED_G <a href=\"/E\">on</a> <br>");
            client.print("LED_G <a href=\"/F\">off</a> <br>");
            client.print("light <a href=\"/H\">UP</a> <br>");
            client.print("light <a href=\"/L\">DOWN</a> <br>");
            client.print("light : ");
            client.print(light);

            // The HTTP response ends with another blank line:light
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          //digitalWrite(5, HIGH);               // GET /H turns the LED on
          light+=10;
          ledcWrite(LED_numder,light);
          delay(30);
        }
        if (currentLine.endsWith("GET /L")) {
          //digitalWrite(5, LOW);                // GET /L turns the LED off
          light-=10;
          ledcWrite(LED_numder,light);
          delay(30);
        }
        if (currentLine.endsWith("GET /A")) {
          //analogWrite(13, light);
          //digitalWrite(13, HIGH);
          ledcWrite(0,light);
          LED_numder=0;
          }
        if (currentLine.endsWith("GET /B")) {
          //digitalWrite(13, LOW);                // GET /L turns the LED off
          ledcWrite(0,0);
        }
        if (currentLine.endsWith("GET /C")) {
          //analogWrite(12, light);
          //digitalWrite(12, HIGH);
          ledcWrite(1,light);
          LED_numder=1;
          }
        if (currentLine.endsWith("GET /D")) {
          //digitalWrite(12, LOW);                // GET /L turns the LED off
          ledcWrite(1,0);
        }
        if (currentLine.endsWith("GET /E")) {
          //analogWrite(14, light);
          //digitalWrite(14, HIGH);
          ledcWrite(2,light);
          LED_numder=2;
          }
        if (currentLine.endsWith("GET /F")) {
          //digitalWrite(14, LOW);                // GET /L turns the LED off
          ledcWrite(2,0);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
