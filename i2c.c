/*
 * i2c.c
 *
 *  Author: Mert
 */ 

#include "i2c.h"

void i2c_init(void) {
	TWSR = 0x00; // Set prescaler to zero
	TWBR = 0x0C; // Set SCL frequency to 400kHz
	TWCR = (1 << TWEN); // Enable TWI module
}

void i2c_start(void) {
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));
}

void i2c_stop(void) {
	TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
}

void i2c_write(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));
}
