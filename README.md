# NRF24L01+ HAL Driver

## Overview
HAL driver for NRF24L01+ 2.4GHz RF transceiver with dynamic payload capabilities. 

>Supports:
>- Static and Dynamic Payloads :alien:
>- Data rates: 250Kbps, 1Mbps, 2Mbps
>- Output power: 0, -6, -12, -18 dBm
>- Hardware ACK and Auto-Retransmit
>- IRQ handling

### SPI Configuration
| Parameter | Value |
|-----------|-------|
| Mode | Full-Duplex Master |
| Data Size | 8 bits |
| Data Rate | 0-10 Mbps |
| CPOL | 0 |
| CPHA | 0 |
| First Bit | MSB First |

## Hardware Connection
| NRF24L01+ | STM32 |
|-----------|-------|
| CSN       | SPI_CSN |
| SCK       | SPI_SCK |
| MOSI      | SPI_MOSI |
| MISO      | SPI_MISO |
| CE        | NRF_CE |
| IRQ       | NRF_IRQ (EXTI) |
| VCC       | 3.3V |
| GND       | GND |

## Basic Usage

### Initialization
```c
// Transmitter Mode
nrf_init(_MODE_TX, _PAYLOAD_DYNAMIC, 2400, _PWR_6DBM, _SPEED_2MBPS, 0);

// Receiver Mode
nrf_init(_MODE_RX, _PAYLOAD_DYNAMIC, 2400, _PWR_6DBM, _SPEED_2MBPS, 0);
```

### Data Transfer (No interrupt)
```c
// Transmit
nrf_transmit(tx_data, tx_data_len);

// Receive
if(nrf_rx_buffer_ready()) {
    nrf_receive(rx_data);
}
```

### Interrupt Handling
```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == NRF_IRQ_Pin) {
        if(nrf_check_rx_dr()) {
            nrf_receive(rx_data);
            nrf_flush_rx_fifo();
            nrf_reset_rx_dr();
        }
        else if(nrf_check_tx_ds()) {
            nrf_flush_tx_fifo();
            nrf_reset_tx_ds();
        }
        else if(nrf_check_max_rt()) {
            nrf_flush_tx_fifo();
            nrf_reset_max_rt();
        }
    }
}
```

## Core Functions
| Function | Description |
|----------|-------------|
| `nrf_init()` | Initialize with mode, payload type, frequency, power, speed, payload length |
| `nrf_disable_itr()` | Disable/ enable IRQ |
| `nrf_transmit()` | Send data buffer |
| `nrf_receive()` | Read received data |
| `nrf_flush_tx_fifo()` | Clear TX FIFO |
| `nrf_flush_rx_fifo()` | Clear RX FIFO |
| `nrf_check_rx_dr()` | Check if data received |
| `nrf_check_tx_ds()` | Check if data sent |
| `nrf_reset_rx_dr()` | Clear RX flag |
| `nrf_reset_tx_ds()` | Clear TX flag |

### Example Code

#### main.c (IRQ, DPL)
```c
#include "nrf24l01.h"

#define BUFF_SIZE    32

uint8_t tx_buffer[BUFF_SIZE];
uint8_t rx_buffer[BUFF_SIZE] = {0};
uint8_t tx_index = 1;

int main(void)
{

    for(int i = 0; i < BUFF_SIZE; i++){
        tx_buffer[i] = i;
    }

//Transmitter
    nrf_init(_MODE_TX, _PAYLOAD_DYNAMIC , 2400, _PWR_6DBM, _SPEED_2MBPS, 0);
    nrf_disable_itr(1, 0, 0);
//Receiver
    nrf_init(_MODE_RX, _PAYLOAD_DYNAMIC , 2400, _PWR_6DBM, _SPEED_2MBPS, 0);
    nrf_disable_itr(0, 1, 1);


    while (1)
    {
//Transmitter
        HAL_Delay(1000);
        for(int i = 0; i < BUFF_SIZE; i++){
            tx_buffer[i]++;
        }
        nrf_transmit(tx_buffer, tx_index++);
        if(tx_index > BUFF_SIZE)    tx_index = 1;
//Receiver
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == NRF_IRQ_Pin){
        if(nrf_check_rx_dr()){
            nrf_receive(rx_buffer);
            nrf_flush_rx_fifo();
            nrf_reset_rx_dr();
        }
        else if(nrf_check_max_rt()){
            nrf_flush_tx_fifo();
            nrf_reset_max_rt();
        }
        else if(nrf_check_tx_ds()){
            nrf_flush_tx_fifo();
            nrf_reset_tx_ds();
        }
    }
}

```

Happy transmitting :rocket:

