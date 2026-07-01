// Implementação para o módulo manchester_diff
#include "manchester_diff.h"

// --------------------------------------------------------------
// RECEPTOR (RX)
// --------------------------------------------------------------
bool receberFrame_ManchesterDiferencial(uint8_t* frame_out, uint8_t* tam_lido, int pin_sensor, unsigned long tempo_bit) {
  unsigned long timeout_us = 10UL * tempo_bit;
  bool estadoAtual = digitalRead(pin_sensor);

  uint8_t N = 0;
  bool cabecalhoLido = false;

  // etapa == -1 -> lê o byte de cabeçalho (tamanho N)
  // etapa >= 0  -> lê os N bytes de dados
  for (int etapa = -1; etapa < 255; etapa++) {
    if (cabecalhoLido && etapa >= N) break;

    uint8_t byte_atual = 0;

    for (int j = 7; j >= 0; j--) {
      unsigned long inicio = micros();
      while (digitalRead(pin_sensor) == estadoAtual) {
        if (micros() - inicio > timeout_us) return false;
      }
      unsigned long delta = micros() - inicio;
      estadoAtual = !estadoAtual;

      if (delta > (tempo_bit * 3) / 4) {
        // Transição "longa": não houve inversão na fronteira -> bit = 1
        byte_atual |= (1 << j);
      } else {
        // Transição "curta": houve inversão na fronteira (bit = 0);
        // falta consumir a transição obrigatória do meio do bit.
        inicio = micros();
        while (digitalRead(pin_sensor) == estadoAtual) {
          if (micros() - inicio > timeout_us) return false;
        }
        estadoAtual = !estadoAtual;
      }
    }

    if (etapa == -1) {
      N = byte_atual;
      *tam_lido = N;
      cabecalhoLido = true;
    } else {
      frame_out[etapa] = byte_atual;
    }
  }

  return true;
}

