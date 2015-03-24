/* Setup shield-specific #include statements */
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information


WiFiClient client;

// The number of times to trigger the action if the condition is met.
// We limit this so you won't use all of your Temboo calls while testing.
int maxCalls = 10;

// The number of times this Choreo has been run so far in this sketch.
int calls = 0;
int loopcount = 0;
int sensorPin = A0;            // select the input pin for the ldr
unsigned int sensorValue = 0;  // variable to store the value coming from the ldr

void setup()
{ 
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(2, HIGH);
  digitalWrite(4, HIGH);
  //Start Serial port
  Serial.begin(9600);        // start serial for output - for testing
  
  // For debugging, wait until the serial console is connected.
  delay(4000);
  while(!Serial);

  int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present.
  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    // If there's no WiFi shield, stop here.
    while(true);
  }

  Serial.println("OK");

  // Try to connect to the local WiFi network.
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin(WIFI_SSID, WPA_PASSWORD);

    if (wifiStatus == WL_CONNECTED) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    delay(5000);
  }

  digitalWrite(2, LOW);
  digitalWrite(4, LOW);

  // Initialize pins
  pinMode(A0, INPUT);
  pinMode(6, OUTPUT);
  Serial.println("Setup complete.\n");
}

void loop()
{
  float clocktime;
  String Status;
  // read the value from the ldr:
  sensorValue = analogRead(sensorPin);     
  if(sensorValue<400){
    digitalWrite(4, HIGH);
    Status = "ON ";
    Serial.print (Status);
    Serial.print(sensorValue, DEC);     // print the value (0 to 1024)
    runAppendRow(sensorValue, Status);
  }
  else{
    digitalWrite(4, LOW);   // set the LED on
    Status = "OFF ";
    Serial.print (Status);
    Serial.print(sensorValue, DEC);     // print the value (0 to 1024)
    runAppendRow(sensorValue, Status);
  }
  
  // For DEBUGGING - Print out our data, uncomment the lines below
  Serial.println("");                   // print carriage return  
  delay(20000);  
}


void runAppendRow(int sensorValue, String Status) {
  TembooChoreo AppendRowChoreo(client);
  //digitalWrite(4, HIGH); 

  // Set Temboo account credentials
  AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
  AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);

  // Set profile to use for execution
  AppendRowChoreo.setProfile("arduinoLightSpreadsheet");

  // Set Choreo inputs
  String RowDataValue(sensorValue);
  RowDataValue +=",";
  RowDataValue +=Status;
  
  AppendRowChoreo.addInput("RowData", RowDataValue);


  // Identify the Choreo to run
  AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");

  // Run the Choreo
  unsigned int returnCode = AppendRowChoreo.run();

  // A return code of zero means everything worked
  if (returnCode == 0) {
    Serial.println("Done!\n");
  } else {
    // A non-zero return code means there was an error
    // Read and print the error message
    while (AppendRowChoreo.available()) {
      char c = AppendRowChoreo.read();
      Serial.print(c);
    }
    Serial.println();
  }
  
  AppendRowChoreo.close();
  for(int num = 0; num < 3; num++)
  {
    digitalWrite(2, HIGH);
    delay(250);      
    digitalWrite(2, LOW);
    delay(250);
  }
  
  
}
