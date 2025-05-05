/*
 * nrf24l01.c
 *
 *  Created on: Apr 14, 2025
 *      Author: Juliano
 */

#include "nrf24l01.h"

NRF_Settings nrf;

/*
 * HAL BASED SPI TRANSMISSION
 */

static HAL_StatusTypeDef spi_transmit(SPI_HandleTypeDef *spi, uint8_t *tx, uint8_t len, uint32_t timeout){
	return HAL_SPI_Transmit(spi, tx, len, timeout);
}

static HAL_StatusTypeDef spi_receive(SPI_HandleTypeDef *spi, uint8_t *rx, uint8_t len, uint32_t timeout){
	return HAL_SPI_Receive(spi, rx, len, timeout);
}

static HAL_StatusTypeDef spi_transmit_receive(SPI_HandleTypeDef *spi, uint8_t *tx, uint8_t *rx, uint8_t len, uint32_t timeout) {
	return HAL_SPI_TransmitReceive(spi, tx, rx, len, timeout);
}

static HAL_StatusTypeDef nrf_set_rx_addresses(SPI_HandleTypeDef *spi){
	HAL_StatusTypeDef result;

	uint8_t addr_check[ADDRESS_5_BYTES] = {0};
	uint8_t command = 0;

    uint8_t addr_p0[COMMAND_LENGTH + ADDRESS_5_BYTES] = {(NRF_CMD_W_REGISTER | NRF_REG_RX_ADDR_P0), 0xE7, 0xE7, 0xE7, 0xE7, 0xE7}; // W_REGISTER | RX_ADDR_P0
    CSN_LOW;
    result = spi_transmit(spi, addr_p0, COMMAND_LENGTH + ADDRESS_5_BYTES, SPI_TRANSMISSION_TIMEOUT);
    CSN_HIGH;
    if (result != HAL_OK) return result;

    command = (NRF_CMD_R_REGISTER | NRF_REG_RX_ADDR_P0);
    CSN_LOW;
    result = spi_transmit(spi, &command, COMMAND_LENGTH, SPI_TRANSMISSION_TIMEOUT);
    result = spi_receive(spi, addr_check, ADDRESS_5_BYTES, SPI_TRANSMISSION_TIMEOUT);
    CSN_HIGH;
    if (result != HAL_OK) return result;

    uint8_t addr_p1[COMMAND_LENGTH + ADDRESS_5_BYTES] = {(NRF_CMD_W_REGISTER | NRF_REG_RX_ADDR_P1), 0xC2, 0xC2, 0xC2, 0xC2, 0xC2}; // W_REGISTER | RX_ADDR_P1
    CSN_LOW;
    result = spi_transmit(spi, addr_p1, COMMAND_LENGTH + ADDRESS_5_BYTES, SPI_TRANSMISSION_TIMEOUT);
    CSN_HIGH;
    if (result != HAL_OK) return result;

    command = (NRF_CMD_R_REGISTER | NRF_REG_RX_ADDR_P1);
    CSN_LOW;
    result = spi_transmit(spi, &command, COMMAND_LENGTH, SPI_TRANSMISSION_TIMEOUT);
    result = spi_receive(spi, addr_check, ADDRESS_5_BYTES, SPI_TRANSMISSION_TIMEOUT);
    CSN_HIGH;
    if (result != HAL_OK) return result;

    /*
    // Pipe 2: 1-byte address
    uint8_t addr_p2[2] = {(NRF_CMD_W_REGISTER | NRF_REG_RX_ADDR_P2), 0xC3}; // W_REGISTER | RX_ADDR_P2
    CSN_LOW;
    result = spi_transmit(spi, addr_p2, 2, SPI_TRANSMISSION_TIMEOUT);
    CSN_HIGH;
    if (result != HAL_OK) return result;

    // Pipe 3: 1-byte address
    uint8_t addr_p3[2] = {(NRF_CMD_W_REGISTER | NRF_REG_RX_ADDR_P3), 0xC4}; // W_REGISTER | RX_ADDR_P3
    CSN_LOW;
    result = spi_transmit(spi, addr_p3, 2, SPI_TRANSMISSION_TIMEOUT);
    CSN_HIGH;
    if (result != HAL_OK) return result;

    // Pipe 4: 1-byte address
    uint8_t addr_p4[2] = {(NRF_CMD_W_REGISTER | NRF_REG_RX_ADDR_P4), 0xC5}; // W_REGISTER | RX_ADDR_P4
    CSN_LOW;
    result = spi_transmit(spi, addr_p4, 2, SPI_TRANSMISSION_TIMEOUT);
    CSN_HIGH;
    if (result != HAL_OK) return result;

    // Pipe 5: 1-byte address
    uint8_t addr_p5[2] = {(NRF_CMD_W_REGISTER | NRF_REG_RX_ADDR_P5), 0xC6}; // W_REGISTER | RX_ADDR_P5
    CSN_LOW;
    result = spi_transmit(spi, addr_p5, 2, SPI_TRANSMISSION_TIMEOUT);
    CSN_HIGH;
    if (result != HAL_OK) return result;
    */
    return result;
}

uint8_t nrf_send_command(uint8_t cmd){
	uint8_t status = 0;

	CE_LOW;
	CSN_LOW;

	(void) spi_transmit_receive(SPI, &cmd, &status, COMMAND_LENGTH, SPI_TRANSMISSION_TIMEOUT);

	CSN_HIGH;
	CE_HIGH;

	return status;
}

uint8_t nrf_read_command(uint8_t cmd){
	uint8_t result = 0;

	CE_LOW;
	CSN_LOW;

	(void) spi_transmit(SPI, &cmd, COMMAND_LENGTH, SPI_TRANSMISSION_TIMEOUT);
	(void) spi_receive(SPI, &result, COMMAND_LENGTH, SPI_TRANSMISSION_TIMEOUT);

	CSN_HIGH;
	CE_HIGH;

	return result;
}

uint8_t nrf_read_register(uint8_t reg){
	uint8_t status = 0;
	uint8_t reg_val = 0;
	uint8_t cmd = NRF_CMD_R_REGISTER | reg;

	CSN_LOW;

	(void) spi_transmit_receive(SPI, &cmd, &status, COMMAND_LENGTH, SPI_TRANSMISSION_TIMEOUT);
	(void) spi_receive(SPI, &reg_val, COMMAND_LENGTH, SPI_TRANSMISSION_TIMEOUT);

	CSN_HIGH;

	return reg_val;
}

uint8_t nrf_write_register(uint8_t reg, uint8_t val){
	uint8_t status = 0;

	reg = NRF_CMD_W_REGISTER | reg;

	CSN_LOW;

	(void) spi_transmit_receive(SPI, &reg, &status, COMMAND_LENGTH, SPI_TRANSMISSION_TIMEOUT);
	(void) spi_transmit(SPI, &val, COMMAND_LENGTH, SPI_TRANSMISSION_TIMEOUT);

	CSN_HIGH;

	return status;
}

uint8_t nrf_transmit(uint8_t *buffer, uint8_t len){
	uint8_t status = 0;
	uint8_t command = NRF_CMD_W_TX_PAYLOAD;
	CSN_LOW;
	(void) spi_transmit_receive(SPI, &command, &status, COMMAND_LENGTH, SPI_TRANSMISSION_TIMEOUT);
	(void) spi_transmit(SPI, buffer, len, SPI_TRANSMISSION_TIMEOUT);
	CSN_HIGH;
	return status;
}

uint8_t nrf_receive(uint8_t *buffer){
	uint8_t len = 0;
	uint8_t status = 0;
	uint8_t command = NRF_CMD_R_RX_PAYLOAD;

	if(nrf.payload_setting == _PAYLOAD_DYNAMIC){
		len = nrf_read_command(NRF_CMD_R_RX_PL_WID);
		if(len > 32)	nrf_flush_rx_fifo();
	}
	else{
		len = nrf.payload_length;
	}

	CSN_LOW;
	(void) spi_transmit_receive(SPI, &command, &status, COMMAND_LENGTH, SPI_TRANSMISSION_TIMEOUT);
	(void) spi_receive(SPI, buffer, len, SPI_TRANSMISSION_TIMEOUT);
	CSN_HIGH;
	return status;
}

uint8_t nrf_rx_buffer_ready(void){
	HAL_SPI_StateTypeDef spi_state = HAL_SPI_GetState(SPI);
	uint8_t rx_full = ((nrf_ret_fifo_status() >> RX_FULL) & 1);

	if(spi_state == HAL_SPI_STATE_READY && rx_full)	return 1;

	return 0;
}

uint8_t nrf_flush_tx_fifo(void){
	nrf_send_command(NRF_CMD_FLUSH_TX);
	return ((nrf_read_register(NRF_REG_FIFO_STATUS) >> TX_FULL) & 1);
}

uint8_t nrf_flush_rx_fifo(void){
	nrf_send_command(NRF_CMD_FLUSH_RX);
	return ((nrf_read_register(NRF_REG_FIFO_STATUS) >> RX_FULL) & 1);
}

void nrf_reset(void){
	CSN_HIGH;
	CE_LOW;

	(void) nrf_send_command(NRF_CMD_FLUSH_TX);
	(void) nrf_send_command(NRF_CMD_FLUSH_RX);

	(void) nrf_set_rx_addresses(SPI);

	(void) nrf_write_register(NRF_REG_CONFIG, 0x08);
	(void) nrf_write_register(NRF_REG_EN_AA, 0x3F);
	(void) nrf_write_register(NRF_REG_EN_RXADDR, 0x03);
	(void) nrf_write_register(NRF_REG_SETUP_AW, 0x03);
	(void) nrf_write_register(NRF_REG_SETUP_RETR, 0x03);
	(void) nrf_write_register(NRF_REG_RF_CH, 0x02);
	(void) nrf_write_register(NRF_REG_RF_SETUP, 0x0E);
	(void) nrf_write_register(NRF_REG_STATUS, 0x70);
	(void) nrf_write_register(NRF_REG_RX_PW_P0, 0x00);
	(void) nrf_write_register(NRF_REG_RX_PW_P1, 0x00);
	(void) nrf_write_register(NRF_REG_RX_PW_P2, 0x00);
	(void) nrf_write_register(NRF_REG_RX_PW_P3, 0x00);
	(void) nrf_write_register(NRF_REG_RX_PW_P4, 0x00);
	(void) nrf_write_register(NRF_REG_RX_PW_P5, 0x00);
	(void) nrf_write_register(NRF_REG_DYNPD, 0x00);
	(void) nrf_write_register(NRF_REG_FEATURE, 0x00);

	nrf.mode = _MODE_RX;
	nrf.payload_setting = _PAYLOAD_STATIC;
	nrf.frequency = 2402;
	nrf.power = _PWR_0DBM;
	nrf.speed = _SPEED_2MBPS;
}

uint8_t nrf_init(NRF_MODE mode, NRF_PAYLOAD pd, uint16_t freq, NRF_OUTPUT_PWR pwr, NRF_OUTPUT_SPEED speed, uint8_t pd_len){
	uint8_t result = 0;

	nrf_reset();

	nrf.mode = mode;
	nrf.payload_setting = pd;
	nrf.payload_length = pd_len;
	nrf.frequency = freq;
	nrf.power = pwr;
	nrf.speed = speed;

	(void) nrf_flush_tx_fifo();
	(void) nrf_flush_rx_fifo();

	(void) nrf_set_frequency(freq);
	(void) nrf_set_speed(speed);
	(void) nrf_set_power(pwr);

	(void) nrf_enable_AA_pipes(2);

	(void) nrf_set_addr_width(5);
	(void) nrf_set_crc_scheme(_CRC_2BYTE);

	(void) nrf_set_auto_ret_count(3);
	(void) nrf_set_auto_ret_delay(1000);

	nrf_power_up();

	switch(mode){
	case _MODE_RX:
		nrf_prx();
		break;
	case _MODE_TX:
		nrf_ptx();
		break;
	}

	switch(pd){
	case _PAYLOAD_STATIC:
		(void) nrf_set_static_rx_width(0, pd_len);
		(void) nrf_set_static_rx_width(1, pd_len);
		break;
	case _PAYLOAD_DYNAMIC:
		(void) nrf_enable_DPL(enabled);
		(void) nrf_enable_ACK_PAY(enabled);
		(void) nrf_enable_DPL_pipes(2);
		break;
	}

	CE_HIGH;

	return result;
}

uint8_t nrf_set_frequency(uint16_t freq){
	uint8_t reg = nrf_read_register(NRF_REG_RF_CH) & 0x00;
	if(freq < 2400)	freq = 2400;
	freq -= 2400;
	if(freq > 1025)	freq = 1025;
	nrf.frequency = freq + 2400;

	reg |= freq;
	(void) nrf_write_register(NRF_REG_RF_CH, reg);
	return reg;
}

uint8_t nrf_set_power(NRF_OUTPUT_PWR power){
	nrf.power = power;
	uint8_t reg = nrf_read_register(NRF_REG_RF_SETUP) & 0xF9;
	reg |= (power << 1);
	(void) nrf_write_register(NRF_REG_RF_SETUP, reg);
	return reg;
}

uint8_t nrf_set_speed(NRF_OUTPUT_SPEED speed){
	nrf.speed = speed;
	uint8_t reg = nrf_read_register(NRF_REG_RF_SETUP) & 0xD7;

	switch(speed){
	case _SPEED_250KBPS:
		reg |= (1 << RF_DR_LOW);
		break;
	case _SPEED_1MBPS:
		break;
	case _SPEED_2MBPS:
		reg |= (1 << RF_DR_HIGH);
		break;
	}

	(void) nrf_write_register(NRF_REG_RF_SETUP, reg);
	return reg;
}

uint8_t nrf_set_static_rx_width(uint8_t pipe_num, uint8_t width){
	if(pipe_num > 5)	pipe_num = 5;
	uint8_t  reg = nrf_read_register(NRF_REG_RX_PW_P0 + pipe_num) & 0x3F;
	reg |= width;
	(void) nrf_write_register(NRF_REG_RX_PW_P0 + pipe_num, reg);
	return reg;
}

uint8_t nrf_set_addr_width(uint8_t width){
	if(width > 5)	width = 5;

	uint8_t reg = nrf_read_register(NRF_REG_SETUP_AW) & 0x00;
	reg |= width - 2;
	(void) nrf_write_register(NRF_REG_SETUP_AW, reg);
	return reg;
}

uint8_t nrf_set_crc_scheme(NRF_CRC bytes){
	uint8_t reg = nrf_read_register(NRF_REG_CONFIG) & 0xFB;

	reg |= (bytes << CRCO);
	(void) nrf_write_register(NRF_REG_CONFIG, reg);
	return reg;
}

uint8_t nrf_set_auto_ret_count(uint8_t ret){
	uint8_t reg = nrf_read_register(NRF_REG_SETUP_RETR) & 0xF0;

	if(ret > 15)	ret = 15;
	reg |= ret;

	(void) nrf_write_register(NRF_REG_SETUP_RETR, reg);
	return reg;
}

uint8_t nrf_set_auto_ret_delay(uint16_t delay_us){
	uint8_t reg = nrf_read_register(NRF_REG_SETUP_RETR) & 0x0F;

	if(delay_us > 4000)	delay_us = 4000;

	delay_us = (delay_us - 250) / 250;

	reg |= (delay_us << AUT_RET_DEL);

	(void) nrf_write_register(NRF_REG_SETUP_RETR, reg);
	return reg;
}

uint8_t nrf_disable_itr(uint8_t rx_dr, uint8_t tx_ds, uint8_t max_rt){
	uint8_t reg = nrf_read_register(NRF_REG_CONFIG) & 0x8F;

	reg |= (rx_dr << MASK_RX_DR);
	reg |= (tx_ds << MASK_TX_DS);
	reg |= (max_rt << MASK_MAX_RT);

	(void) nrf_write_register(NRF_REG_CONFIG, reg);
	return reg;
}

uint8_t nrf_check_rx_dr(void){
	return (nrf_read_register(NRF_REG_STATUS) >> RX_DR) & 0x01;
}

uint8_t nrf_check_tx_ds(void){
	return (nrf_read_register(NRF_REG_STATUS) >> TX_DS) & 0x01;
}

uint8_t nrf_check_max_rt(void){
	return (nrf_read_register(NRF_REG_STATUS) >> MAX_RT) & 0x01;
}

void nrf_reset_rx_dr(void){
	uint8_t reg = nrf_read_register(NRF_REG_STATUS);
	reg |= (1 << RX_DR);
	(void) nrf_write_register(NRF_REG_STATUS, reg);
}

void nrf_reset_tx_ds(void){
	uint8_t reg = nrf_read_register(NRF_REG_STATUS);
	reg |= (1 << TX_DS);
	(void) nrf_write_register(NRF_REG_STATUS, reg);
}

void nrf_reset_max_rt(void){
	uint8_t reg = nrf_read_register(NRF_REG_STATUS);
	reg |= (1 << MAX_RT);
	(void) nrf_write_register(NRF_REG_STATUS, reg);
}

uint8_t nrf_check_ret_count(void){
	return (nrf_read_register(NRF_REG_OBSERVE_TX) & 0x0F);
}

uint8_t nrf_check_lost_count(void){
	return ((nrf_read_register(NRF_REG_OBSERVE_TX) >> 4) & 0x0F);
}

uint8_t nrf_power_detector(void){
	return (nrf_read_register(NRF_REG_RPD) & 0x01);
}

uint8_t nrf_ret_fifo_status(void){
	return (nrf_read_register(NRF_REG_FIFO_STATUS));
}

uint8_t nrf_enable_DPL(MODE mode){
	uint8_t reg = nrf_read_register(NRF_REG_FEATURE) & 0xFB;
	reg |= (mode << 2);
	(void) nrf_write_register(NRF_REG_FEATURE, reg);
	return reg;
}

uint8_t nrf_enable_DPL_pipes(uint8_t num_of_pipes){
	uint8_t reg = nrf_read_register(NRF_REG_DYNPD);

	if(num_of_pipes > 6)	num_of_pipes = 6;

	for(int i = 0; i < num_of_pipes; i++){
		reg |= (1 << i);
	}

	(void) nrf_write_register(NRF_REG_DYNPD, reg);
	return reg;
}

uint8_t nrf_enable_DPL_pipe_custom(uint8_t pipe_num){
	uint8_t reg = nrf_read_register(NRF_REG_DYNPD);
	reg |= (1 << pipe_num);
	(void) nrf_write_register(NRF_REG_DYNPD, reg);
	return reg;
}

uint8_t nrf_enable_ACK_PAY(MODE mode){
	uint8_t reg = nrf_read_register(NRF_REG_FEATURE) & 0xFD;
	reg |= (mode << 1);
	(void) nrf_write_register(NRF_REG_FEATURE, reg);
	return reg;
}

uint8_t nrf_enable_AA_pipes(uint8_t num_of_pipes){
	uint8_t reg = 0;

	if(num_of_pipes > 6)	num_of_pipes = 6;

	for(int i = 0; i < num_of_pipes; i++){
		reg |= (1 << i);
	}

	(void) nrf_write_register(NRF_REG_EN_AA, reg);
	return reg;
}

uint8_t nrf_enable_AA_pipe_custom(uint8_t pipe_num){
	uint8_t reg = 0;
	reg |= (1 << pipe_num);
	(void) nrf_write_register(NRF_REG_EN_AA, reg);
	return reg;
}

uint8_t nrf_check_payload_pipeline(void){
	uint8_t result = 0;
	result = nrf_read_register(NRF_REG_STATUS) & 0x07;
	return result;
}

void nrf_ptx(void){
	nrf.mode = _MODE_TX;
	uint8_t reg = nrf_read_register(NRF_REG_CONFIG) & 0xFE;
	nrf_write_register(NRF_REG_CONFIG, reg);
}

void nrf_prx(void){
	nrf.mode = _MODE_RX;
	uint8_t reg = nrf_read_register(NRF_REG_CONFIG) & 0xFE;
	reg |= 1;
	(void) nrf_write_register(NRF_REG_CONFIG, reg);
}

void nrf_power_up(void){
	uint8_t reg = nrf_read_register(NRF_REG_CONFIG) & 0xFD;
	reg |= (1 << 1);
	(void) nrf_write_register(NRF_REG_CONFIG, reg);
}

void nrf_power_down(void){
	uint8_t reg = nrf_read_register(NRF_REG_CONFIG) & 0xFD;
	(void) nrf_write_register(NRF_REG_CONFIG, reg);
}

void nrf_read_all_registers(void){
	uint8_t reg = 0;
	//breakpoint here
	reg = nrf_read_register(NRF_REG_CONFIG);
	reg = nrf_read_register(NRF_REG_EN_AA);
	reg = nrf_read_register(NRF_REG_EN_RXADDR);
	reg = nrf_read_register(NRF_REG_SETUP_AW);
	reg = nrf_read_register(NRF_REG_SETUP_RETR);
	reg = nrf_read_register(NRF_REG_RF_CH);
	reg = nrf_read_register(NRF_REG_RF_SETUP);
	reg = nrf_read_register(NRF_REG_STATUS);
	reg = nrf_read_register(NRF_REG_OBSERVE_TX);
	reg = nrf_read_register(NRF_REG_FIFO_STATUS);
	reg = nrf_read_register(NRF_REG_DYNPD);
	reg = nrf_read_register(NRF_REG_FEATURE);
	reg += 0xFF;	//warning eliminator
}

