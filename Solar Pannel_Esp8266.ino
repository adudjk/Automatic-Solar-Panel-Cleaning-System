#include <ESP8266WiFi.h>
#include "ThingSpeak.h"


const char *ssid = "";  // WifI SSID and WPA2 Key
const char *pass = "";

const int N = 5;
int curr_sam[N];
int volt_sam[N];

// Channel Details //
unsigned long channelID = X; // Write Channel ID from ThingSpeak
const char*  WriteapiKey = ""; // Write API Key from ThingSpeak
const char* ReadapiKey = "";  // Raed API Key from ThingSpeak
const int FieldNo = 3;


const int currPin = 14;
const int voltPin = 12;

int count = 0;

WiFiClient client;


void setup() 
{
  Serial.begin(9600);  //Initialize serial
  delay(1000);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}
 
void loop() 
{      
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass); 
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("WiFi connected"); 
  }


  int i=0;
  float solarCurr = 0;
  float solarVolt = 0;
  for (i = 0; i < N; i++) 
  {
                curr_sam[i] = analogRead(currPin);
                volt_sam[i] = analogRead(voltPin);
                delay(10);
  }

  float curr_avg = 0;
  float volt_avg = 0;
  for (i = 0; i < N; i++) 
  {
                curr_avg += curr_sam[i];
                volt_avg += volt_sam[i];

  }

  curr_avg /= N;
  volt_avg /= N;

  //Cconvert ADc Value to Voltages to get Actual Current & Voltage
  solarCurr = (curr_avg * 3.3 )/(4095);
  solarVolt = (volt_avg * 3.3 )/(4095);

  // By Using a Voltage Divider We Step Down the Actual Voltage.
  // For that reason we multiply the 6 with avg voltage to get the actual voltage of the solar panel.
  solarVolt *= 6;

  float solarWatt = solarVolt*solarCurr;
  float solarEffy = solarWatt/(0.46*0.34*128);
  
  if (count >= 15 ) 
  {
    count = 0;
    Serial.println("------------------------------------------------------------------");
    Serial.print("Solar Voltage = ");
    Serial.println(solarVolt);
    Serial.print("Solar Current = ");
    Serial.println(solarCurr);
    Serial.print("Solar Power = ");
    Serial.println(solarWatt);
    Serial.print("Solar Efficiency = ");
    Serial.println(solarEffy);
    Serial.println("------------------------------------------------------------------");
    if (WiFi.status() == WL_CONNECTED) 
    {
      ThingSpeak.setField(1, solarVolt);
      ThingSpeak.setField(2, solarCurr);
      ThingSpeak.setField(3, solarWatt);
      ThingSpeak.setField(4, solarEffy);
      // write to the ThingSpeak channel
      int x = ThingSpeak.writeFields(channelID, WriteapiKey);
      if (x == 200) 
      {
        Serial.println("Channels update successful.");
      }
      else 
      {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
    } 
    else 
    {
      Serial.println("\r\n############################################################");
      Serial.println("Failed to update Data to ThingSpeak Server. ");
      Serial.println("WiFi Not Connected...");
      Serial.println("############################################################\r\n");
    }
    Serial.print("Data Reading.");
  }
}
