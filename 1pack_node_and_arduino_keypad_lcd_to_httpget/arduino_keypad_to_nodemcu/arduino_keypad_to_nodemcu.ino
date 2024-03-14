#include <SoftwareSerial.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h> // Library for I2C LCD

SoftwareSerial myserial(12, 13); // RX TX
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns, 2 rows

bool isNumberEntered = false; // Variable to track whether a number is entered
unsigned long lastKeyPressTime = 0;
bool isScreenCleared = false;

void setup() {
  Serial.begin(9600);
  myserial.begin(9600);
  lcd.init(); // Initialize LCD
  lcd.backlight(); // Turn on backlight
  lcd.setCursor(0, 0);
  lcd.print("Hallo Silahkan");
  lcd.setCursor(0, 1);
  lcd.print("Isi nomer Telp");
}

const byte ROWS = 4; // 4 rows
const byte COLS = 4; // 4 columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte colPins[COLS] = {5, 4, 3, 2}; // pin 2,3,4,5 for keypad column pin (refer to the diagram)
byte rowPins[ROWS] = {9, 8, 7, 6}; // pin 6,7,8,9 for keypad row pin (refer to the diagram)
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

#define MAX_INPUT_LENGTH 20 // Maximum input length

char buffer[MAX_INPUT_LENGTH]; // Buffer to store input
int bufferIndex = 0; // Index to track buffer position

void sendBufferContent() {
  Serial.println(); // Move to next line
  Serial.print("Input data: ");
  myserial.write(buffer, bufferIndex); // Send the entire buffer content as a string

  // Display on LCD
  lcd.clear(); // Clear LCD
  lcd.setCursor(0, 0); // Set cursor to first column of first row
  if (isNumberEntered) {
    lcd.print("Nomer Anda: "); // Update display message
  }
  lcd.setCursor(0, 1); // Set cursor to first column of second row
  lcd.print(buffer);
  lcd.backlight(); // Turn on backlight

  Serial.println(); // Move to next line
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    lastKeyPressTime = millis();

    if (!isNumberEntered) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Nomer Anda: ");
      isNumberEntered = true;
      isScreenCleared = false; // Reset the screen clearing flag
    }

    if (key != '#' && key != '*') {
      buffer[bufferIndex++] = key;
      Serial.print(key);

      lcd.setCursor(bufferIndex - 1, 1);
      lcd.print(" ");

      lcd.setCursor(bufferIndex - 1, 1);
      lcd.print(key);

      if (bufferIndex >= MAX_INPUT_LENGTH) {
        sendBufferContent();
        bufferIndex = 0;
        memset(buffer, 0, sizeof(buffer));
      }
    } else if (key == '*') { // Asterisk key is pressed (backspace)
      if (bufferIndex > 0) {
        bufferIndex--;
        buffer[bufferIndex] = '\0'; // Null terminate the removed character

        // Clear the character from LCD
        lcd.setCursor(bufferIndex, 1);
        lcd.print(" ");
      }
    } else { // Pound key is pressed
      sendBufferContent();
      bufferIndex = 0;
      memset(buffer, 0, sizeof(buffer));
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Nomer Anda");
      lcd.setCursor(0, 1);
      lcd.print("Telah tersimpan");
      delay(5000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hallo Silahkan");
      lcd.setCursor(0, 1);
      lcd.print("Isi nomer Telp");
      isNumberEntered = false;
      isScreenCleared = false; // Reset the screen clearing flag
    }
  }

  // Check if no key has been pressed for 10 seconds
  // Check if no key has been pressed for 10 seconds
  if (millis() - lastKeyPressTime > 5000 && !isNumberEntered && !isScreenCleared) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hallo Silahkan");
    lcd.setCursor(0, 1);
    lcd.print("Isi nomer Telp");
    isScreenCleared = true; // Set the screen clearing flag
  }

}
