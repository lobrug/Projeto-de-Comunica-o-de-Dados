// Implementação para o módulo manchester_diff
#include "manchester_diff.h"

// --------------------------------------------------------------
// TRANSMISSOR (TX)
// --------------------------------------------------------------
void enviarFrame_ManchesterDiferencial(const uint8_t* frame, uint8_t tam, int pin_led, unsigned long tempo_bit) {
  bool estadoAtual = digitalRead(pin_led);
  unsigned long tempoMeioBit = tempo_bit / 2;

  // Monta um "frame virtual": 1 byte de cabeçalho (tam) + os dados
  for (int etapa = -1; etapa < (int)tam; etapa++) {
    uint8_t byteAtual = (etapa == -1) ? tam : frame[etapa];

    for (int j = 7; j >= 0; j--) {
      bool bit = (byteAtual >> j) & 0x01;

      // FRONTEIRA DO BIT: inverte se for '0'
      if (bit == 0) {
        estadoAtual = !estadoAtual;
      }
      digitalWrite(pin_led, estadoAtual);

      unsigned long inicio_espera = micros();
      while (micros() - inicio_espera < tempoMeioBit) {}

      // MEIO DO BIT: transição obrigatória de clock
      estadoAtual = !estadoAtual;
      digitalWrite(pin_led, estadoAtual);

      inicio_espera = micros();
      while (micros() - inicio_espera < tempoMeioBit) {}
    }
  }
}
