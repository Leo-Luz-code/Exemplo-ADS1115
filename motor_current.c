/*
 * Exemplo 3: Medição de Corrente de Motor
 * * Este código mede a corrente consumida por um motor DC usando um sensor
 * de corrente como o ACS712, conectado ao pino AIN0.
 * * O ACS712 gera uma tensão proporcional à corrente. Com 0A de corrente,
 * a saída é VCC/2 (ex: 1.65V para um sensor alimentado com 3.3V).
 * A tensão aumenta ou diminui a partir deste ponto central.
 * * Circuito sugerido:
 * - Saída (Vout) do sensor ACS712 -> AIN0 do ADS1115
 * * ATENÇÃO: Verifique o datasheet do seu sensor ACS712 para os valores
 * corretos de `ZERO_CURRENT_VOLTAGE` e `SENSITIVITY`. Os valores abaixo
 * são para um módulo ACS712-05B alimentado com 3.3V.
 */

#include <stdio.h>
#include <math.h> // Para usar fabsf()
#include "pico/stdlib.h"
#include "ads1115/ads1115.h"

// --- Configurações ---
#define I2C_PORT i2c0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5
#define ADS1115_I2C_ADDR 0x48

// --- Constantes do Sensor de Corrente (ACS712-05B com VCC = 3.3V) ---
// Tensão de saída com 0A de corrente. Se VCC=5V, seria 2.5V.
const float ZERO_CURRENT_VOLTAGE = 1.65f;
// Sensibilidade do sensor em Volts por Ampere. Para o ACS712-05B, é 185mV/A.
const float SENSITIVITY = 0.185f;

// Instância do ADC
ads1115_adc_t adc;

void setup_hardware()
{
    stdio_init_all();
    printf("Exemplo de Medição de Corrente de Motor com ADS1115\n");

    // Inicializa I2C
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Inicializa o ADS1115
    ads1115_init(I2C_PORT, ADS1115_I2C_ADDR, &adc);

    // Configura o ADC
    ads1115_set_input_mux(ADS1115_MUX_SINGLE_0, &adc); // Medir no pino AIN0
    ads1115_set_pga(ADS1115_PGA_4_096, &adc);          // O ACS712 (3.3V) opera entre 0-3.3V
    ads1115_set_operating_mode(ADS1115_MODE_SINGLE_SHOT, &adc);
    ads1115_set_data_rate(ADS1115_RATE_860_SPS, &adc); // Taxa alta para capturar picos

    // Escreve a configuração no dispositivo
    ads1115_write_config(&adc);
}

int main()
{
    setup_hardware();

    uint16_t raw_adc;
    float sensor_voltage;
    float current_amps;

    while (1)
    {
        // Lê o valor bruto do ADC
        ads1115_read_adc(&raw_adc, &adc);

        // Converte o valor bruto para tensão
        sensor_voltage = ads1115_raw_to_volts(raw_adc, &adc);

        // Calcula a corrente em Amperes
        // Corrente = (TensãoLida - TensãoZero) / Sensibilidade
        current_amps = (sensor_voltage - ZERO_CURRENT_VOLTAGE) / SENSITIVITY;

        // Imprime os resultados
        // Usamos fabsf() para mostrar o valor absoluto, já que a corrente pode ser negativa
        printf("Tensão do Sensor: %.3f V, Corrente: %.3f A\n",
               sensor_voltage, current_amps);

        sleep_ms(100); // Leitura rápida a cada 100ms
    }
    return 0;
}
