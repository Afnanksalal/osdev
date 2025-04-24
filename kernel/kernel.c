#include <stdint.h>
#include <stdbool.h>

#define VIDEO_MEMORY (char*)0xb8000
#define WIDTH 80
#define HEIGHT 25

// Snake related variables
static uint8_t snake_x = 10;
static uint8_t snake_y = 10;
static char direction = 'R';
static uint8_t snake_length = 3;
static uint8_t snake_body_x[WIDTH * HEIGHT];
static uint8_t snake_body_y[WIDTH * HEIGHT];

// Food related variables
static uint8_t food_x;
static uint8_t food_y;

// Game state
static bool game_over = false;

// Snake body characters
static char snake_head_char = '>';  // Initial head character
static char snake_body_char = '=';

// Simple rand() function for food generation. Not cryptographically secure.
unsigned int rand() {
    static unsigned int seed = 1;  // Initialize with a seed value

    // Linear Congruential Generator (LCG) parameters
    unsigned int a = 1664525;
    unsigned int c = 1013904223;
    unsigned int m = 0xFFFFFFFF;  // 2^32

    seed = (a * seed + c) & m;
    return seed;
}

void srand(unsigned int new_seed) {
    static unsigned int seed = 1;  // Initialize with default
    seed = new_seed;  // Then assign the new seed value
}

void draw_char(uint8_t x, uint8_t y, char c, uint8_t color) {
    if (x >= WIDTH || y >= HEIGHT) return; // Boundary check

    char* video = VIDEO_MEMORY + 2 * (y * WIDTH + x);
    video[0] = c;
    video[1] = color;
}

void clear_screen() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            draw_char(x, y, ' ', 0x07);
        }
    }
}

unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void update_direction() {
    unsigned char scancode = inb(0x60);
    switch (scancode) {
        case 0x4D: if (direction != 'L') direction = 'R'; break; // Right arrow
        case 0x4B: if (direction != 'R') direction = 'L'; break; // Left arrow
        case 0x48: if (direction != 'D') direction = 'U'; break; // Up arrow
        case 0x50: if (direction != 'U') direction = 'D'; break; // Down arrow
    }

    // Update the head character based on direction
    switch (direction) {
        case 'R': snake_head_char = '>'; break;
        case 'L': snake_head_char = '<'; break;
        case 'U': snake_head_char = '^'; break;
        case 'D': snake_head_char = 'v'; break;
    }
}

void sleep() {
    for (volatile int i = 0; i < 25000000; i++); // Increased sleep for slower speed
}

void generate_food() {
    bool valid_food_position = false;
    while (!valid_food_position) {
        food_x = rand() % WIDTH;
        food_y = rand() % HEIGHT;
        // Make sure food isn't on the snake
        valid_food_position = true;
        for (int i = 0; i < snake_length; i++) {
            if (food_x == snake_body_x[i] && food_y == snake_body_y[i]) {
                valid_food_position = false;
                break;
            }
        }
    }
}

bool check_collision() {
    // Check collision with self only
    for (int i = 1; i < snake_length; i++) {
        if (snake_x == snake_body_x[i] && snake_y == snake_body_y[i]) {
            return true;
        }
    }
    return false;
}

void move_snake() {
    // Store current head position in the body array
    snake_body_x[0] = snake_x;
    snake_body_y[0] = snake_y;

    // Calculate the next head position
    switch (direction) {
        case 'R': snake_x++; break;
        case 'L': snake_x--; break;
        case 'U': snake_y--; break;
        case 'D': snake_y++; break;
    }

    // Wrap around boundaries - Fixed to handle unsigned integers properly
    if (snake_x >= WIDTH) snake_x = 0;
    if (snake_x == 255) snake_x = WIDTH - 1;  // Overflow for uint8_t
    if (snake_y >= HEIGHT) snake_y = 0;
    if (snake_y == 255) snake_y = HEIGHT - 1;  // Overflow for uint8_t

    // Check for food
    bool ate_food = (snake_x == food_x && snake_y == food_y);

    // Move the body (shift all segments)
    for (int i = snake_length - 1; i > 0; i--) {
        snake_body_x[i] = snake_body_x[i - 1];
        snake_body_y[i] = snake_body_y[i - 1];
    }

    // Update the first body segment to follow the head
    snake_body_x[0] = snake_x;
    snake_body_y[0] = snake_y;

    // Handle eating food
    if (ate_food) {
        snake_length++; // Increment length
        generate_food(); // Generate new food
    }

    // Check collision with self AFTER move and wrapping
    if (check_collision()) {
        game_over = true;
    }
}

void draw_snake() {
    // Draw the snake's head
    draw_char(snake_body_x[0], snake_body_y[0], snake_head_char, 0x0A); // Draw the head

    // Draw the snake's body
    for (int i = 1; i < snake_length; i++) {
        draw_char(snake_body_x[i], snake_body_y[i], snake_body_char, 0x0A);
    }
}

void kernel_main() {
    // Initialize random seed
    uint32_t seed = 12345;
    srand(seed);

    clear_screen();

    // Initialize snake body
    snake_x = 10;
    snake_y = 10;
    snake_body_x[0] = snake_x;
    snake_body_y[0] = snake_y;
    snake_body_x[1] = snake_x - 1;
    snake_body_y[1] = snake_y;
    snake_body_x[2] = snake_x - 2;
    snake_body_y[2] = snake_y;

    generate_food();

    while (!game_over) {
        update_direction();
        move_snake();

        if (!game_over) {  // Only redraw if not game over.
            clear_screen();

            // Draw food
            draw_char(food_x, food_y, '*', 0x0C);

            // Draw snake
            draw_snake();
            sleep();
        }
    }

    clear_screen();
    const char* game_over_msg = "Game Over! Score: ";
    int len = 0;
    while (game_over_msg[len] != '\0') {
        len++;
    }
    int start_x = (WIDTH - len) / 2;
    int start_y = HEIGHT / 2;
    for (int i = 0; i < len; i++) {
        draw_char(start_x + i, start_y, game_over_msg[i], 0x0E);
    }

    // Display the score (snake length)
    uint8_t score = snake_length - 3; // starting length is 3
    char score_str[3]; // Max score will be 99 (if HEIGHT and WIDTH are 25 and 80), so 2 digits + null terminator
    int score_len = 0;
    if (score < 10) {
        score_str[0] = score + '0'; // Convert to ASCII character
        score_len = 1;
    } else {
        score_str[0] = (score / 10) + '0';
        score_str[1] = (score % 10) + '0';
        score_len = 2;
    }

    for (int i = 0; i < score_len; i++) {
        draw_char(start_x + len + i, start_y, score_str[i], 0x0E);
    }
}
