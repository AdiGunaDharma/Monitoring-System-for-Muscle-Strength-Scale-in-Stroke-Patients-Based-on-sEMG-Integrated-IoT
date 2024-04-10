#include <WiFi.h>
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Budi Pratomo 2"
#define WIFI_PASSWORD "budilandi123"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCdM741zIRuHLY81RRoJUimC2orMdg11I4"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://mossing-test-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

int pushCount=0;
String sendData;


//Tambahan
float SensorData, KalmanFilterData;
float Xt, Xt_update, Xt_prev;
float Pt, Pt_update, Pt_prev;
float Kt, R, Q;




void setup() {
  // put your setup code here, to run once:
// put your setup code here, to run once:
  Serial.begin(115200);

   R=100; Q=1; Pt_prev=1;

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);




}

void loop() {

SensorData = analogRead(34);
 Xt_update = Xt_prev;
 Pt_update = Pt_prev + Q;
 Kt = Pt_update / (Pt_update + R);
 Xt = Xt_update + ( Kt * (SensorData - Xt_update));
 Pt = (1 - Kt) * Pt_update;
 Xt_prev = Xt;
 Pt_prev = Pt;
 KalmanFilterData=Xt;
 /*Serial.print(SensorData,3);
 Serial.print(",");
 Serial.print(KalmanFilterData,3);
 Serial.println();*/

 float voltage = (((KalmanFilterData/4095.0)*3.3)*1000.0);
 float amp = map(voltage, 0, 3300, 0*100, 10*100)/100.0;
 float Famp = mapPecahan(voltage, 0, 3300, 0, 10);
 Serial.println(Famp);
 delayMicroseconds(100);




sendData = String(Famp) + "-" + String(pushCount);




  // put your main code here, to run repeatedly:
if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int



    if (Firebase.RTDB.pushString(&fbdo, "/Muscle Voltage", sendData)){
      Serial.println();
      Serial.print(sendData);
      Serial.println("  - successfully saved to: " + fbdo.dataPath());
      Serial.println("( " + fbdo.dataType() + ")");
        pushCount++;
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }




    count++;
    
    
    }

}

float mapPecahan(long x, long fromLow, long fromHigh, float toLow, float toHigh)
{
  return (x - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}