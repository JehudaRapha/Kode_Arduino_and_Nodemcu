void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.print("Hallo");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    char data = Serial.read();
    if (!data)Serial.end();

    if(data == '1'){
      digitalWrite(LED_BUILTIN, HIGH);
    }

    if (data == '2') {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}
