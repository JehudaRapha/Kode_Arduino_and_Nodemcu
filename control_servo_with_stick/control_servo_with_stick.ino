#include <ezButton.h>
#include <Servo.h>

#define VRX_PIN A0 // Arduino pin connected to VRX pin
#define VRY_PIN A1 // Arduino pin connected to VRY pin
#define SW_PIN  8  // Arduino pin connected to SW  pin

ezButton button(SW_PIN);
Servo servoMotor; // Servo object

int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis
int bValue = 0; // To store value of the button

void setup() {
  Serial.begin(9600);
  button.setDebounceTime(50); // set debounce time to 50 milliseconds
  servoMotor.attach(9); // Connect servo to pin 9
}

void loop() {
  button.loop(); // MUST call the loop() function first

  // read analog X and Y analog values
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);

  // Read the button value
  bValue = button.getState();

  if (button.isPressed()) {
    Serial.println("The button is pressed");
    // Reset servo position when the button is pressed
    servoMotor.write(90); // Set servo to its default position (90 degrees)
  }

  // Map joystick values to servo angle (assuming servo range is 0-180 degrees)
  int angle = map(xValue, 0, 1023, 0, 180);

  // Move servo based on joystick value
  servoMotor.write(angle);

  if (button.isReleased()) {
    Serial.println("The button is released");
    // TODO do something here
  }

  // print data to Serial Monitor on Arduino IDE
  Serial.print("x = ");
  Serial.print(xValue);
  Serial.print(", y = ");
  Serial.print(yValue);
  Serial.print(" : button = ");
  Serial.println(bValue);

  delay(20); // Add a small delay for stability
}
