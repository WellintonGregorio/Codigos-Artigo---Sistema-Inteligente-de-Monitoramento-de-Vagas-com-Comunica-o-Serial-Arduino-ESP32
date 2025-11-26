// ====== ESTACIONAMENTO INTELIGENTE - ESP32 (FINAL) ======
// Recebe: VC1,VC2,VC3,VC4,VE1,VE2,VE3,VE4
// Controle de LEDs + buzzer ativo LOW
// ==========================================================

// ------------------ PINAGEM ---------------------
#define LED_VC1_VERDE 2
#define LED_VC1_VERM  25
#define LED_VC2_VERDE 4
#define LED_VC2_VERM  5
#define LED_VC3_VERDE 18
#define LED_VC3_VERM  19
#define LED_VC4_VERDE 21
#define LED_VC4_VERM  22

// LEDs amarelos (ocupada)
#define LED_VE1 23
#define LED_VE2 14
#define LED_VE3 27
#define LED_VE4 32

// LEDs azuis (livre)
#define LED_VEL1 12
#define LED_VEL2 13
#define LED_VEL3 33
#define LED_VEL4 17

// Buzzer ativo em LOW
#define BUZZER 26

// Estados
bool vagasComuns[4]    = {0,0,0,0};
bool vagasEspeciais[4] = {0,0,0,0};
bool ultimoEstadoEspecial[4] = {0,0,0,0};

void setup() {
  Serial.begin(115200);      // Debug
  Serial2.begin(9600, SERIAL_8N1, 16, 17);  // RX2 = 16, TX2 = 17

  // Configurar LEDs comuns
  pinMode(LED_VC1_VERDE, OUTPUT);
  pinMode(LED_VC1_VERM,  OUTPUT);
  pinMode(LED_VC2_VERDE, OUTPUT);
  pinMode(LED_VC2_VERM,  OUTPUT);
  pinMode(LED_VC3_VERDE, OUTPUT);
  pinMode(LED_VC3_VERM,  OUTPUT);
  pinMode(LED_VC4_VERDE, OUTPUT);
  pinMode(LED_VC4_VERM,  OUTPUT);

  // LEDs especiais ocupada
  pinMode(LED_VE1, OUTPUT);
  pinMode(LED_VE2, OUTPUT);
  pinMode(LED_VE3, OUTPUT);
  pinMode(LED_VE4, OUTPUT);

  // LEDs especiais livres
  pinMode(LED_VEL1, OUTPUT);
  pinMode(LED_VEL2, OUTPUT);
  pinMode(LED_VEL3, OUTPUT);
  pinMode(LED_VEL4, OUTPUT);

  // Buzzer
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH); // desligado (ativo por LOW)

  Serial.println("ESP32 Pronta para receber dados do Arduino...");
}

void loop() {

  // ================= LEITURA DA SERIAL =================
  if (Serial2.available()) {
    String linha = Serial2.readStringUntil('\n');
    linha.trim();

    int valores[8];
    int index = 0;

    // separa por vírgulas
    char separador[] = ",";
    char *token = strtok((char*)linha.c_str(), separador);

    while (token != NULL && index < 8) {
      valores[index++] = atoi(token);
      token = strtok(NULL, separador);
    }

    // Se os 8 valores foram recebidos corretamente
    if (index == 8) {
      // Atribuir
      for (int i = 0; i < 4; i++) vagasComuns[i]    = valores[i];
      for (int i = 0; i < 4; i++) vagasEspeciais[i] = valores[i+4];

      Serial.print("Recebido → ");
      Serial.println(linha);
    }
  }

  // ==================== LEDS VAGAS COMUNS =====================
  digitalWrite(LED_VC1_VERDE, !vagasComuns[0]);
  digitalWrite(LED_VC1_VERM,  vagasComuns[0]);

  digitalWrite(LED_VC2_VERDE, !vagasComuns[1]);
  digitalWrite(LED_VC2_VERM,  vagasComuns[1]);

  digitalWrite(LED_VC3_VERDE, !vagasComuns[2]);
  digitalWrite(LED_VC3_VERM,  vagasComuns[2]);

  digitalWrite(LED_VC4_VERDE, !vagasComuns[3]);
  digitalWrite(LED_VC4_VERM,  vagasComuns[3]);

  // ==================== LEDS VAGAS ESPECIAIS =====================

  // Amarelo = ocupada
  digitalWrite(LED_VE1, vagasEspeciais[0]);
  digitalWrite(LED_VE2, vagasEspeciais[1]);
  digitalWrite(LED_VE3, vagasEspeciais[2]);
  digitalWrite(LED_VE4, vagasEspeciais[3]);

  // Azul = livre
  digitalWrite(LED_VEL1, !vagasEspeciais[0]);
  digitalWrite(LED_VEL2, !vagasEspeciais[1]);
  digitalWrite(LED_VEL3, !vagasEspeciais[2]);
  digitalWrite(LED_VEL4, !vagasEspeciais[3]);

  // ==================== BUZZER =====================
  for (int i = 0; i < 4; i++) {
    if (vagasEspeciais[i] == 1 && ultimoEstadoEspecial[i] == 0) {
      // mudou de livre -> ocupada
      tone(BUZZER, 3000);
      delay(200);
      noTone(BUZZER);
      digitalWrite(BUZZER, HIGH);
    }
    ultimoEstadoEspecial[i] = vagasEspeciais[i];
  }

  delay(20);
}
