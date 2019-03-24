#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "<wifi name>"
#define STAPSK  "<wifi password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

int redPin = 13;
int greenPin = 12;
int bluePin = 14;

int redValue = 0;
int greenValue = 0;
int blueValue = 0;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  setColor();
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());
  
  
  Serial.println("Set up completed");
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  // Match request
  bool isValidRequest = false;
  
  int redIndexer = req.indexOf(F("/red/"));
  if (redIndexer != -1)
  {
    redValue = req.substring(redIndexer + 5).toInt();
    isValidRequest = true;
    Serial.print("Setting red: ");
    Serial.println(redValue);
  }

  int greenIndexer = req.indexOf(F("/green/"));
  if (greenIndexer != -1)
  {
    greenValue = req.substring(greenIndexer + 7).toInt();  
    isValidRequest = true;
    Serial.print("Setting green: ");
    Serial.println(greenValue);
  }

  int blueIndexer = req.indexOf(F("/blue/"));
  if (blueIndexer != -1)
  {
    blueValue = req.substring(blueIndexer + 6).toInt();  
    isValidRequest = true;
    Serial.print("Setting blue: ");
    Serial.println(blueValue);
  }

  if (!isValidRequest)
  {
    Serial.println(F("invalid request"));  
  }

  setColor();
  

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }
  
  String red = String(redValue);
  String green = String(greenValue);
  String blue = String(blueValue);

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nRGB is set to: "));
  client.print(red + " " + green + " " + blue);

  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  Serial.println(F("Disconnecting from client"));
}

void setColor()
{
  Serial.println("Setting color");
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);  
}
