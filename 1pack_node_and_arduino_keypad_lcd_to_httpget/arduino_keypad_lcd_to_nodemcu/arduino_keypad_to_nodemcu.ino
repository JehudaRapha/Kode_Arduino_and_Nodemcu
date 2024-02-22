#include <SoftwareSerial.h>
SoftwareSerial myserial(12, 13); // RX TX
#include <Keypad.h>

void setup() {
  Serial.begin(9600);
  myserial.begin(9600);
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
  Serial.println(); // Move to next line
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    if (key != '#') {
      buffer[bufferIndex++] = key; // Store key in buffer
      Serial.print(key); // Print the key
    } else {
      sendBufferContent(); // Send the content of the buffer
      // Reset buffer index and clear buffer
      bufferIndex = 0;
      memset(buffer, 0, sizeof(buffer));
    }
  }
}
