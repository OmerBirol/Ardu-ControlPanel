//sıcaklık nem ve ldr değeri gönderen kod 

#include <Wire.h>
#include <WiFi.h>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "thingProperties.h"   
#include "picobricks.h"        

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define SCREEN_ADDRESS 0x3C
SSD1306 OLED(SCREEN_ADDRESS, SCREEN_WIDTH, SCREEN_HEIGHT);
#define PIR_PIN 33
#define LDR_PIN 35

SHTC3 shtc3;

void setup() {
  Serial.begin(115200);
  delay(1500);
  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  Wire.begin();    
  shtc3.begin();   

  // OLED başlat
  OLED.init();
  OLED.clear();
  OLED.setCursor(10, 20);
  OLED.print("Booting...");
  OLED.show();

  // IoT Cloud ayarları
  initProperties();
  onHumChange();
  onBoardTempChange();
  onPirChange();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  // Cloud bağlantısını güncelle
  ArduinoCloud.update();

  // Ölçüme başladığımızı bildir
  

  // 1) LDR oku ve global 'ldr' değişkenine ata
  ldr = analogRead(LDR_PIN);

  // 2) Sıcaklık ve nem oku
  float t = shtc3.readTemperature();
  float h = shtc3.readHumidity();

  // 3) Cloud değişkenlerine ata
  if (!isnan(t) && !isnan(h)) {
    BoardTemp = t;
    Hum       = h;
  } else {
    Serial.println("SHTC3 sensöründen veri okunamadı!");
  }
  pir = digitalRead(PIR_PIN);

  // 4) Seri monitöre yazdır
  Serial.print("LDR: ");
  Serial.print(ldr);
  Serial.print("   T: ");
  Serial.print(BoardTemp, 1);
  Serial.print(" °C   H: ");
  Serial.print(Hum, 1);
  Serial.println(" %");
  Serial.print(" %   PIR: ");
  Serial.println(pir ? "ON" : "OFF");

  // 5) Float değerleri stringe dönüştür (1 ondalık hane)
  char tempStr[8], humStr[8],ldrStr[8];
  sprintf(tempStr, "%.1f", BoardTemp);
  sprintf(humStr,  "%.1f", Hum);
  sprintf(ldrStr,  "%.1f", ldr);
  // 6) OLED’e yazdır
  OLED.clear();

  OLED.setCursor(0,  0);
  OLED.print("LDR: ");
  OLED.print(ldrStr);

  OLED.setCursor(0, 12);
  OLED.print("TEMP:   ");
  OLED.print(tempStr);
  OLED.print(" C");

  OLED.setCursor(0, 24);
  OLED.print("HUM:   ");
  OLED.print(humStr);
  OLED.print(" %");

  OLED.setCursor(0, 36);
  OLED.print("PIR: ");
  OLED.print(pir ? "ON" : "OFF");

  OLED.show();

  delay(2000);
}

