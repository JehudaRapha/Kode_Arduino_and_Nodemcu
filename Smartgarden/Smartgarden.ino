/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6OWRNWS3C" //merupakan identitas yang diperlukan untuk menghubungkan alat dengan server platform BLYNK
#define BLYNK_TEMPLATE_NAME "IoT Smart Garden"
#define BLYNK_AUTH_TOKEN "n6wrUoEGck8t_fXU50uK_1Fq-a0XYkIf"

#define BLYNK_PRINT Serial


#include <WiFi.h> //memanggil library yang telah di instal sebelumnya
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "naekparulian"; //merupakan pengaturan wifi untuk menghubungkan alat dengan internet
char pass[] = "naek1234";

//deklarasi DHT11
#include <DHT.h> //memanggil library sensor DHT11
#define DHT_SENSOR_PIN 13 //menentukan pin yang dipilih untuk menghubungkan sensor dengan esp32
#define DHT_SENSOR_TYPE DHT11 //deklarasi bahwa kita akan menggunakan library DHT11
DHT dht_sensor(DHT_SENSOR_PIN,DHT_SENSOR_TYPE);
float kelembaban, temperatur; //sebuah variable untuk menyimpan nilai kelembaban dan temperatur

void DHTsensor(){
kelembaban = dht_sensor.readHumidity(); //membaca nilai kelembaban
temperatur = dht_sensor.readTemperature(); //membaca nilati temperatur
Blynk.virtualWrite(V1, kelembaban); //nilai dari variable kelembaban akan dikirim ke V1 pada BLYNK untuk ditampilkan pada aplikasi
Blynk.virtualWrite(V2, temperatur); //nilai dari variable temperatur akan dikirim ke V2 pada BLYNK untuk ditampilkan pada aplikasi
    if (isnan(kelembaban) || isnan(temperatur)){ 
     Serial.println("Sensor tidak berfungsi!"); //merupakan fungsi jika kelembaban dan temperatur tidak terbaca
    } else {
      Serial.print("Kelembaban : ");  //merupakan fungsi untuk menunjukkan kelembaban 
      Serial.print(kelembaban);
      Serial.println("%");

      Serial.print("Temperatur : ");   //merupakan fungsi untuk menunjukkan temperatur
      Serial.print(temperatur);
      Serial.println("Derajat Celcius");
    } 
delay(2000);      
}


//deklarasi moisture sensor
const int sensorPin = 34; //menunjukkan sensor berada pada pin berapa
int sensorValue = 0; //sebagai variable untuk menetapkan nilai awal sensor moisture
int svalue;
const int kering = 3030; //sebagai indikator sensor kering
const int basah = 1000; //sebagai indikator sensor basah
void bacasensor(){
  sensorValue = analogRead(sensorPin); //variabel sensorValue akan membaca sinyal analog dari sensor
  svalue = map(sensorValue, kering, basah, 0, 100); //merupakan fungsi untuk mengubah nilai 3030-1000 menjadi 0-100%
  if (svalue < 0){
    svalue = 0;   
  } else if (svalue > 100){
    svalue = 100;        
  }
  Blynk.virtualWrite(V0, svalue); //mengirim nilai 0-100% ke BLYNK melalui V0
  Serial.print("Nilai dari sensor : "); //untuk mencetak teks "Nilai dari sensor : "
  Serial.println(svalue); //untuk mencetak nilai yang didapat dari sinyal analog sensor
  //Serial.println(sensorValue);
  delay(2000); //untuk memberikan delay dalam mencetak
}

BLYNK_WRITE(V3){ //sebagai coding relay, yang berfungsi untuk menyalakan atau mematikan relay
  if (param.asInt()==HIGH){
  digitalWrite(27, LOW); //menunjukkan bahwa relay terhubung pada pin 27 pada ESP32
  } else {
    digitalWrite(27, HIGH);
  }
}

BLYNK_WRITE(V4){ //sebagai coding relay, yang berfungsi untuk menyalakan atau mematikan relay
  if (param.asInt()==HIGH){
  digitalWrite(12, LOW); //menunjukkan bahwa relay terhubung pada pin 12 pada ESP32
  } else {
    digitalWrite(12, HIGH);
  }
}

BLYNK_WRITE(V5){ //sebagai coding relay, yang berfungsi untuk menyalakan atau mematikan relay
  if (param.asInt()==HIGH){ 
  digitalWrite(26, LOW); //menunjukkan bahwa relay terhubung pada pin 26 pada ESP32
  } else {
    digitalWrite(26, HIGH);
  }
}

void setup()
{
  // memanggil semua variable yang telah dibuat agar ketika alat menyala semua akan berfungsi
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht_sensor.begin();
  pinMode(27, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(26, OUTPUT);
}

void loop() //merupakan perulangan untuk membaca nilai pada setiap sensor
{
  Blynk.run();
  DHTsensor();
  bacasensor();
  delay(1000);
}
