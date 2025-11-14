#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// LCD Pins
#define LCD_PIN_RS 7
#define LCD_PIN_E  6
#define LCD_PIN_DB4 2
#define LCD_PIN_DB5 3
#define LCD_PIN_DB6 4
#define LCD_PIN_DB7 5

// RFID Pins
#define RFID_PIN_SDA 10
#define RFID_PIN_RST 8

// Servo Pin
#define SERVO_PIN 9

// Create objects
LiquidCrystal lcd(LCD_PIN_RS, LCD_PIN_E, LCD_PIN_DB4, LCD_PIN_DB5, LCD_PIN_DB6, LCD_PIN_DB7);
MFRC522 rfid(RFID_PIN_SDA, RFID_PIN_RST);
Servo lockServo;

// Servo positions
const int LOCK_POS = 20;     // locked position
const int OPEN_POS = 150;    // unlocked position

// Put your OWN card UID here (from Serial Monitor)
String authorizedUID = "A1 B2 C3 D4";   // <-- change to your RFID card UID

// Convert RFID UID to string
String readUID() {
    String uidString = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
        uidString += String(rfid.uid.uidByte[i], HEX);
        if (i < rfid.uid.size - 1) uidString += " ";
    }
    uidString.toUpperCase();
    return uidString;
}

void setup() {
    Serial.begin(9600);

    // LCD setup
    lcd.begin(16, 2);
    lcd.print("Locker System");
    delay(1500);
    lcd.clear();

    // RFID setup
    SPI.begin();
    rfid.PCD_Init();
    Serial.println("RFID Ready");

    // Servo setup
    lockServo.attach(SERVO_PIN);
    lockServo.write(LOCK_POS);
    delay(300);
}

void loop() {

    // Look for new RFID card
    if (!rfid.PICC_IsNewCardPresent()) return;
    if (!rfid.PICC_ReadCardSerial()) return;

    // Read UID
    String uid = readUID();
    Serial.print("UID: ");
    Serial.println(uid);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card UID:");
    lcd.setCursor(0, 1);
    lcd.print(uid);

    delay(800);

    // Check authorization
    if (uid == authorizedUID) {
        lcd.clear();
        lcd.print("Access Granted");
        Serial.println("Access Granted");

        // Unlock
        lockServo.write(OPEN_POS);
        delay(2000);

        // Lock again
        lockServo.write(LOCK_POS);
        lcd.setCursor(0, 1);
        lcd.print("Locked");
        delay(1000);
    } 
    else {
        lcd.clear();
        lcd.print("Access Denied");
        Serial.println("Access Denied");
        delay(1500);
    }

    rfid.PICC_HaltA();
}
