#include "nrz_l.h"

// ------------------------------------------------------------------
// RECEPTOR (RX) NRZ-L
// ------------------------------------------------------------------
bool receberFrame_NRZL(uint8_t* frame_out, uint8_t* tam_lido, int pin_sensor, unsigned long tempo_bit) {
  unsigned long timeout_us = 10UL * tempo_bit; 
  uint8_t bytes_lidos = 0;
  uint8_t N = 0;
  bool lendo_tamanho = true;
  
  while (true) {
    // 1. AGUARDA O START BIT (Transição do estado ocioso LOW para HIGH)
    unsigned long inicio_timeout = micros();
    while (digitalRead(pin_sensor) == LOW) {
       if (micros() - inicio_timeout > timeout_us) {
           return false; // Timeout: Linha inativa
       }
    }
    
    // 2. SINCRONIZAÇÃO DE FASE
    // Aguarda 1.5 * tempo_bit para deslocar a leitura para o exato CENTRO do 1º bit de dados
    unsigned long inicio_bit = micros();
    while (micros() - inicio_bit < (tempo_bit + (tempo_bit / 2))) {}
    
    uint8_t byte_atual = 0;
    
    // 3. LÊ OS 8 BITS DE DADOS
    for (int j = 7; j >= 0; j--) {
      // Aplica a regra: Se a leitura for LOW, o bit é 1.
      if (digitalRead(pin_sensor) == LOW) {
        byte_atual |= (1 << j);
      }
      // Se for HIGH, o bit permanece 0 (que é o estado inicial da variável)
      
      // Aguarda 1 tempo_bit inteiro para cair no centro do próximo bit
      inicio_bit = micros();
      while (micros() - inicio_bit < tempo_bit) {}
    }
    
    // 4. PROCESSA O BYTE LIDO
    if (lendo_tamanho) {
       N = byte_atual;
       *tam_lido = N;
       lendo_tamanho = false;
       if (N == 0) return true; 
    } else {
       frame_out[bytes_lidos] = byte_atual;
       bytes_lidos++;
       if (bytes_lidos >= N) {
           return true; 
       }
    }
  }
}
