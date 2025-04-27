#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdbool.h>

#define GAME_WIDTH 10
#define GAME_HEIGHT 20
#define GAME_OFFSET_X 5
#define GAME_OFFSET_Y 2

extern const uint8_t TETROMINOES[7][4][4];
extern const uint8_t PIECE_COLORS[7];

extern uint8_t game_board[GAME_HEIGHT][GAME_WIDTH];
extern int8_t current_piece_x;
extern int8_t current_piece_y;
extern uint8_t current_piece_type;
extern uint8_t current_piece_rotation;
extern uint8_t next_piece_type;
extern bool game_over;
extern uint32_t score;
extern uint32_t level;
extern uint32_t lines_cleared;

void game_main();
void reset_game();
void new_piece();
bool check_collision(int8_t offset_x, int8_t offset_y, uint8_t rotation);
void rotate_piece();
void lock_piece();
void clear_lines();

unsigned int rand();
void srand(unsigned int new_seed);
void sprintf(char* buffer, const char* format, uint32_t value);

#endif // GAME_H
