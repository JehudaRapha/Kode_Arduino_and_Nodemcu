#include <SoftwareSerial.h>
SoftwareSerial myserial(D1,D2);
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  myserial.begin(115200);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}
String getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;
    
    // Iterate over the characters in the string
    for (int i = 0; i <= maxIndex; i++) {
        // If the current character is the separator
        if (data.charAt(i) == separator) {
            found++;
            // If we have found the desired index
            if (found == index) {
                strIndex[1] = i; // Set the end index of the substring
                break; // Exit the loop
            }
            strIndex[0] = i + 1; // Set the start index of the next substring
        }
    }
    // If the index was found and it's not the last character
    if (found > 0 && strIndex[1] < maxIndex) {
        // Return the substring from the last separator to the end of the string
        return data.substring(strIndex[0], strIndex[1]);
    } else {
        // Return an empty string if the index was not found or it's the last character
        return "";
    }
}


void loop() { // run over and over
  if (myserial.available()) {
    String data = myserial.readStringUntil('\n');
    String node1 = getValue(data,'|',0);//sensor 1
    String node2 = getValue(data,'|',1);//sensor 2
    Serial.print("Data Full : ");
    Serial.println(data);
    Serial.print("data sensor 1 : ");
    Serial.println(node1);
    Serial.print("data sensor 2 : ");
    Serial.println(node2);
  }
}
