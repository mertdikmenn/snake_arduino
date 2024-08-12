#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdlib.h>
#include "i2c.h"
#include "oled.h"

#define MAX_SNAKE_LENGTH 100

#define BUTTON_UP    PIND5
#define BUTTON_DOWN  PIND6
#define BUTTON_LEFT  PIND7
#define BUTTON_RIGHT PIND4

// Direction macros
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

//BUZZER_PIN PB1
void init_buzzer(void) {
	DDRB |= (1 << DDB1); // Set BUZZER_PIN (PB1) as output

	// Set up Timer 1 for Fast PWM mode
	TCCR1A |= (1 << COM1A0);   // Toggle OC1A on Compare Match
	TCCR1B |= (1 << WGM12) | (1 << CS11); // CTC mode, prescaler 8
}

void play_buzzer(uint16_t frequency, uint16_t duration) {
	uint16_t ocr_value = (F_CPU / (2 * 8 * frequency)) - 1; // Calculate the OCR value

	OCR1A = ocr_value; // Set the compare match value for Timer 1

	// Wait for the specified duration
	for(uint8_t t = 0; t < duration; t++){ // Duration of the tone: durx100ms
		 _delay_ms(100);
	}

	// Turn off the buzzer
	TCCR1A &= ~(1 << COM1A0); // Stop toggling OC1A
}

void init_interrupts(void) {
	// Enable Pin Change Interrupts for PORTD4 to PORTD7
	PCICR |= (1 << PCIE2);     // Enable Pin Change Interrupts for PCINT[23:16]
	PCMSK2 |= (1 << PCINT20) | (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23);  // Enable interrupts for PD4, PD5, PD6, PD7

	sei(); // Enable global interrupts
}

volatile uint8_t new_direction = RIGHT;  // Default direction

ISR(PCINT2_vect) {
	// Check which button was pressed
	if (!(PIND & (1 << BUTTON_UP))) {
		if (new_direction != DOWN) new_direction = UP;
	} 
	else if (!(PIND & (1 << BUTTON_DOWN))) {
		if (new_direction != UP) new_direction = DOWN;
	}
	else if (!(PIND & (1 << BUTTON_LEFT))) {
		if (new_direction != RIGHT) new_direction = LEFT;
	}
	else if (!(PIND & (1 << BUTTON_RIGHT))) {
		if (new_direction != LEFT) new_direction = RIGHT;
	}
}

void update_direction(Snake *snake) {
	snake->direction = new_direction;
}

void init_buttons(void) {
	DDRD &= ~((1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT)); // Set as input
	PORTD |= (1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT);  // Enable pull-up
}

void move_snake(Snake *snake) {
	for (int i = snake->length - 1; i > 0; i--) {
		snake->body[i] = snake->body[i - 1];
	}
	switch (snake->direction) {
		case UP: snake->body[0].y--; break;
		case RIGHT: snake->body[0].x++; break;
		case DOWN: snake->body[0].y++; break;
		case LEFT: snake->body[0].x--; break;
	}
}

uint8_t check_collision(Snake *snake, Food *food) {
    // Check wall collision
    if (snake->body[0].x >= 128 || snake->body[0].y >= 64) {
		play_buzzer(200, 1000); // Play a low tone for game over (200 Hz for 1 second)
		return 1;
	}

    // Check self collision
    for (int i = 1; i < snake->length; i++) {
	    if (snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y) {
			play_buzzer(200, 10); // Play a low tone for game over (200 Hz for 10*100 ms (1 second))
			return 1;
		}
    }

    // Check food collision (within the 3x3 block)
    if (snake->body[0].x >= food->position.x - 1 && snake->body[0].x <= food->position.x + 1 &&
    snake->body[0].y >= food->position.y - 1 && snake->body[0].y <= food->position.y + 1) {
	    snake->length += 5; // Increase snake length
	    food->position.x = (rand() % (128 - 2)) + 1; // Ensure the new food position is within bounds
	    food->position.y = (rand() % (64 - 2)) + 1;  // Ensure the new food position is within bounds
		play_buzzer(1000, 1); // Play a high-pitched tone when food is eaten (1000 Hz for 1*100 ms)
    }

    return 0;
}

int main(void) {
	i2c_init();
	oled_init();
	_delay_ms(10000);
	oled_clear();
	init_buttons();
	init_interrupts();

	GameState game;
	game.snake.length = 5;
	game.snake.body[0].x = 64;
	game.snake.body[0].y = 32;
	game.snake.direction = RIGHT;
	game.food.position.x = 80;
	game.food.position.y = 32;
	game.game_over = 0;

	while (1) {
		if (!game.game_over) {
			update_direction(&game.snake);
			move_snake(&game.snake);
			game.game_over = check_collision(&game.snake, &game.food);
			oled_clear();
			draw_snake(&game.snake);
			draw_food(&game.food);
			_delay_ms(100);
		}
	}

	return 0;
}
