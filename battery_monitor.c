/*
 * Exemplo 1: Monitoramento Preciso da Bateria
 * * Este código mede a tensão de uma bateria (ex: LiPo 3.7V) conectada ao
 * pino AIN0 do ADS1115.
 * * ATENÇÃO: A tensão máxima de uma bateria LiPo (~4.2V) é maior que a tensão
 * de operação do Pico (3.3V). É ESSENCIAL usar um DIVISOR DE TENSÃO para
 * proteger o ADS1115 e o Pico.
 * * Circuito sugerido:
 * BATERIA+ --- [Resistor R1] --+-- [Resistor R2] --- GND
 * |
 * AIN0 do ADS1115
 * * Use resistores de igual valor (ex: R1=10k, R2=10k) para dividir a tensão
 * por 2. O código levará essa divisão em conta.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "ads1115/ads1115.h"

// --- Configurações ---
#define I2C_PORT i2c0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5
#define ADS1115_I2C_ADDR 0x48

// Fator do divisor de tensão. Se R1=10k e R2=10k, o fator é (R1+R2)/R2 = 2.0
const float VOLTAGE_DIVIDER_FACTOR = 2.0;

// Instância do ADC
ads1115_adc_t adc;

void setup_hardware()
{
    stdio_init_all();
    printf("Exemplo de Monitor de Bateria com ADS1115\n");

    // Inicializa I2C
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Inicializa o ADS1115
    ads1115_init(I2C_PORT, ADS1115_I2C_ADDR, &adc);

    // Configura o ADC para este caso de uso
    ads1115_set_input_mux(ADS1115_MUX_SINGLE_0, &adc);          // Medir no pino AIN0
    ads1115_set_pga(ADS1115_PGA_4_096, &adc);                   // Ganho para até 4.096V (seguro para Vbat/2)
    ads1115_set_operating_mode(ADS1115_MODE_SINGLE_SHOT, &adc); // Modo de leitura única
    ads1115_set_data_rate(ADS1115_RATE_128_SPS, &adc);          // Taxa de amostragem padrão

    // Escreve a configuração no dispositivo
    ads1115_write_config(&adc);
}

int main()
{
    setup_hardware();

    uint16_t raw_adc;
    float adc_voltage;
    float battery_voltage;

    while (1)
    {
        // Lê o valor bruto do ADC
        ads1115_read_adc(&raw_adc, &adc);

        // Converte o valor bruto para tensão (no pino do ADC)
        adc_voltage = ads1115_raw_to_volts(raw_adc, &adc);

        // Calcula a tensão real da bateria, compensando o divisor de tensão
        battery_voltage = adc_voltage * VOLTAGE_DIVIDER_FACTOR;

        // Imprime os resultados
        printf("Valor Bruto: %d, Tensão no Pino: %.3f V, Tensão da Bateria: %.3f V\n",
               raw_adc, adc_voltage, battery_voltage);

        sleep_ms(2000); // Espera 2 segundos para a próxima leitura
    }
    return 0;
}