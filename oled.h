/*
 * oled.h
 *
 *  Author: Mert
 */ 

#ifndef OLED_H
#define OLED_H

#include <avr/io.h>
#include "game_structs.h"

#define OLED_ADDR 0x3C // I2C address of the SSD1306 OLED display

void oled_command(uint8_t command);
void oled_data(uint8_t data);
void oled_init(void);
void oled_clear(void);
void draw_pixel(uint8_t x, uint8_t y);
void draw_snake(Snake *snake);
void draw_food(Food *food);

#endif // OLED_H

