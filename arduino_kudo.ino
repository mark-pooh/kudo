const char* net_ssid  = "wifi_user";
const char* net_pass  = "wifi_password";
const char* web_host  = "kudobin.000webhostapp.com";
const char* web_bin   = "12345";

#include<ESP8266WiFi.h>

#define TRIGGER 5
#define ECHO    4
#define RLED    13
#define YLED    12
#define GLED    14
#define BUZZ    15

String binIP;

void setup() {
  //Connecting to network
  Serial.begin(115200);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(RLED, OUTPUT);
  pinMode(YLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  delay(10);
  
  /* INFO ONLY: DELIBLE */
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(net_ssid);
  /* END OF INFO ONLY */
  
  WiFi.begin(net_ssid, net_pass);  

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  /* INFO ONLY: DELIBLE */
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  /* END OF INFO ONLY */

  /* GET BIN IP -> REQUEST BIN CAPACITY REMOTELY */
  binIP = WiFi.localIP().toString();

  /* TESTING: DELIBLE */
  Serial.println(binIP);

  delay(5000);

  //Connecting to host
  Serial.print("Connecting to ");
  Serial.println(web_host);
  /* END OF TESTING */
}

void loop() {
  //Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;

  if(!client.connect(web_host, httpPort)) {
    Serial.println("Connection failed!");
    return;
  } else {
    Serial.print("Connected to ");
    Serial.println(web_host);
  }
  
  long duration, distance;
  long min_distance, bin_height, bin_full, bin_notempty, bin_empty;
  int TONE, RDIS, YDIS, GDIS;
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  
  duration = pulseIn(ECHO, HIGH);
  distance = (duration / 2) / 29.1;
  
  Serial.print(distance);
  Serial.println(" cm");

  digitalWrite(GLED, LOW);
  digitalWrite(YLED, LOW);
  digitalWrite(RLED, LOW);

  min_distance  = 3;  /* Approximately minimum distance HC-SR04 can detect */
  bin_height    = 15 - min_distance;  /* Remove unnecessary distance */
  bin_full      = bin_height - ((80/100) * bin_height); /* 80% full */
  bin_notempty  = bin_height - ((50/100) * bin_height); /* 50% full */
  bin_empty     = bin_height - ((20/100) * bin_height); /* 20% full */
  distance      = distance - min_distance;

  if(distance <= 3){
    digitalWrite(GLED, LOW);
    digitalWrite(YLED, LOW);
    digitalWrite(RLED, HIGH);

    analogWrite(BUZZ, 100);
  }
  else if(distance > 3 && distance < 7){
    digitalWrite(GLED, LOW);
    digitalWrite(YLED, HIGH);
    digitalWrite(RLED, LOW); 

    analogWrite(BUZZ, 0);
  }
  else if(distance >= 7){
    digitalWrite(GLED, HIGH);
    digitalWrite(YLED, LOW);
    digitalWrite(RLED, LOW);

    analogWrite(BUZZ, 0);
  }
  /*
  if(distance <= bin_full){
    GDIS = LOW;
    YDIS = LOW;    
    RDIS = HIGH;
    TONE = 100;    
  } else if(distance > bin_full && distance < bin_empty){
    GDIS = LOW;
    YDIS = HIGH;
    RDIS = LOW;
    TONE = 0;
  } else if (distance >= bin_empty) {
    GDIS = HIGH;
    YDIS = LOW;    
    RDIS = LOW;
    TONE = 0;
  }

  analogWrite(BUZZ, TONE);
  
  digitalWrite(GLED, GDIS);
  digitalWrite(YLED, YDIS);
  digitalWrite(RLED, RDIS);  */

  String url = "/insert.php?bCapacity=";
    url += distance;
    url += "&bHeight=";
    url += bin_height;
    url += "&web_bin=";
    url += web_bin;
    url += "&binIP=";
    url += binIP;

  Serial.print("Requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                "Host: " + web_host + "\r\n" +
                "Connection:  close\r\n\r\n");
    
  unsigned long timeout = millis();

  while(client.available() == 0) {
    if(millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  /*
  delay(60UL * 60UL * 1000UL);  //60 minutes each of 60 seconds each of 1000 milliseconds all unsigned longs

  */
  delay(1000); // 1 seconds
}