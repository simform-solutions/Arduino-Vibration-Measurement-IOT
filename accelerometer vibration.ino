#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <PubSubClient.h>
#include "SoftwareSerial.h"

#define WIFI_AP "IOT"
#define WIFI_PASSWORD "87654321"

#define TOKEN "7Xhzm313Me12TCy9FpT0"

char thingsboardServer[] = "demo.thingsboard.io";

// Initialize the Ethernet client object
WiFiEspClient espClient;

PubSubClient client(espClient);

SoftwareSerial soft(2, 3); // RX, TX

int status = WL_IDLE_STATUS;
unsigned long lastSend;

// Accelerometer pin description
const int xpin = A0; // x-axis
const int ypin = A1; // y-axis
const int zpin = A2; // z-axis

float Xval, Yval, Zval; // acceleration in each axis
float Anet; // Net acceleration 
void setup()
 {
 Serial.begin(9600); 
  // initialize the wifi
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  lastSend = 0;


 }
 
void loop() {
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(500);
    }
    Serial.println("Connected to AP");
  }

  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getaccelarate();
    lastSend = millis();
  }

  client.loop();
}

void getaccelarate(){

  // get accelerometer xyz axes data
 Xval = analogRead(xpin);
 Yval = analogRead(ypin);
 Zval = analogRead(zpin);
 // compute net accelaration
 Anet = sqrt(sq(Xval) + sq(Yval) + sq(Zval));
 Serial.println(Anet);

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"Anet\":"; payload += Anet; 
  payload += "}";

  // Send payload to cloud
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );

 delay(200); // Delay between passing values
  
  }


void InitWiFi()
{
  // initialize serial for ESP module
  soft.begin(9600);
  // initialize ESP module
  WiFi.init(&soft);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_AP);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(500);
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("Arduino Uno Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

