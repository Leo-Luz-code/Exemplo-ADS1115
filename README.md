# ADS1115 com Raspberry Pi Pico

Este repositório contém uma biblioteca e três exemplos práticos para utilizar o conversor analógico-digital (ADC) de 16 bits ADS1115 com a placa Raspberry Pi Pico, utilizando o SDK C/C++.

## Descrição

O Raspberry Pi Pico possui ADCs internos de 12 bits, mas para aplicações que exigem maior precisão, como monitoramento de baterias, leitura de sensores de alta sensibilidade ou medições de corrente, um ADC externo como o ADS1115 é uma excelente escolha. Este projeto visa facilitar a integração do ADS1115, fornecendo uma biblioteca simples e exemplos claros para cenários comuns.

## Conteúdo do Projeto

### Biblioteca ads1115

Uma biblioteca simples (`ads1115.h`, `ads1115.c`, `registers.h`) para inicializar e interagir com o módulo ADS1115 via I2C.

### Exemplos

1. **Monitoramento de Bateria** (`battery_monitor.c`):
   Mede com precisão a tensão de uma bateria (ex: LiPo), utilizando um divisor de tensão para garantir a segurança dos componentes.

2. **Leitura de Sensores de Linha** (`line_follower.c`):
   Lê múltiplos sensores analógicos (como os usados em robôs seguidores de linha), alternando entre os canais do ADS1115.

3. **Medição de Corrente de Motor** (`motor_current.c`):
   Utiliza um sensor de corrente (ex: ACS712) para medir o consumo de um motor DC, convertendo a tensão de saída do sensor para Amperes.

## Requisitos de Hardware

- Raspberry Pi Pico
- Módulo ADC ADS1115
- Protoboard e jumpers
- **Para o Exemplo 1**: Uma bateria (ex: LiPo 3.7V) e dois resistores de mesmo valor (ex: 10kΩ)
- **Para o Exemplo 2**: Sensores de linha analógicos (ex: QTR-1A, TCRT5000)
- **Para o Exemplo 3**: Um sensor de corrente (ex: ACS712) e um motor DC com sua fonte de alimentação

## Requisitos de Software

- Raspberry Pi Pico C/C++ SDK configurado
- CMake (versão 3.13 ou superior)
- Compilador ARM GCC (`arm-none-eabi-gcc`)

## Estrutura de Arquivos

```
.
├── ads1115/
│   ├── ads1115.c
│   ├── ads1115.h
│   └── registers.h
├── battery_monitor.c
├── line_follower.c
├── motor_current.c
├── CMakeLists.txt
└── pico_sdk_import.cmake
```

## Como Compilar e Usar

### 1. Configurar o SDK do Pico

Antes de compilar, abra o arquivo `pico_sdk_import.cmake` e edite a seguinte linha para apontar para o diretório onde o seu SDK do Pico está instalado:

```cmake
# Exemplo:
set(PICO_SDK_PATH "/caminho/para/seu/pico-sdk")
```

Alternativamente, você pode definir uma variável de ambiente `PICO_SDK_PATH`.

### 2. Compilar o Projeto

Navegue até o diretório raiz do projeto e execute os seguintes comandos no terminal:

```bash
mkdir build
cd build
cmake ..
make
```

Após a compilação, a pasta `build` conterá os arquivos de firmware para cada exemplo: `battery_monitor.uf2`, `line_follower.uf2`, e `motor_current.uf2`.

### 3. Carregar o Firmware

1. Pressione e segure o botão **BOOTSEL** do seu Pico
2. Conecte-o ao computador via USB
3. Solte o botão **BOOTSEL**. O Pico aparecerá como um dispositivo de armazenamento USB
4. Arraste e solte o arquivo `.uf2` desejado para dentro do dispositivo de armazenamento do Pico. A placa irá reiniciar automaticamente com o novo firmware

## Detalhes dos Exemplos e Circuitos

**Pinos I2C Padrão:** Todos os exemplos usam `i2c0` com SDA no GP4 e SCL no GP5.

### 1. Monitoramento de Bateria (`battery_monitor.c`)

**Circuito:** É essencial usar um divisor de tensão para ler baterias com tensão superior a 3.3V. Conecte o ponto central do divisor ao pino AIN0 do ADS1115.

```
BATERIA+ --- [R1] --+-- [R2] --- GND
                   |
                  AIN0
```

**Código:** O fator do divisor (`VOLTAGE_DIVIDER_FACTOR`) deve ser ajustado no código para refletir os resistores usados (Fator = (R1+R2)/R2).

### 2. Leitura de Sensores de Linha (`line_follower.c`)

**Circuito:** Conecte a saída analógica de cada sensor de linha a um pino de entrada diferente do ADS1115.

- Sensor Esquerdo → AIN0
- Sensor Central → AIN1
- Sensor Direito → AIN2

### 3. Medição de Corrente de Motor (`motor_current.c`)

**Circuito:** Conecte o pino de saída (Vout) do sensor de corrente (ex: ACS712) ao pino AIN0 do ADS1115.

**Código:** As constantes `ZERO_CURRENT_VOLTAGE` (tensão com 0A) e `SENSITIVITY` (mV/A) devem ser ajustadas de acordo com o modelo do seu sensor e a tensão de alimentação dele, conforme o datasheet.

## Licença

Este projeto é distribuído sob a licença BSD-3-Clause.
