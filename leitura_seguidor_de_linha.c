#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Funções e definições do ADS1115 (reutilizar do exemplo anterior)
const uint8_t ADS_ADDR = 0x48;
const uint8_t REG_CONV = 0x00;
const uint8_t REG_CONF = 0x01;
int16_t read_adc(uint8_t channel); // Assumindo que a função do exemplo 1 está aqui

int main() {
    stdio_init_all();
    
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    
    while(1) {
        // Lê os valores dos três sensores conectados aos canais A0, A1 e A2
        int16_t sensor_esquerda = read_adc(0);
        int16_t sensor_centro = read_adc(1);
        int16_t sensor_direita = read_adc(2);

        printf("Esquerda: %d\tCentro: %d\tDireita: %d\n", 
               sensor_esquerda, sensor_centro, sensor_direita);
        
        // A partir daqui, implementa-se a lógica de controle do AGV (ex: PID)
        // com base nos valores lidos para corrigir a trajetória.

        sleep_ms(100);
    }
    return 0;
}

// Implementação da função read_adc (copiada do exemplo anterior)
int16_t read_adc(uint8_t channel) {
    uint16_t config = 0;
    config |= (1 << 15); // Inicia conversão
    config |= (1 << 8);  // Modo single-shot
    config |= (1 << 9);  // PGA_1 -> +/- 4.096V
    switch (channel) {
        case 0: config |= (4 << 12); break;
        case 1: config |= (5 << 12); break;
        case 2: config |= (6 << 12); break;
        case 3: config |= (7 << 12); break;
    }
    uint8_t write_buf[3];
    write_buf[0] = REG_CONF;
    write_buf[1] = (config >> 8) & 0xFF;
    write_buf[2] = config & 0xFF;
    i2c_write_blocking(i2c_default, ADS_ADDR, write_buf, 3, false);
    sleep_ms(20);
    uint8_t reg_ptr = REG_CONV;
    i2c_write_blocking(i2c_default, ADS_ADDR, &reg_ptr, 1, true);
    uint8_t read_buf[2];
    i2c_read_blocking(i2c_default, ADS_ADDR, read_buf, 2, false);
    return (int16_t)((read_buf[0] << 8) | read_buf[1]);
}
