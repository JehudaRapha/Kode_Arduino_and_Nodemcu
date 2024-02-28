#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h> // Include the Servo library

// Inisialisasi LCD I2C pada alamat 0x27 dengan 16 kolom dan 2 baris
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

class MenuManager {
  private:
    int menuIndex = 0; // Index untuk navigasi menu
    const int buttonLeft = 8; // Pin tombol kiri
    const int buttonRight = 6; // Pin tombol kanan
    const int buttonSelect = 7; // Pin tombol select
    const int buzzerPin = 9; // Pin untuk buzzer
    int buzzerVolume = 100; // Volume default buzzer (0 - 255)
    const int servoPin = 10;

    // Declare the Servo object
#define relay1 2
#define relay2 3
#define relay3 4
#define relay4 5

  public:
    MenuManager() {
      // Konfigurasi pin tombol sebagai input dengan pull-up internal
      pinMode(buttonLeft, INPUT_PULLUP);
      pinMode(buttonRight, INPUT_PULLUP);
      pinMode(buttonSelect, INPUT_PULLUP);
      pinMode(relay1, OUTPUT);
      pinMode(relay2, OUTPUT);
      pinMode(relay3, OUTPUT);
      pinMode(relay4, OUTPUT);

      pinMode(buzzerPin, OUTPUT);

      digitalWrite(relay1, HIGH);
      digitalWrite(relay2, HIGH);
      digitalWrite(relay3, HIGH);
      digitalWrite(relay4, HIGH);
    }

    int getServoPin() {
      return servoPin;
    }

    void beep() {
      tone(buzzerPin, 1000, buzzerVolume);
      delay(100); // Duration of the beep
      noTone(buzzerPin);
    }

    void update() {
      // Navigasi ke kiri
      if (!digitalRead(buttonLeft)) {
        menuIndex = max(0, menuIndex - 1);
        show();
        beep();
        delay(200); // Anti bouncing
      }
      // Navigasi ke kanan
      else if (!digitalRead(buttonRight)) {
        menuIndex = min(2, menuIndex + 1); // Angka 2 harus diubah sesuai jumlah menu - 1
        show();
        beep();
        delay(200); // Anti bouncing
      }
      // Pilih menu
      else if (!digitalRead(buttonSelect)) {
        execute();
        delay(200); // Anti bouncing
      }
    }

    // Menampilkan menu saat ini
    void show() {
      lcd.clear();
      switch (menuIndex) {
        case 0:
          lcd.print("Bumbu Pedas"); // Menu 1
          break;
        case 1:
          lcd.print("Bumbu Manis"); // Menu 2
          break;
        case 2:
          lcd.print("Bumbu Gurih"); // Menu 3
          break;
      }
    }

    // Eksekusi pilihan menu
    void execute() {
      lcd.clear();
      switch (menuIndex) {
        case 0:
          beep();
          delay(200); // Anti bouncing
          lcd.print("Pedas Selected");
          delay(2000);
          lcd.clear();
          beep();
          delay(200); // Anti bouncing
          lcd.print("Proses Potong");
          lcd.setCursor(0, 1);
          lcd.print("Kentang");
          for (int i = 0; i < 4; ++i) { // Lakukan gerakan servo dua kali
            // Gerakan servo dari 0 derajat hingga 180 derajat
            for (int angle = 0; angle <= 180; angle += 5) {
              myServo.write(angle);
              delay(10); // Delay untuk setiap langkah gerakan
            }

            // Kembalikan servo dari 180 derajat ke 0 derajat
            for (int angle = 180; angle >= 0; angle -= 5) {
              myServo.write(angle);
              delay(10); // Delay untuk setiap langkah gerakan
            }
          }
          lcd.clear();
          beep();
          delay(200); // Anti bouncing
          lcd.print("Potong Kentang");
          lcd.setCursor(0, 1);
          lcd.print("Selesai");
          delay(2000);
          lcd.clear();
          beep();
          delay(200); // Anti bouncing
          lcd.print("Lanjut Goreng");
          lcd.setCursor(0, 1);
          lcd.print("Kentang");
          for (int i = 0; i < 2; ++i) { // Lakukan gerakan servo dua kali
            // Gerakan servo dari 0 derajat hingga 180 derajat
            for (int angle = 0; angle <= 180; angle += 5) {
              myServo.write(angle);
              delay(10); // Delay untuk setiap langkah gerakan
            }

            // Kembalikan servo dari 180 derajat ke 0 derajat
            for (int angle = 180; angle >= 0; angle -= 5) {
              myServo.write(angle);
              delay(10); // Delay untuk setiap langkah gerakan
            }
          }
          lcd.clear();
          beep();
          delay(200); // Anti bouncing
          lcd.print("Goreng Kentang");
          lcd.setCursor(0, 1);
          lcd.print("Selesai");
          delay(2000);
          lcd.clear();
          beep();
          delay(200); // Anti bouncing
          lcd.print("Proses Takar");
          lcd.setCursor(0, 1);
          lcd.print("Bumbu Pedas");
          digitalWrite(relay1, LOW);
          delay(2000);
          digitalWrite(relay1, HIGH);
          lcd.clear();
          beep();
          delay(200); // Anti bouncing
          lcd.print("Kentang Goreng");
          lcd.setCursor(0, 1);
          lcd.print("Siap Dimakan!");
          delay(5000);
          break;
        case 1:
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Manis Selected");
          delay(2000);
          lcd.clear();
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Proses Potong");
          lcd.setCursor(0, 1);
          lcd.print("Kentang");
          delay(2000);
          lcd.clear();
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Potong Kentang");
          lcd.setCursor(0, 1);
          lcd.print("Selesai");
          delay(2000);
          lcd.clear();
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Lanjut Goreng");
          lcd.setCursor(0, 1);
          lcd.print("Kentang");
          delay(2000);
          lcd.clear();
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Goreng Kentang");
          lcd.setCursor(0, 1);
          lcd.print("Selesai");
          delay(2000);
          lcd.clear();
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Proses Takar");
          lcd.setCursor(0, 1);
          lcd.print("Bumbu Manis");
          digitalWrite(relay1, LOW);
          delay(2000);
          digitalWrite(relay1, HIGH);
          lcd.clear();
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Kentang Goreng");
          lcd.setCursor(0, 1);
          lcd.print("Siap Dimakan!");
          delay(5000);
          break;
        case 2:
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Gurih Selected");
          delay(2000);
          lcd.clear();
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Proses Potong");
          lcd.setCursor(0, 1);
          lcd.print("Kentang");
          delay(2000);
          lcd.clear();
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Potong Kentang");
          lcd.setCursor(0, 1);
          lcd.print("Selesai");
          delay(2000);
          lcd.clear();
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Lanjut Goreng");
          lcd.setCursor(0, 1);
          lcd.print("Kentang");
          delay(2000);
          lcd.clear();
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Goreng Kentang");
          lcd.setCursor(0, 1);
          lcd.print("Selesai");
          delay(2000);
          lcd.clear();
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Proses Takar");
          lcd.setCursor(0, 1);
          lcd.print("Bumbu Gurih");
          digitalWrite(relay1, LOW);
          delay(2000);
          digitalWrite(relay1, HIGH);
          lcd.clear();
          analogWrite(buzzerPin, buzzerVolume);
          delay(200); // Anti bouncing
          analogWrite(buzzerPin, 0);
          lcd.print("Kentang Goreng");
          lcd.setCursor(0, 1);
          lcd.print("Siap Dimakan!");
          delay(5000);
          break;
      }
      delay(1000); // Tampilkan pilihan untuk sementara waktu
      show(); // Kembali ke tampilan menu
    }
};

MenuManager menuManager;

void setup() {
  lcd.init(); // Inisialisasi LCD
  lcd.backlight(); // Nyalakan backlight
  myServo.attach(menuManager.getServoPin());
  menuManager.show(); // Tampilkan menu awal
}

void loop() {
  menuManager.update(); // Perbaharui status menu berdasarkan input tombol
}
