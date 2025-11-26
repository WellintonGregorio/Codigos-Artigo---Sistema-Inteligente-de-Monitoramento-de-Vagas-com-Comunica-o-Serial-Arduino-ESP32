// ===========================================================
// ARDUINO – 4 ULTRASSÔNICOS AJ-SR04 (vagas comuns)
//            4 SHARP GP2Y0A21 (vagas especiais)
// Envia: VC1,VC2,VC3,VC4,VE1,VE2,VE3,VE4
// ===========================================================

#include <SoftwareSerial.h>

// ---------------- PINAGEM ----------------

// ======= ULTRASSÔNICOS (VAGAS COMUNS) =======
#define TRIG1 2
#define ECHO1 3

#define TRIG2 4
#define ECHO2 5

#define TRIG3 6
#define ECHO3 7

#define TRIG4 8
#define ECHO4 9

// ======= SHARP (VAGAS ESPECIAIS) =======
#define SHARP1 A0
#define SHARP2 A1
#define SHARP3 A2
#define SHARP4 A3

// Comunicação com a ESP32
#define RX_ESP 10
#define TX_ESP 11
SoftwareSerial espSerial(RX_ESP, TX_ESP);

// ---------------- LIMIARES ----------------
#define DIST_OCUPADA 70  // cm — ultrassônico
#define SHARP_MIN 90     // distância em cm ≈ ajustar por teste
#define SHARP_MAX 10     // distância mínima (~10 cm)

// Função de leitura Sharp (usando aproximação típica GP2Y0A21)
int lerSharp(int pin) {
  int v = analogRead(pin);
  if (v < 3) return 999; // sem objeto

  float d = 48200.0 / (v - 20); // fórmula aproximação GP2Y0A21
  return (int)d;
}

// Função medir distância ultrassônico
long medirDistancia(int TRIG, int ECHO) {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duracao = pulseIn(ECHO, HIGH, 30000);

  if (duracao == 0) return 999;
  return duracao * 0.034 / 2;
}

void setup() {
  // pinos ultrassom
  pinMode(TRIG1, OUTPUT); pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT); pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT); pinMode(ECHO3, INPUT);
  pinMode(TRIG4, OUTPUT); pinMode(ECHO4, INPUT);

  espSerial.begin(9600);
  Serial.begin(9600);
  Serial.println("=== Arduino Iniciado (8 sensores) ===");
}

void loop() {

  // ----- Vagas Comuns (ultrassônico) -----
  long d1 = medirDistancia(TRIG1, ECHO1);
  long d2 = medirDistancia(TRIG2, ECHO2);
  long d3 = medirDistancia(TRIG3, ECHO3);
  long d4 = medirDistancia(TRIG4, ECHO4);

  int VC1 = (d1 <= DIST_OCUPADA) ? 1 : 0;
  int VC2 = (d2 <= DIST_OCUPADA) ? 1 : 0;
  int VC3 = (d3 <= DIST_OCUPADA) ? 1 : 0;
  int VC4 = (d4 <= DIST_OCUPADA) ? 1 : 0;

  // ----- Vagas Especiais (SHARP) -----
  int s1 = lerSharp(SHARP1);
  int s2 = lerSharp(SHARP2);
  int s3 = lerSharp(SHARP3);
  int s4 = lerSharp(SHARP4);

  int VE1 = (s1 >= SHARP_MAX && s1 <= SHARP_MIN) ? 1 : 0;
  int VE2 = (s2 >= SHARP_MAX && s2 <= SHARP_MIN) ? 1 : 0;
  int VE3 = (s3 >= SHARP_MAX && s3 <= SHARP_MIN) ? 1 : 0;
  int VE4 = (s4 >= SHARP_MAX && s4 <= SHARP_MIN) ? 1 : 0;

  // ----- Enviar Pacote -----
  espSerial.print(VC1); espSerial.print(",");
  espSerial.print(VC2); espSerial.print(",");
  espSerial.print(VC3); espSerial.print(",");
  espSerial.print(VC4); espSerial.print(",");
  espSerial.print(VE1); espSerial.print(",");
  espSerial.print(VE2); espSerial.print(",");
  espSerial.print(VE3); espSerial.print(",");
  espSerial.println(VE4);

  // Debug
  Serial.print("VC:");
  Serial.print(VC1); Serial.print(",");
  Serial.print(VC2); Serial.print(",");
  Serial.print(VC3); Serial.print(",");
  Serial.println(VC4);

  Serial.print("VE:");
  Serial.print(VE1); Serial.print(",");
  Serial.print(VE2); Serial.print(",");
  Serial.print(VE3); Serial.print(",");
  Serial.println(VE4);

  delay(150);
}
