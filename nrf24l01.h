/*
 * nrf24l01.h
 *
 *  Created on: Apr 14, 2025
 *      Author: Juliano
 */

#ifndef NRF24L01_NRF24L01_H_
#define NRF24L01_NRF24L01_H_

#include "stdint.h"
#include "spi.h"

/*
 * CSN - SPI signal - chip select
 * CE - chip enable -  active high signal, used to activate the chip in RX or TX mode
 */

/*
 * SPI SETTINGS
 */

#define SPI			&hspi3
#define SPI_TRANSMISSION_TIMEOUT	10

/*
 * PIN DEFINITIONS
 */

#define CSN_PORT 	SPI3_CSN_GPIO_Port
#define CSN_PIN		SPI3_CSN_Pin

#define CE_PORT		NRF_CE_GPIO_Port
#define CE_PIN		NRF_CE_Pin


#define CSN_HIGH	HAL_GPIO_WritePin(SPI3_CSN_GPIO_Port, SPI3_CSN_Pin, 1);
#define CSN_LOW		HAL_GPIO_WritePin(SPI3_CSN_GPIO_Port, SPI3_CSN_Pin, 0);

#define CE_HIGH		HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, 1);
#define CE_LOW		HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, 0);

/*
 * ENUMS
 */

typedef enum{
	disabled = 0,
	enabled = 1
}MODE;

typedef enum{
	_PWR_18DBM = 0x0,
	_PWR_12DBM = 0x1,
	_PWR_6DBM = 0x2,
	_PWR_0DBM = 0x3
}NRF_OUTPUT_PWR;

typedef enum{
	_SPEED_1MBPS = 0x0,
	_SPEED_2MBPS = 0x1,
	_SPEED_250KBPS = 0x2
}NRF_OUTPUT_SPEED;

typedef enum{
	_CRC_1BYTE,
	_CRC_2BYTE
}NRF_CRC;

typedef enum{
	_MODE_RX,
	_MODE_TX
}NRF_MODE;

typedef enum{
	_PAYLOAD_STATIC,
	_PAYLOAD_DYNAMIC
}NRF_PAYLOAD;

typedef struct{
	NRF_MODE mode;
	NRF_PAYLOAD payload_setting;
	uint8_t payload_length;
	uint16_t frequency;
	NRF_OUTPUT_PWR power;
	NRF_OUTPUT_SPEED speed;
}NRF_Settings;

/*
 * FUNCTION PROTOTYPES
 */

uint8_t nrf_send_command(uint8_t cmd);
uint8_t nrf_read_command(uint8_t cmd);
uint8_t nrf_read_register(uint8_t reg);
uint8_t nrf_write_register(uint8_t reg, uint8_t val);
uint8_t nrf_transmit(uint8_t *buffer, uint8_t len);
uint8_t nrf_receive(uint8_t *buffer);
uint8_t nrf_rx_buffer_ready(void);
uint8_t nrf_flush_tx_fifo(void);
uint8_t nrf_flush_rx_fifo(void);
void nrf_reset(void);
uint8_t nrf_init(NRF_MODE mode, NRF_PAYLOAD pd, uint16_t freq, NRF_OUTPUT_PWR pwr, NRF_OUTPUT_SPEED speed, uint8_t pd_len);
uint8_t nrf_set_frequency(uint16_t freq);
uint8_t nrf_set_power(NRF_OUTPUT_PWR power);
uint8_t nrf_set_speed(NRF_OUTPUT_SPEED speed);
uint8_t nrf_set_static_rx_width(uint8_t pipe_num, uint8_t width);
uint8_t nrf_set_addr_width(uint8_t width);
uint8_t nrf_set_crc_scheme(NRF_CRC bytes);
uint8_t nrf_set_auto_ret_count(uint8_t ret);
uint8_t nrf_set_auto_ret_delay(uint16_t delay_us);
uint8_t nrf_disable_itr(uint8_t rx_dr, uint8_t tx_ds, uint8_t max_rt);
uint8_t nrf_check_rx_dr(void);
uint8_t nrf_check_tx_ds(void);
uint8_t nrf_check_max_rt(void);
void nrf_reset_rx_dr(void);
void nrf_reset_tx_ds(void);
void nrf_reset_max_rt(void);
uint8_t nrf_check_ret_count(void);
uint8_t nrf_check_lost_count(void);
uint8_t nrf_power_detector(void);
uint8_t nrf_ret_fifo_status(void);
uint8_t nrf_enable_DPL(MODE mode);
uint8_t nrf_enable_DPL_pipes(uint8_t num_of_pipes);
uint8_t nrf_enable_DPL_pipe_custom(uint8_t pipe_num);
uint8_t nrf_enable_ACK_PAY(MODE mode);
uint8_t nrf_enable_AA_pipes(uint8_t num_of_pipes);
uint8_t nrf_enable_AA_pipe_custom(uint8_t pipe_num);
uint8_t nrf_check_payload_pipeline(void);
void nrf_ptx(void);
void nrf_prx(void);
void nrf_power_up(void);
void nrf_power_down(void);
void nrf_read_all_registers(void);

/*
 * COMMANDS
 */

#define NRF_CMD_R_REGISTER			0b00000000
#define NRF_CMD_W_REGISTER			0b00100000
#define NRF_CMD_R_RX_PAYLOAD		0b01100001
#define NRF_CMD_W_TX_PAYLOAD		0b10100000
#define NRF_CMD_FLUSH_TX			0b11100001
#define NRF_CMD_FLUSH_RX			0b11100010
#define NRF_CMD_REUSE_TX_PL			0b11100011
#define NRF_CMD_R_RX_PL_WID			0b01100000
#define NRF_CMD_W_ACK_PAYLOAD		0b10101000
#define NRF_CMD_W_TX_PAYLOAD_NOACK	0b10110000
#define NRF_CMD_NOP					0b11111111

/*
 * REGISTERS
 */

#define NRF_REG_CONFIG				0x00
#define NRF_REG_EN_AA				0x01
#define NRF_REG_EN_RXADDR			0x02
#define NRF_REG_SETUP_AW			0x03
#define NRF_REG_SETUP_RETR			0x04
#define NRF_REG_RF_CH				0x05
#define NRF_REG_RF_SETUP			0x06
#define NRF_REG_STATUS				0x07
#define NRF_REG_OBSERVE_TX			0x08
#define NRF_REG_RPD					0x09
#define NRF_REG_RX_ADDR_P0			0x0A
#define NRF_REG_RX_ADDR_P1			0x0B
#define NRF_REG_RX_ADDR_P2			0x0C
#define NRF_REG_RX_ADDR_P3			0x0D
#define NRF_REG_RX_ADDR_P4			0x0E
#define NRF_REG_RX_ADDR_P5			0x0F
#define NRF_REG_TX_ADDR				0x10
#define NRF_REG_RX_PW_P0			0x11
#define NRF_REG_RX_PW_P1			0x12
#define NRF_REG_RX_PW_P2			0x13
#define NRF_REG_RX_PW_P3			0x14
#define NRF_REG_RX_PW_P4			0x15
#define NRF_REG_RX_PW_P5			0x16
#define NRF_REG_FIFO_STATUS			0x17
#define NRF_REG_DYNPD				0x1C
#define NRF_REG_FEATURE				0x1D

/*
 * BITS
 */

#define PRIM_RX		0
#define PWR_UP		1
#define CRCO		2
#define EN_CRC		3
#define MASK_MAX_RT	4
#define MASK_TX_DS	5
#define MASK_RX_DR	6
#define AUT_RET_CNT	0
#define AUT_RET_DEL	4
#define RF_DR_LOW	5
#define RF_DR_HIGH	3
#define MAX_RT		4
#define TX_DS		5
#define RX_DR		6
#define RPD			0
#define RX_EMPTY	0
#define RX_FULL		1
#define TX_EMPTY	4
#define TX_FULL		5
#define EN_DPL		2
#define EN_ACK_PAY	1
#define EN_DYN_ACK	0

/*
 * MAGIC NUMBERS
 */

#define COMMAND_LENGTH	1
#define ADDRESS_5_BYTES	5

#endif /* NRF24L01_NRF24L01_H_ */
