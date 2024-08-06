const int sensor = D3;  // Pin for the magnetic contact switch

int state; // 0 close - 1 open switch

void setup()
{
    pinMode(sensor, INPUT_PULLUP);
    Serial.begin(115200);  // Initialize Serial communication at 115200 baud rate
}

void loop()
{
    state = digitalRead(sensor);
    
    // Print the state of the sensor to the Serial Monitor
    Serial.print("Sensor State: ");
    Serial.println(state);
    
    delay(200);  // Wait for 200 milliseconds
}
