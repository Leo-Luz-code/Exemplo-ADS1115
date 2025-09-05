#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Endereço I2C do ADS1115
const uint8_t ADS_ADDR = 0x48;

// Ponteiros para os registradores
const uint8_t REG_CONV = 0x00;
const uint8_t REG_CONF = 0x01;

// Função para ler um canal do ADS1115
int16_t read_adc(uint8_t channel) {
    // Configuração para a leitura (16 bits)
    // Bit 15: 1 (Inicia conversão)
    // Bit 14-12: MUX (Canal A0-A3)
    // Bit 11-9: PGA (Ganho)
    // Bit 8: Modo (Single-shot)
    uint16_t config = 0;
    
    // Inicia uma conversão single-shot
    config |= (1 << 15); 
    // Modo single-shot
    config |= (1 << 8);  
    // Define o ganho (PGA_1 -> +/- 4.096V)
    config |= (1 << 9);  

    // Define o canal (MUX)
    switch (channel) {
        case 0: config |= (4 << 12); break;
        case 1: config |= (5 << 12); break;
        case 2: config |= (6 << 12); break;
        case 3: config |= (7 << 12); break;
    }
    
    // Escreve a configuração no registrador de configuração
    uint8_t write_buf[3];
    write_buf[0] = REG_CONF;
    write_buf[1] = (config >> 8) & 0xFF; // MSB
    write_buf[2] = config & 0xFF;        // LSB
    i2c_write_blocking(i2c_default, ADS_ADDR, write_buf, 3, false);

    // Aguarda a conversão terminar (pode ser otimizado com polling do bit de status)
    sleep_ms(20);

    // Aponta para o registrador de conversão para leitura
    uint8_t reg_ptr = REG_CONV;
    i2c_write_blocking(i2c_default, ADS_ADDR, &reg_ptr, 1, true);

    // Lê os 2 bytes do resultado
    uint8_t read_buf[2];
    i2c_read_blocking(i2c_default, ADS_ADDR, read_buf, 2, false);

    return (int16_t)((read_buf[0] << 8) | read_buf[1]);
}

int main() {
    stdio_init_all();
    
    // Inicializa I2C nos pinos padrão (GP4=SDA, GP5=SCL)
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    
    // Fatores do divisor de tensão (R1=10k, R2=2.2k)
    const float R1 = 10000.0f;
    const float R2 = 2200.0f;
    const float fator_divisor = (R1 + R2) / R2;

    while(1) {
        int16_t adc_value = read_adc(0); // Lê canal A0
        // Converte o valor raw para tensão (usando a faixa de +/- 4.096V)
        float tensao_no_pino = adc_value * (4.096f / 32767.0f);
        float tensao_bateria = tensao_no_pino * fator_divisor;

        printf("Valor ADC: %d\n", adc_value);
        printf("Tensão no Pino: %.4f V\n", tensao_no_pino);
        printf("Tensão da Bateria: %.2f V\n", tensao_bateria);
        
        if (tensao_bateria < 11.0) {
            printf("ALERTA: Bateria Fraca!\n");
        }
        printf("---\n");
        sleep_ms(2000);
    }
    return 0;
}
