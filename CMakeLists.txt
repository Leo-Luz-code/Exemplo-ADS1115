# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

# Importa as definições do SDK do Pico
include(pico_sdk_import.cmake)

project(pico_ads1115_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Inicializa o SDK
pico_sdk_init()

# --- Biblioteca ADS1115 ---
# Adiciona a biblioteca como uma "INTERFACE" para ser facilmente linkada
add_library(ads1115 INTERFACE)
target_sources(ads1115 INTERFACE
    ads1115/ads1115.c
)
target_include_directories(ads1115 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ads1115
)
target_link_libraries(ads1115 INTERFACE
    hardware_i2c
    pico_stdlib
)

# --- Executáveis dos Exemplos ---

# Exemplo 1: Monitor de Bateria
add_executable(battery_monitor
    battery_monitor.c
)
target_link_libraries(battery_monitor PRIVATE ads1115)
pico_add_extra_outputs(battery_monitor)

# Exemplo 2: Seguidor de Linha
add_executable(line_follower
    line_follower.c
)
target_link_libraries(line_follower PRIVATE ads1115)
pico_add_extra_outputs(line_follower)

# Exemplo 3: Medidor de Corrente
add_executable(motor_current
    motor_current.c
)
target_link_libraries(motor_current PRIVATE ads1115)
pico_add_extra_outputs(motor_current)
