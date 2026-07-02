#include "nrz_l.h"

// ------------------------------------------------------------------
// TRANSMISSOR (TX) NRZ-L
// ------------------------------------------------------------------
void enviarFrame_NRZL(const uint8_t* frame, uint8_t tam, int pin_led, unsigned long tempo_bit) {
  // etapa == -1 -> envia o byte de cabeçalho (tamanho 'tam')
  // etapa >= 0  -> envia os bytes de dados do frame
  for (int etapa = -1; etapa < (int)tam; etapa++) {
    uint8_t byteAtual = (etapa == -1) ? tam : frame[etapa];

    // START BIT (HIGH - Desperta o receptor criando uma borda de subida)
    digitalWrite(pin_led, HIGH);
    unsigned long inicio_espera = micros();
    while (micros() - inicio_espera < tempo_bit) {}

    // 8 BITS DE DADOS (MSB First)
    for (int j = 7; j >= 0; j--) {
      bool bit = (byteAtual >> j) & 0x01;

      // Aplica a regra: se bit for 1, nível é LOW. Se for 0, nível é HIGH.
      digitalWrite(pin_led, bit ? LOW : HIGH);

      inicio_espera = micros();
      while (micros() - inicio_espera < tempo_bit) {}
    }

    // STOP BIT (LOW - Retorna a linha ao estado ocioso / prepara para o próximo Start)
    digitalWrite(pin_led, LOW);
    inicio_espera = micros();
    while (micros() - inicio_espera < tempo_bit) {}
  }
}
