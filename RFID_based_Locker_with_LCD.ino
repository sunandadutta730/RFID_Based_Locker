#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// RFID setup
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

// I2C LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD I2C address to 0x27 (may vary)

// Pins
#define LED_G 5
#define LED_R 4
#define RELAY 3
#define BUZZER 2

// Timing
#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000

void setup() {
  Serial.begin(9600);
  SPI.begin();           
  mfrc522.PCD_Init();    
  
  // LCD init
  lcd.init();            
  lcd.backlight();       
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");

  // I/O init
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  digitalWrite(RELAY, HIGH);
}

void loop() {
  // Look for new card
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select card
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID Tag : ");
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card Detected");

  if (content.substring(1) == "XX XX XX XX") { // <-- Replace with actual UID
    Serial.println("Authorized access");
    lcd.setCursor(0, 1);
    lcd.print("Access Granted");

    digitalWrite(RELAY, HIGH);
    digitalWrite(LED_G, HIGH);
    delay(ACCESS_DELAY);
    digitalWrite(RELAY, LOW);
    digitalWrite(LED_G, LOW);
  } 
  else {
    Serial.println("Access denied");
    lcd.setCursor(0, 1);
    lcd.print("Access Denied");

    digitalWrite(LED_R, HIGH);
    tone(BUZZER, 300);
    delay(DENIED_DELAY);
    digitalWrite(LED_R, LOW);
    noTone(BUZZER);
  }

  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");

  // Stop communication
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
