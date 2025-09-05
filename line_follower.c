/*
 * Exemplo 2: Leitura de Sensores de Linha Analógicos
 * * Este código lê o valor de 3 sensores de linha (ex: QTR, TCRT5000)
 * conectados aos pinos AIN0, AIN1 e AIN2 do ADS1115.
 * * O programa alterna a leitura entre os três canais e imprime
 * a tensão de cada um, útil para robôs seguidores de linha.
 * * Circuito sugerido:
 * - Saída do Sensor Esquerdo -> AIN0
 * - Saída do Sensor Central  -> AIN1
 * - Saída do Sensor Direito  -> AIN2
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "ads1115/ads1115.h"

// --- Configurações ---
#define I2C_PORT i2c0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5
#define ADS1115_I2C_ADDR 0x48

#define NUM_SENSORS 3

// Instância do ADC
ads1115_adc_t adc;

void setup_hardware()
{
    stdio_init_all();
    printf("Exemplo de Leitura de Sensores de Linha com ADS1115\n");

    // Inicializa I2C
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Inicializa o ADS1115
    ads1115_init(I2C_PORT, ADS1115_I2C_ADDR, &adc);

    // Configurações comuns do ADC
    ads1115_set_pga(ADS1115_PGA_4_096, &adc); // Ganho para sensores de 3.3V/5V
    ads1115_set_operating_mode(ADS1115_MODE_SINGLE_SHOT, &adc);
    ads1115_set_data_rate(ADS1115_RATE_250_SPS, &adc);
}

// Função para ler um canal específico
float read_sensor(enum ads1115_mux_t channel)
{
    uint16_t raw_adc;

    // Seleciona o canal de entrada
    ads1115_set_input_mux(channel, &adc);

    // Escreve a nova configuração (apenas o MUX mudou)
    ads1115_write_config(&adc);

    // Faz a leitura
    ads1115_read_adc(&raw_adc, &adc);

    // Retorna a tensão convertida
    return ads1115_raw_to_volts(raw_adc, &adc);
}

int main()
{
    setup_hardware();

    float sensor_values[NUM_SENSORS];
    enum ads1115_mux_t channels[] = {
        ADS1115_MUX_SINGLE_0, // Sensor 0
        ADS1115_MUX_SINGLE_1, // Sensor 1
        ADS1115_MUX_SINGLE_2  // Sensor 2
    };

    while (1)
    {
        // Lê cada sensor sequencialmente
        for (int i = 0; i < NUM_SENSORS; i++)
        {
            sensor_values[i] = read_sensor(channels[i]);
        }

        // Imprime os valores lidos
        printf("Valores: [S0: %.3f V]  [S1: %.3f V]  [S2: %.3f V]\n",
               sensor_values[0], sensor_values[1], sensor_values[2]);

        sleep_ms(500); // Meio segundo entre as leituras completas
    }
    return 0;
}
