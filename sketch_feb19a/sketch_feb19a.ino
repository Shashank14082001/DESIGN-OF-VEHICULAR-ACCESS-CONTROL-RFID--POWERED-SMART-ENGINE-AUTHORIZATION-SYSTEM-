#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER 6
#define MOTOR_PIN 7 // Define the pin for motor control

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

// Define the authorized UID
String authorizedUID = "16 08 B9 89";

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.begin(16, 2);
  lcd.print("  RFID CAR ACCESS ");
  delay(2000);
  lcd.clear();
  lcd.print("    ~WELCOME~ ");

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(MOTOR_PIN, OUTPUT); // Set MOTOR_PIN as an output
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Show UID on serial monitor
  Serial.print("UID tag: ");
  String content = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  Serial.println();
  Serial.print("Message: ");
  content.toUpperCase(); // Convert content to uppercase
  Serial.println(content);

  lcd.clear();

  // Compare the content with the authorized UID
  Serial.print("Comparing content with authorized UID...");
  Serial.println(content);
  if (content == authorizedUID) {
    lcd.setCursor(0, 0);
    lcd.print("License APPROVED    ");
    lcd.setCursor(0, 1);
    lcd.print("Access Granted     ");

    Serial.println("Authorized access");
    tone(BUZZER, 1000); // Sound a tone for approved access
    digitalWrite(MOTOR_PIN, HIGH); // Turn on the motor
    delay(2000); // Adjust this delay as needed
    digitalWrite(MOTOR_PIN, LOW); // Turn off the motor
    noTone(BUZZER);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Unauthorized access");
    lcd.setCursor(0, 1);
    lcd.print("Access Denied      ");

    Serial.println("Access denied");
    tone(BUZZER, 300); // Sound a different tone for denied access
    delay(2000); // Adjust this delay as needed
    noTone(BUZZER);
  }

  delay(3000); // Add a delay before the next card can be read
}
