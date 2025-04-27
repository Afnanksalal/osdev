#include "rendering.h"
#include "game.h"

void draw_char(uint8_t x, uint8_t y, char c, uint8_t color) {
    if (x >= WIDTH || y >= HEIGHT) return;
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

void draw_borders() {
    for (int y = GAME_OFFSET_Y; y < GAME_OFFSET_Y + GAME_HEIGHT; y++) {
        draw_char(GAME_OFFSET_X - 1, y, 0xB3, 0x07);
        draw_char(GAME_OFFSET_X + GAME_WIDTH, y, 0xB3, 0x07);
    }

    for (int x = GAME_OFFSET_X - 1; x <= GAME_OFFSET_X + GAME_WIDTH; x++) {
        draw_char(x, GAME_OFFSET_Y - 1, 0xC4, 0x07);
        draw_char(x, GAME_OFFSET_Y + GAME_HEIGHT, 0xC4, 0x07);
    }

    draw_char(GAME_OFFSET_X - 1, GAME_OFFSET_Y - 1, 0xDA, 0x07);
    draw_char(GAME_OFFSET_X + GAME_WIDTH, GAME_OFFSET_Y - 1, 0xBF, 0x07);
    draw_char(GAME_OFFSET_X - 1, GAME_OFFSET_Y + GAME_HEIGHT, 0xC0, 0x07);
    draw_char(GAME_OFFSET_X + GAME_WIDTH, GAME_OFFSET_Y + GAME_HEIGHT, 0xD9, 0x07);

    int info_x = GAME_OFFSET_X + GAME_WIDTH + 3;
    for (int y = GAME_OFFSET_Y; y < GAME_OFFSET_Y + 14; y++) {
        draw_char(info_x - 1, y, 0xB3, 0x07);
        draw_char(info_x + 15, y, 0xB3, 0x07);
    }

    for (int x = info_x - 1; x <= info_x + 15; x++) {
        draw_char(x, GAME_OFFSET_Y - 1, 0xC4, 0x07);
        draw_char(x, GAME_OFFSET_Y + 13, 0xC4, 0x07);
    }

    draw_char(info_x - 1, GAME_OFFSET_Y - 1, 0xDA, 0x07);
    draw_char(info_x + 15, GAME_OFFSET_Y - 1, 0xBF, 0x07);
    draw_char(info_x - 1, GAME_OFFSET_Y + 13, 0xC0, 0x07);
    draw_char(info_x + 15, GAME_OFFSET_Y + 13, 0xD9, 0x07);
}

void draw_info_panel() {
    int info_x = GAME_OFFSET_X + GAME_WIDTH + 3;
    int info_y = GAME_OFFSET_Y;

    for (int y = info_y; y < info_y + 13; y++) {
        for (int x = info_x; x < info_x + 15; x++) {
            draw_char(x, y, ' ', 0x07);
        }
    }

    const char* title = "TETRIS";
    for (int i = 0; i < 6; i++) {
        draw_char(info_x + 5 + i, info_y, title[i], 0x0E);
    }

    const char* score_label = "SCORE:";
    for(int i = 0; i < 6; i++) draw_char(info_x + i, info_y + 2, score_label[i], 0x07);

    char score_str[10];
    sprintf(score_str, "%u", score);
    for (int i = 0; score_str[i] != '\0'; i++) {
        draw_char(info_x + 7 + i, info_y + 2, score_str[i], 0x07);
    }

    const char* level_label = "LEVEL:";
    for(int i = 0; i < 6; i++) draw_char(info_x + i, info_y + 4, level_label[i], 0x07);
    char level_str[10];
    sprintf(level_str, "%u", level);
    for (int i = 0; level_str[i] != '\0'; i++) {
        draw_char(info_x + 7 + i, info_y + 4, level_str[i], 0x07);
    }

    const char* lines_label = "LINES:";
    for(int i = 0; i < 6; i++) draw_char(info_x + i, info_y + 6, lines_label[i], 0x07);
    char lines_str[10];
    sprintf(lines_str, "%u", lines_cleared);
    for (int i = 0; lines_str[i] != '\0'; i++) {
        draw_char(info_x + 7 + i, info_y + 6, lines_str[i], 0x07);
    }

    const char* next_label = "NEXT:";
    for(int i = 0; i < 5; i++) draw_char(info_x + i, info_y + 8, next_label[i], 0x07);
}

void draw_next_piece() {
    int info_x = GAME_OFFSET_X + GAME_WIDTH + 3;
    int info_y = GAME_OFFSET_Y;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            draw_char(info_x + 6 + x, info_y + 9 + y, ' ', 0x07);
        }
    }

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (TETROMINOES[next_piece_type][y][x]) {
                draw_char(info_x + 6 + x, info_y + 9 + y, 0xDB, PIECE_COLORS[next_piece_type]);
            }
        }
    }
}

void draw_game_board() {
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            if (game_board[y][x]) {
                draw_char(GAME_OFFSET_X + x, GAME_OFFSET_Y + y, 0xDB, PIECE_COLORS[game_board[y][x] - 1]);
            } else {
                draw_char(GAME_OFFSET_X + x, GAME_OFFSET_Y + y, ' ', 0x07);
            }
        }
    }
}

void draw_current_piece() {
    uint8_t rotated_piece[4][4];
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if (current_piece_rotation == 0) rotated_piece[y][x] = TETROMINOES[current_piece_type][y][x];
            else if (current_piece_rotation == 1) rotated_piece[y][x] = TETROMINOES[current_piece_type][3-x][y];
            else if (current_piece_rotation == 2) rotated_piece[y][x] = TETROMINOES[current_piece_type][3-y][3-x];
            else if (current_piece_rotation == 3) rotated_piece[y][x] = TETROMINOES[current_piece_type][x][3-y];
        }
    }

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (rotated_piece[y][x]) {
                int board_x = current_piece_x + x;
                int board_y = current_piece_y + y;
                if (board_x >= 0 && board_x < GAME_WIDTH && board_y >= 0 && board_y < GAME_HEIGHT) {
                    draw_char(GAME_OFFSET_X + board_x, GAME_OFFSET_Y + board_y,
                              0xDB, PIECE_COLORS[current_piece_type]);
                }
            }
        }
    }
}

void show_game_over() {
    clear_screen();

    const char* game_over_msg = "GAME OVER";
    int len = 9;
    int start_x = (WIDTH - len) / 2;
    int start_y = HEIGHT / 2 - 2;

    for (int i = 0; i < len; i++) {
        draw_char(start_x + i, start_y, game_over_msg[i], 0x0C);
    }

    const char* score_msg = "FINAL SCORE: ";
    len = 13;
    start_x = (WIDTH - len) / 2;
    start_y += 2;

    for (int i = 0; i < len; i++) {
        draw_char(start_x + i, start_y, score_msg[i], 0x07);
    }

    char score_str[10];
    sprintf(score_str, "%u", score);
    for (int i = 0; score_str[i] != '\0'; i++) {
        draw_char(start_x + len + i, start_y, score_str[i], 0x07);
    }
}
