#include<WiFi.h>
#include<WiFiUdp.h> // Réseau UDP
#include<OSCMessage.h> // Protocole OSC
#include<Wire.h> // Pour le MPU6500

// Pour le Wifi --------------------
const char hotspotName[] = "SWORD";
const char password[] = "password"; // minimum de 8 char

WiFiUDP Udp;
const IPAddress IP_ordinateur(192, 168, 4, 2); // adresse de l'ordi
const int outPort = 8000; // port écouté

// Pour le MPU6500 --------------------
const int MPU_addr=0x68;  // I2C address of the MPU-6050

int16_t AcX,AcY,AcZ; // acceleration X, Y et Z

void setup() 
{
  setup_WiFi();
  setup_MPU6500();
}

void loop() 
{
  grab_MPU6500_data();
  sendData();
  delay(50);
}

void setup_WiFi() // Initialize le Wifi
{
  WiFi.softAP(hotspotName, password); // Création du hotspot
  Udp.begin(outPort); // Start UDP
}

void setup_MPU6500() // Initialize le MPU6500
{
  Wire.begin(21,22);
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void grab_MPU6500_data() // Collecte les valeurs MPU6500
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
}

void sendData() // Envoie les données en OSC
{
  // OSC Accel X --------------------
  OSCMessage msgAccelX("/accelX");
  msgAccelX.add(AcX); // Message "/accelX <int>"

  Udp.beginPacket(IP_ordinateur, outPort);
  msgAccelX.send(Udp);
  Udp.endPacket();
  msgAccelX.empty(); // On vide l'espace en mémoire

  // OSC Accel Y --------------------
  OSCMessage msgAccelY("/accelY");
  msgAccelY.add(AcY); // Message "/accelY <int>"

  Udp.beginPacket(IP_ordinateur, outPort);
  msgAccelY.send(Udp);
  Udp.endPacket();
  msgAccelY.empty(); // On vide l'espace en mémoire

  // OSC Accel Z --------------------
  OSCMessage msgAccelZ("/accelZ");
  msgAccelZ.add(AcZ); // Message "/accelY <int>"

  Udp.beginPacket(IP_ordinateur, outPort);
  msgAccelZ.send(Udp);
  Udp.endPacket();
  msgAccelZ.empty(); // On vide l'espace en mémoire
}