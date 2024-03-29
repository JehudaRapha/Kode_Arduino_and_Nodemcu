#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(12, 13); // RX, TX

// Inisialisasi objek LCD dengan alamat I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buttonPin = 2; // Pin untuk tombol pendorong
const int buttonPin2 = 3; // Pin untuk tombol pendorong
const int buttonPin3 = 4;

const int relayPin1 = 5; // Pin untuk relay 1
const int relayPin2 = 6; // Pin untuk relay 2
const int relayPin3 = 7; // Pin untuk relay 3
const int relayPin4 = 8; // Pin untuk relay 4

#define trigPin 11
#define echoPin 10

const int buzzerPin = 9; // Pin untuk buzzer
int buzzerVolume = 50; // Volume default buzzer (0 - 255)

int lastButtonState = LOW;
unsigned long buttonPressTime = 0;
unsigned long lastRefreshTime = 0;
bool relay1State = false;
bool relay2State = false;
bool relay3State = false;
bool relay4State = false;

enum MenuState {SELAMAT_DATANG, MENU, ES_JERUK, ES_COKLAT, ES_CINCAU, ES_KOPI}; // Menandai berbagai status tampilan
MenuState menuState = SELAMAT_DATANG; // Menandai status tampilan saat ini

void setup() {
  mySerial.begin(9600);
  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();

  // Print pesan selamat datang di posisi (1, 0)
  lcd.setCursor(1, 0);
  lcd.print("Selamat Datang");
  // Print pesan pilih menu di posisi (0, 3)
  lcd.setCursor(3, 1);
  lcd.print("Pilih Menu");

  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP); // Atur pin tombol sebagai input dengan resistor pull-up internal
  pinMode(buttonPin2, INPUT_PULLUP); // Atur pin tombol kedua sebagai input dengan resistor pull-up internal
  pinMode(buttonPin3, INPUT_PULLUP);

  // Atur pin relay sebagai output
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);

  // Matikan relay saat booting
  digitalWrite(relayPin1, HIGH);
  digitalWrite(relayPin2, HIGH);
  digitalWrite(relayPin3, HIGH);
  digitalWrite(relayPin4, HIGH);

  // Atur pin buzzer sebagai output
  pinMode(buzzerPin, OUTPUT);

  // Atur pin sensor ultrasonik
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Baca jarak dari sensor ultrasonik
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  int duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2; // Menghitung jarak dalam centimeter

  if (distance < 30) {
    // Baca status tombol
    int buttonState = digitalRead(buttonPin);
    int buttonState2 = digitalRead(buttonPin2);
    int buttonState3 = digitalRead(buttonPin3); // Baca status tombol ketiga

    // Refresh setiap 10 detik
    if (millis() - lastRefreshTime > 20000) {
      lastRefreshTime = millis();
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Selamat Datang");
      lcd.setCursor(3, 1);
      lcd.print("Pilih Menu");
      menuState = SELAMAT_DATANG; // Mengembalikan menuState ke SELAMAT_DATANG
      Serial.println("Selamat Datang");
      Serial.println("Pilih Menu");
    }

    // Cek tombol 1 (Next)
    if (buttonState == LOW) {
      Serial.println("Next Menu Selanjutnya");
      // Mengatur besar suara buzzer
      analogWrite(buzzerPin, buzzerVolume);
      // Membunyikan buzzer selama 200ms
      delay(200);
      // Mematikan buzzer
      analogWrite(buzzerPin, 0);

      switch (menuState) {
        case SELAMAT_DATANG:
          lcd.clear(); // Hapus tampilan sebelumnya
          lcd.setCursor(5, 0);
          lcd.print("Menu :");
          lcd.setCursor(4, 1);
          lcd.print("Es Jeruk");
          Serial.println("Menu : Es Jeruk");
          menuState = ES_JERUK;
          break;
        case MENU:
          lcd.clear(); // Hapus tampilan sebelumnya
          lcd.setCursor(4, 1);
          if (lcd.print("Es Coklat")) {
            Serial.println("Menu : Es Coklat");
            menuState = ES_COKLAT;
          } else {
            Serial.println("Menu : Es Jeruk");
            menuState = ES_JERUK;
          }
          break;
        case ES_JERUK:
          lcd.clear(); // Hapus tampilan sebelumnya
          lcd.setCursor(5, 0);
          lcd.print("Menu : ");
          lcd.setCursor(4, 1);
          lcd.print("Es Coklat");
          Serial.println("Menu : Es Coklat");
          menuState = ES_COKLAT;
          break;
        case ES_COKLAT:
          lcd.clear(); // Hapus tampilan sebelumnya
          lcd.setCursor(5, 0);
          lcd.print("Menu :");
          lcd.setCursor(4, 1);
          lcd.print("Es Cincau");
          Serial.println("Menu : Es Cincau");
          menuState = ES_CINCAU; // Perubahan status menu menjadi ES_CINCAU
          break;
        case ES_CINCAU:
          lcd.clear(); // Hapus tampilan sebelumnya
          lcd.setCursor(5, 0);
          lcd.print("Menu :");
          lcd.setCursor(4, 1);
          lcd.print("Es Kopi");
          Serial.println("Menu : Es Kopi");
          menuState = ES_KOPI; // Perubahan status menu menjadi ES_KOPI
          break;
        case ES_KOPI:
          lcd.clear(); // Hapus tampilan sebelumnya
          lcd.setCursor(5, 0);
          lcd.print("Menu :");
          lcd.setCursor(4, 1);
          lcd.print("Es Jeruk");
          Serial.println("Menu : Es Jeruk");
          menuState = ES_JERUK; // Kembali ke menu pertama setelah menu terakhir
          break;
      }
    }

    // Cek tombol 2 (Back)
    if (buttonState2 == LOW) {
      Serial.println("Menu Sebelumnya");
      // Mengatur besar suara buzzer
      analogWrite(buzzerPin, buzzerVolume);
      // Membunyikan buzzer selama 200ms
      delay(200);
      // Mematikan buzzer
      analogWrite(buzzerPin, 0);

      switch (menuState) {
        case ES_JERUK:
          lcd.clear(); // Hapus tampilan sebelumnya
          lcd.setCursor(1, 0);
          lcd.print("Selamat Datang");
          lcd.setCursor(3, 1);
          lcd.print("Pilih Menu");
          Serial.println("Selamat Datang");
          Serial.println("Pilih Menu");
          menuState = SELAMAT_DATANG;
          break;
        case ES_COKLAT:
          lcd.clear(); // Hapus tampilan sebelumnya
          lcd.setCursor(5, 0);
          lcd.print("Menu :");
          lcd.setCursor(4, 1);
          lcd.print("Es Jeruk");
          Serial.println("Menu : Es Jeruk");
          menuState = ES_JERUK;
          break;
        case ES_CINCAU:
          lcd.clear(); // Hapus tampilan sebelumnya
          lcd.setCursor(5, 0);
          lcd.print("Menu :");
          lcd.setCursor(4, 1);
          lcd.print("Es Coklat");
          Serial.println("Menu : Es Coklat");
          menuState = ES_COKLAT;
          break;
        case ES_KOPI:
          lcd.clear(); // Hapus tampilan sebelumnya
          lcd.setCursor(5, 0);
          lcd.print("Menu :");
          lcd.setCursor(4, 1);
          lcd.print("Es Cincau");
          Serial.println("Menu : Es Cincau");
          menuState = ES_CINCAU;
          break;
      }
    }

    // Cek tombol 3
    if (buttonState3 == LOW) {
      Serial.println("Terpilih");
      // Mengatur besar suara buzzer
      analogWrite(buzzerPin, buzzerVolume);
      // Membunyikan buzzer selama 200ms
      delay(200);
      // Mematikan buzzer
      analogWrite(buzzerPin, 0);

      // Mengaktifkan relay sesuai dengan menu yang dipilih
      switch (menuState) {
        case ES_JERUK:
          // Menampilkan pesan sedang diproses dan harap tunggu
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Sedang Diproses");
          lcd.setCursor(2, 1);
          lcd.print("Harap Tunggu");

          delay(2000); // Menunggu 1000 ms sebelum menampilkan pesan berikutnya

          // Menampilkan pesan pesanan berhasil
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Pesanan Berhasil");
          lcd.setCursor(2, 1);
          lcd.print("Terimakasih");

          delay(100);

          // Mengaktifkan relay untuk Es Jeruk
          digitalWrite(relayPin1, LOW);
          delay(1000); // Menunggu 1000 ms
          digitalWrite(relayPin1, HIGH); // Mematikan relay setelah 1000 ms

          delay(3000); // Menunggu 3000 ms (3 detik) sebelum kembali ke tampilan awal

          // Mengirim pesan melalui komunikasi serial
          mySerial.println("Es Jeruk Telah Terjual");
          Serial.println("Es Jeruk Telah Terjual"); // Tampilkan pesan di Serial Monitor


          // Kembali ke tampilan awal
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("Selamat Datang");
          lcd.setCursor(3, 1);
          lcd.print("Pilih Menu");
          menuState = SELAMAT_DATANG; // Mengembalikan menuState ke SELAMAT_DATANG
          Serial.println("Selamat Datang");
          Serial.println("Pilih Menu");
          break;


        case ES_COKLAT:
          // Menampilkan pesan sedang diproses dan harap tunggu
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Sedang Diproses");
          lcd.setCursor(2, 1);
          lcd.print("Harap Tunggu");

          delay(2000); // Menunggu 1000 ms sebelum menampilkan pesan berikutnya

          // Menampilkan pesan pesanan berhasil
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Pesanan Berhasil");
          lcd.setCursor(2, 1);
          lcd.print("Terimakasih");

          delay(100);

          // Mengaktifkan relay untuk Es Jeruk
          digitalWrite(relayPin2, LOW);
          delay(1000); // Menunggu 1000 ms
          digitalWrite(relayPin2, HIGH); // Mematikan relay setelah 1000 ms

          delay(3000); // Menunggu 3000 ms (3 detik) sebelum kembali ke tampilan awal

          mySerial.println("Es Coklat Telah Terjual");
          Serial.println("Es Coklat Telah Terjual"); // Tampilkan pesan di Serial Monitor

          // Kembali ke tampilan awal
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("Selamat Datang");
          lcd.setCursor(3, 1);
          lcd.print("Pilih Menu");
          menuState = SELAMAT_DATANG; // Mengembalikan menuState ke SELAMAT_DATANG
          Serial.println("Selamat Datang");
          Serial.println("Pilih Menu");
          break;
        case ES_CINCAU:
          // Menampilkan pesan sedang diproses dan harap tunggu
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Sedang Diproses");
          lcd.setCursor(2, 1);
          lcd.print("Harap Tunggu");

          delay(2000); // Menunggu 1000 ms sebelum menampilkan pesan berikutnya

          // Menampilkan pesan pesanan berhasil
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Pesanan Berhasil");
          lcd.setCursor(2, 1);
          lcd.print("Terimakasih");

          delay(100);

          // Mengaktifkan relay untuk Es Jeruk
          digitalWrite(relayPin3, LOW);
          delay(1000); // Menunggu 1000 ms
          digitalWrite(relayPin3, HIGH); // Mematikan relay setelah 1000 ms

          delay(3000); // Menunggu 3000 ms (3 detik) sebelum kembali ke tampilan awal

          mySerial.println("Es Cincau Telah Terjual");
          Serial.println("Es Cincau Telah Terjual"); // Tampilkan pesan di Serial Monitor

          // Kembali ke tampilan awal
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("Selamat Datang");
          lcd.setCursor(3, 1);
          lcd.print("Pilih Menu");
          menuState = SELAMAT_DATANG; // Mengembalikan menuState ke SELAMAT_DATANG
          Serial.println("Selamat Datang");
          Serial.println("Pilih Menu");
          break;
        case ES_KOPI:
          // Menampilkan pesan sedang diproses dan harap tunggu
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Sedang Diproses");
          lcd.setCursor(2, 1);
          lcd.print("Harap Tunggu");

          delay(2000); // Menunggu 1000 ms sebelum menampilkan pesan berikutnya

          // Menampilkan pesan pesanan berhasil
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Pesanan Berhasil");
          lcd.setCursor(2, 1);
          lcd.print("Terimakasih");

          delay(100);

          // Mengaktifkan relay untuk Es Jeruk
          digitalWrite(relayPin4, LOW);
          delay(1000); // Menunggu 1000 ms
          digitalWrite(relayPin4, HIGH); // Mematikan relay setelah 1000 ms

          delay(3000); // Menunggu 3000 ms (3 detik) sebelum kembali ke tampilan awal

          mySerial.println("Es Kopi Telah Terjual");
          Serial.println("Es Kopi Telah Terjual"); // Tampilkan pesan di Serial Monitor

          // Kembali ke tampilan awal
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("Selamat Datang");
          lcd.setCursor(3, 1);
          lcd.print("Pilih Menu");
          menuState = SELAMAT_DATANG; // Mengembalikan menuState ke SELAMAT_DATANG
          Serial.println("Selamat Datang");
          Serial.println("Pilih Menu");
          break;
      }
    }
  }
  else {
    // Jika jarak di atas 30 cm dan tombol masih ditekan, bunyikan buzzer selama 5 detik
    if (digitalRead(buttonPin) == LOW || digitalRead(buttonPin2) == LOW || digitalRead(buttonPin3) == LOW) {
      analogWrite(buzzerPin, buzzerVolume);
      delay(5000); // Bunyikan buzzer selama 5 detik
      analogWrite(buzzerPin, 0); // Matikan buzzer setelah 5 detik
    }
  }
}
