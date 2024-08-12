/*
 * game_struct.h
 *
 *  Author: Mert
 */ 


#ifndef GAME_STRUCTS_H
#define GAME_STRUCTS_H

#include <avr/io.h>

typedef struct {
	uint8_t x;
	uint8_t y;
} Point;

typedef struct {
	Point body[100]; // Maximum snake length
	uint8_t length;
	uint8_t direction;
} Snake;

typedef struct {
	Point position;
} Food;

typedef struct {
	Snake snake;
	Food food;
	uint8_t game_over;
} GameState;

#endif // GAME_STRUCTS_H
