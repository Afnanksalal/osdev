#ifndef RENDERING_H
#define RENDERING_H

#include <stdint.h>
#include "game.h"

#define VIDEO_MEMORY (char*)0xb8000
#define WIDTH 80
#define HEIGHT 25

void draw_char(uint8_t x, uint8_t y, char c, uint8_t color);
void clear_screen();
void draw_borders();
void draw_info_panel();
void draw_next_piece();
void draw_game_board();
void draw_current_piece();
void show_game_over();

unsigned char inb(unsigned short port);

#endif // RENDERING_H
