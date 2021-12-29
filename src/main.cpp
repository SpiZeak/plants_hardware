#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <WiFiManager.h>
#include "credentials.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Define the Firebase Data object
FirebaseData fbdo;

// Define the FirebaseAuth data for authentication data
FirebaseAuth auth;

// Define the FirebaseConfig data for config data
FirebaseConfig config;

const int sensePin = 35;
int moistureValue = 0;

void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable   detector

  WiFiManager wifiManager;
  wifiManager.setCountry("SE");

  esp_sleep_enable_timer_wakeup(3600000000);

  Serial.begin(115200);
  Serial.println("Conecting.....");
  wifiManager.autoConnect("Plant Sensor");
  Serial.println("Connected");

  // Assign the project host and api key
  config.host = FIREBASE_HOST;
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /** Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;

  /** Assign the maximum retry of token generation */
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config.
  Serial.println("Connecting to Firebase");
  Firebase.begin(&config, &auth);

  Serial.print("Waiting for token... ");

  while (Firebase.authTokenInfo().status != token_status_ready)
  {
    Serial.print(".");
    delay(300);
  }

  Serial.println("Done.");

  moistureValue = analogRead(sensePin);

  if (Firebase.RTDB.setInt(&fbdo, "plants/plant_1/sensor", moistureValue))
  {
    Serial.println("Values successfully updated:");
    Serial.println(moistureValue);
  }
  else
  {
    Serial.println("Values failed updating");
    Serial.println("REASON: " + fbdo.errorReason());
  }

  esp_deep_sleep_start();
}

void loop()
{
}
