/*
 * oled.c
 *
 *  Author: Mert
 */ 

#include "i2c.h"
#include "oled.h"

void oled_command(uint8_t command) {
	i2c_start();
	i2c_write(OLED_ADDR << 1); // Address the OLED display
	i2c_write(0x00);           // Co = 0, D/C# = 0 (command mode)
	i2c_write(command);        // Send the command
	i2c_stop();
}

void oled_data(uint8_t data) {
	i2c_start();
	i2c_write(OLED_ADDR << 1); // Address the OLED display
	i2c_write(0x40);           // Co = 0, D/C# = 1 (data mode)
	i2c_write(data);           // Send the data
	i2c_stop();
}

void oled_init(void) {
	oled_command(0xAE); // Display off
	oled_command(0x20); // Set Memory Addressing Mode
	oled_command(0x10); // Horizontal Addressing Mode
	oled_command(0xB0); // Set Page Start Address for Page Addressing Mode
	oled_command(0xC8); // Set COM Output Scan Direction
	oled_command(0x00); // Set low column address
	oled_command(0x10); // Set high column address
	oled_command(0x40); // Set start line address
	oled_command(0x81); // Set contrast control register
	oled_command(0xFF);
	oled_command(0xA1); // Set segment re-map
	oled_command(0xA6); // Set normal display
	oled_command(0xA8); // Set multiplex ratio
	oled_command(0x3F); // 1 to 64
	oled_command(0xA4); // Output follows RAM content
	oled_command(0xD3); // Set display offset
	oled_command(0x00); // No offset
	oled_command(0xD5); // Set display clock divide ratio/oscillator frequency
	oled_command(0xF0); // Set divide ratio
	oled_command(0xD9); // Set pre-charge period
	oled_command(0x22); // Phase 1: 2 DCLKs, Phase 2: 2 DCLKs
	oled_command(0xDA); // Set COM pins hardware configuration
	oled_command(0x12);
	oled_command(0xDB); // Set VCOMH deselect level
	oled_command(0x20); // 0.77xVcc
	oled_command(0x8D); // Set DC-DC enable
	oled_command(0x14);
	oled_command(0xAF); // Turn on OLED panel
}

void oled_clear(void) {
	for (uint16_t i = 0; i < 1024; i++) {
		oled_data(0x00);
	}
}

void draw_pixel(uint8_t x, uint8_t y) {
	oled_command(0xB0 + (y >> 3)); // Set page address
	oled_command(x & 0x0F);        // Set lower column address
	oled_command(0x10 | (x >> 4)); // Set higher column address
	oled_data(1 << (y % 8));       // Draw pixel
}

void draw_snake(Snake *snake) {
	for (int i = 0; i < snake->length; i++) {
		draw_pixel(snake->body[i].x, snake->body[i].y);
	}
}

void draw_food(Food *food) {
	uint8_t x = food->position.x;
	uint8_t y = food->position.y;

	// Draw a 3x3 block centered at (x, y)
	for (int8_t dx = -1; dx <= 1; dx++) {
		for (int8_t dy = -1; dy <= 1; dy++) {
			draw_pixel(x + dx, y + dy); // Set the pixel in the buffer
		}
	}
}

