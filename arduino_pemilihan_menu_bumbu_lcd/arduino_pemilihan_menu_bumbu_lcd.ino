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

const int buzzerPin = 9; // Pin untuk buzzer
int buzzerVolume = 50; // Volume default buzzer (0 - 255)

int lastButtonState = LOW;
unsigned long buttonPressTime = 0;
unsigned long lastRefreshTime = 0;
bool relay1State = false;
bool relay2State = false;
bool relay3State = false;
bool relay4State = false;

enum MenuState {SELAMAT_DATANG, MENU, BUMBU_PEDAS, BUMBU_MANIS, BUMBU_GURIH}; // Menandai berbagai status tampilan
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
  lcd.setCursor(2, 1);
  lcd.print("Pilih Bumbu");

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
}

// Fungsi untuk menampilkan menu Selamat Datang
void menuSelamatDatang() {
  lcd.clear(); // Hapus tampilan sebelumnya
  lcd.setCursor(1, 0);
  lcd.print("Selamat Datang");
  lcd.setCursor(2, 1);
  lcd.print("Pilih Bumbu");
  Serial.println("Selamat Datang");
  Serial.println("Pilih Bumbu");
  menuState = SELAMAT_DATANG; // Set menuState ke Selamat Datang
}

// Fungsi untuk menampilkan menu Bumbu Pedas
void menuBumbuPedas() {
  lcd.clear(); // Hapus tampilan sebelumnya
  lcd.setCursor(5, 0);
  lcd.print("Menu :");
  lcd.setCursor(2, 1);
  lcd.print("Bumbu Pedas");
  Serial.println("Menu : Bumbu pedas");
  menuState = BUMBU_PEDAS; // Set menuState ke Bumbu Pedas
}

void menuBumbuPedasDone() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bumbu Pedas");
  lcd.setCursor(0, 1);
  lcd.print("Terpilih");

  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kentang Sdng di");
  lcd.setCursor(0, 1);
  lcd.print("Ptong Hrap Tggu");

  delay(5000); // Menunggu 1000 ms sebelum menampilkan pesan berikutnya

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kentang sudah");
  lcd.setCursor(0, 1);
  lcd.print("diPotong");

  delay(5000);

  // Menampilkan pesan pesanan berhasil
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sekarang Sdng di");
  lcd.setCursor(0, 1);
  lcd.print("Goreng Hrap Tggu");

  delay(5000); // Menunggu 3000 ms (3 detik) sebelum kembali ke tampilan awal

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kentang Telah");
  lcd.setCursor(0, 1);
  lcd.print("Selesai diGoreng");

  delay(5000); // Menunggu 3000 ms (3 detik) sebelum kembali ke tampilan awal

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waktu Penaburan");
  lcd.setCursor(0, 1);
  lcd.print("Bumbu Pedas");

  delay(100);

  digitalWrite(relayPin3, LOW);
  delay(5000); // Menunggu 1000 ms
  digitalWrite(relayPin3, HIGH); // Mematikan relay setelah 1000 ms

  delay(100); // Menunggu 3000 ms (3 detik) sebelum kembali ke tampilan awal

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kentang Bumbu");
  lcd.setCursor(0, 1);
  lcd.print("Pedas Siap diMkn");

  delay(6000);

  //  mySerial.println("Bumbu Pedas Telah Terpakai");
  //  Serial.println("Bumbu Pedas Telah Terpakai"); // Tampilkan pesan di Serial Monitor

  // Kembali ke tampilan awal
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Selamat Datang");
  lcd.setCursor(2, 1);
  lcd.print("Pilih Bumbu");
  menuState = SELAMAT_DATANG; // Mengembalikan menuState ke SELAMAT_DATANG
  Serial.println("Selamat Datang");
  Serial.println("Pilih Bumbu");
}

// Fungsi untuk menampilkan menu Bumbu Manis
void menuBumbuManis() {
  lcd.clear(); // Hapus tampilan sebelumnya
  lcd.setCursor(5 , 0);
  lcd.print("Menu :");
  lcd.setCursor(2, 1);
  lcd.print("Bumbu Manis");
  Serial.println("Menu : Bumbu Manis");
  menuState = BUMBU_MANIS; // Set menuState ke Bumbu Manis
}

void menuBumbuManisDone() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bumbu Manis");
  lcd.setCursor(0, 1);
  lcd.print("Terpilih");

  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kentang Sdng di");
  lcd.setCursor(0, 1);
  lcd.print("Ptong Hrap Tggu");

  delay(5000); // Menunggu 1000 ms sebelum menampilkan pesan berikutnya

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kentang sudah");
  lcd.setCursor(0, 1);
  lcd.print("diPotong");

  delay(5000);

  // Menampilkan pesan pesanan berhasil
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sekarang Sdng di");
  lcd.setCursor(0, 1);
  lcd.print("Goreng Hrap Tggu");

  delay(5000); // Menunggu 3000 ms (3 detik) sebelum kembali ke tampilan awal

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kentang Telah");
  lcd.setCursor(0, 1);
  lcd.print("Selesai diGoreng");

  delay(5000); // Menunggu 3000 ms (3 detik) sebelum kembali ke tampilan awal

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waktu Penaburan");
  lcd.setCursor(0, 1);
  lcd.print("Bumbu Manis");

  delay(100);

  digitalWrite(relayPin2, LOW);
  delay(5000); // Menunggu 1000 ms
  digitalWrite(relayPin2, HIGH); // Mematikan relay setelah 1000 ms

  delay(100); // Menunggu 3000 ms (3 detik) sebelum kembali ke tampilan awal

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kentang Bumbu");
  lcd.setCursor(0, 1);
  lcd.print("Manis Siap diMkn");

  delay(6000);

  //  mySerial.println("Bumbu Manis Telah Terpakai");
  //  Serial.println("Bumbu Manis Telah Terpakai"); // Tampilkan pesan di Serial Monitor

  // Kembali ke tampilan awal
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Selamat Datang");
  lcd.setCursor(2, 1);
  lcd.print("Pilih Bumbu");
  menuState = SELAMAT_DATANG; // Mengembalikan menuState ke SELAMAT_DATANG
  Serial.println("Selamat Datang");
  Serial.println("Pilih Bumbu");
}

// Fungsi untuk menampilkan menu Bumbu Gurih
void menuBumbuGurih() {
  lcd.clear(); // Hapus tampilan sebelumnya
  lcd.setCursor(5, 0);
  lcd.print("Menu :");
  lcd.setCursor(2, 1);
  lcd.print("Bumbu Gurih");
  Serial.println("Menu : Bumbu Gurih");
  menuState = BUMBU_GURIH; // Set menuState ke Bumbu Gurih
}

void menuBumbuGurihDone() {
  // Menampilkan pesan sedang diproses dan harap tunggu
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bumbu Gurih");
  lcd.setCursor(0, 1);
  lcd.print("Terpilih");

  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kentang Sdng di");
  lcd.setCursor(0, 1);
  lcd.print("Ptong Hrap Tggu");

  delay(5000); // Menunggu 1000 ms sebelum menampilkan pesan berikutnya

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kentang sudah");
  lcd.setCursor(0, 1);
  lcd.print("diPotong");

  delay(5000);

  // Menampilkan pesan pesanan berhasil
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sekarang Sdng di");
  lcd.setCursor(0, 1);
  lcd.print("Goreng Hrap Tggu");

  delay(5000); // Menunggu 3000 ms (3 detik) sebelum kembali ke tampilan awal

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kentang Telah");
  lcd.setCursor(0, 1);
  lcd.print("Selesai diGoreng");

  delay(5000); // Menunggu 3000 ms (3 detik) sebelum kembali ke tampilan awal

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waktu Penaburan");
  lcd.setCursor(0, 1);
  lcd.print("Bumbu Gurih");

  delay(100);

  digitalWrite(relayPin3, LOW);
  delay(5000); // Menunggu 1000 ms
  digitalWrite(relayPin3, HIGH); // Mematikan relay setelah 1000 ms

  delay(100); // Menunggu 3000 ms (3 detik) sebelum kembali ke tampilan awal

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kentang Bumbu");
  lcd.setCursor(0, 1);
  lcd.print("Gurih Siap diMkn");

  delay(6000);

  //  mySerial.println("Bumbu Gurih Telah Terpakai");
  //  Serial.println("Bumbu Gurih Telah Terpakai"); // Tampilkan pesan di Serial Monitor

  // Kembali ke tampilan awal
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Selamat Datang");
  lcd.setCursor(2, 1);
  lcd.print("Pilih Bumbu");
  menuState = SELAMAT_DATANG; // Mengembalikan menuState ke SELAMAT_DATANG
  Serial.println("Selamat Datang");
  Serial.println("Pilih Bumbu");
}

void loop() {
  // Baca status tombol
  int buttonState = digitalRead(buttonPin);
  int buttonState2 = digitalRead(buttonPin2);
  int buttonState3 = digitalRead(buttonPin3); // Baca status tombol ketiga

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
        menuBumbuPedas(); // Tampilkan menu Bumbu Pedas
        break;
      case MENU:
        // Tambahkan kode untuk menu jika diperlukan
        break;
      case BUMBU_PEDAS:
        menuBumbuManis(); // Tampilkan menu Bumbu Manis
        break;
      case BUMBU_MANIS:
        menuBumbuGurih(); // Tampilkan menu Bumbu Gurih
        break;
      case BUMBU_GURIH:
        menuBumbuPedas(); // Kembali ke menu Bumbu Pedas setelah Bumbu Gurih
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
      case BUMBU_PEDAS:
        menuSelamatDatang(); // Kembali ke menu Selamat Datang dari Bumbu Pedas
        break;
      case BUMBU_MANIS:
        menuBumbuPedas(); // Kembali ke menu Bumbu Pedas dari Bumbu Manis
        break;
      case BUMBU_GURIH:
        menuBumbuManis(); // Kembali ke menu Bumbu Manis dari Bumbu Gurih
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

    // Tambahkan tindakan untuk tombol ketiga di sini
    // Misalnya, Anda dapat menambahkan logika untuk mengaktifkan relay atau melakukan tindakan lainnya
    // Mengaktifkan relay sesuai dengan menu yang dipilih
    switch (menuState) {
      case BUMBU_PEDAS:
        menuBumbuPedasDone();
        break;
      case BUMBU_MANIS:
        menuBumbuManisDone();
        break;
      case BUMBU_GURIH:
        menuBumbuGurihDone();
        break;
    }
  }
}
