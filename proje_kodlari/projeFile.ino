#include <IRremote.hpp>
#include <Stepper.h>

// --- PIN AYARLARI ---
#define IR_PIN 2

// Buradaki sayıları değiştirerek kabloları sökmeden yönleri düzeltebiliriz.

// SOL MOTOR (Eğer sol tekerlek ters dönerse 4 ile 3'ün yerini değiştir)
#define ENA 5
#define IN1 4  
#define IN2 3  

// SAĞ MOTOR (Eğer sağ tekerlek ters dönerse 8 ile 7'nin yerini değiştir)
#define IN3 7 
#define IN4 8 
#define ENB 6

// Step Motor Ayarları (28BYJ-48)
#define STEPS 2048
Stepper lift(STEPS, 9, 11, 10, 12);

// Zamanlayıcı
unsigned long lastSignalTime = 0;
const int SIGNAL_TIMEOUT = 250;

void setup() {
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  // Motor Hızları
  analogWrite(ENA, 180); 
  analogWrite(ENB, 180); 

  lift.setSpeed(12);
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void loop() {
  if (IrReceiver.decode()) {
    unsigned long code = IrReceiver.decodedIRData.command;
    lastSignalTime = millis(); 

    switch (code) {
      // ⬆ İLERİ GİT
      case 0x18:
      case 0xFF629D:
        digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); // Sol İleri
        digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); // Sağ İleri
        break;

      // ⬇ GERİ GİT
      case 0x52:
      case 0xFFA857:
        digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); // Sol Geri
        digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); // Sağ Geri
        break;

      // ⬅ TANK SOLA DÖNÜŞ
      case 0x8:
      case 0xFF22DD:
        digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); // Sol Geri
        digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); // Sağ İleri
        break;

      // ➡ TANK SAĞA DÖNÜŞ
      case 0x0:
      case 0xFFC23D:
        digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); // Sol İleri
        digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); // Sağ Geri
        break;

      // Yük Yükselt
      case 0x46:
      case 0xFF18E7:
        lift.step(50); 
        break;

      // Yük İndir
      case 0x15:
      case 0xFF4AB5:
        lift.step(-50); 
        break;
    }
    IrReceiver.resume();
  }

  if (millis() - lastSignalTime > SIGNAL_TIMEOUT) {
    stopMotors();
  }
}