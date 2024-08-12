/*
 * i2c.h
 *
 *  Author: Mert
 */ 


#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(uint8_t data);

#endif // I2C_H
