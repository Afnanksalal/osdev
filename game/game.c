#include "game.h"
#include "rendering.h"
#include "input.h"

uint8_t game_board[GAME_HEIGHT][GAME_WIDTH] = {0};
int8_t current_piece_x = 0;
int8_t current_piece_y = 0;
uint8_t current_piece_type = 0;
uint8_t current_piece_rotation = 0;
uint8_t next_piece_type = 0;
bool game_over = false;
uint32_t score = 0;
uint32_t level = 1;
uint32_t lines_cleared = 0;

const uint8_t TETROMINOES[7][4][4] = {
    {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}
};

const uint8_t PIECE_COLORS[7] = {
    0x0B, 0x09, 0x0C, 0x0E, 0x0A, 0x0D, 0x0C
};

static unsigned int current_seed = 1;
unsigned int rand() {
    current_seed = (1664525 * current_seed + 1013904223);
    return current_seed;
}

void srand(unsigned int new_seed) {
    current_seed = new_seed;
}

void sprintf(char* buffer, const char* format, uint32_t value) {
    int i = 0;
    if (value == 0) {
        buffer[i++] = '0';
    } else {
        int j = 0;
        char temp[10];
        while (value > 0) {
            temp[j++] = '0' + (value % 10);
            value /= 10;
        }
        while (j > 0) {
            buffer[i++] = temp[--j];
        }
    }
    buffer[i] = '\0';
}

void game_main() {
    reset_game();

    uint32_t tick_count = 0;
    uint32_t drop_speed = 40 - (level * 2);
    if (drop_speed < 8) drop_speed = 8;

    new_piece();
    next_piece_type = rand() % 7;

    while (!game_over) {
        handle_input();

        if (tick_count % drop_speed == 0) {
            if (!check_collision(0, 1, current_piece_rotation)) {
                current_piece_y++;
            } else {
                lock_piece();
                clear_lines();
                new_piece();
            }
        }

        clear_screen();
        draw_borders();
        draw_info_panel();
        draw_next_piece();
        draw_game_board();
        draw_current_piece();

        for (volatile int i = 0; i < 500000; i++);

        tick_count++;
    }

    show_game_over();

    // After game over screen, just halt
    while(1) {
        __asm__ volatile("cli");
        __asm__ volatile("hlt");
    }
}

void reset_game() {
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            game_board[y][x] = 0;
        }
    }
    current_piece_x = 0;
    current_piece_y = 0;
    current_piece_type = 0;
    current_piece_rotation = 0;
    next_piece_type = 0;
    game_over = false;
    score = 0;
    level = 1;
    lines_cleared = 0;

    srand(rand());
    current_piece_type = rand() % 7;
    next_piece_type = rand() % 7;
}

bool check_collision(int8_t offset_x, int8_t offset_y, uint8_t rotation) {
    uint8_t rotated_piece[4][4];
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if (rotation == 0) rotated_piece[y][x] = TETROMINOES[current_piece_type][y][x];
            else if (rotation == 1) rotated_piece[y][x] = TETROMINOES[current_piece_type][3-x][y];
            else if (rotation == 2) rotated_piece[y][x] = TETROMINOES[current_piece_type][3-y][3-x];
            else if (rotation == 3) rotated_piece[y][x] = TETROMINOES[current_piece_type][x][3-y];
        }
    }

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (rotated_piece[y][x]) {
                int board_x = current_piece_x + x + offset_x;
                int board_y = current_piece_y + y + offset_y;

                if (board_x < 0 || board_x >= GAME_WIDTH || board_y >= GAME_HEIGHT) {
                    return true;
                }

                if (board_y >= 0 && board_x >= 0 && board_x < GAME_WIDTH && game_board[board_y][board_x]) {
                    return true;
                }
            }
        }
    }
    return false;
}

void rotate_piece() {
    uint8_t new_rotation = (current_piece_rotation + 1) % 4;
    if (!check_collision(0, 0, new_rotation)) {
        current_piece_rotation = new_rotation;
    }
}

void lock_piece() {
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
                if (board_y >= 0 && board_x >= 0 && board_x < GAME_WIDTH) {
                    game_board[board_y][board_x] = current_piece_type + 1;
                }
            }
        }
    }
}

void clear_lines() {
    int lines_to_clear = 0;
    int cleared_rows[GAME_HEIGHT];
    int cleared_count = 0;

    for (int y = 0; y < GAME_HEIGHT; y++) {
        bool line_complete = true;
        for (int x = 0; x < GAME_WIDTH; x++) {
            if (!game_board[y][x]) {
                line_complete = false;
                break;
            }
        }

        if (line_complete) {
            cleared_rows[cleared_count++] = y;
        }
    }

    lines_to_clear = cleared_count;

    if (lines_to_clear > 0) {
        int dest_row = GAME_HEIGHT - 1;
        for (int src_row = GAME_HEIGHT - 1; src_row >= 0; src_row--) {
            bool is_cleared_row = false;
            for(int i = 0; i < cleared_count; i++) {
                if (src_row == cleared_rows[i]) {
                    is_cleared_row = true;
                    break;
                }
            }

            if (!is_cleared_row) {
                if (dest_row != src_row) {
                    for (int x = 0; x < GAME_WIDTH; x++) {
                        game_board[dest_row][x] = game_board[src_row][x];
                    }
                }
                dest_row--;
            }
        }
        for (int y = 0; y <= dest_row; y++) {
            for (int x = 0; x < GAME_WIDTH; x++) {
                game_board[y][x] = 0;
            }
        }

        lines_cleared += lines_to_clear;

        switch (lines_to_clear) {
            case 1: score += 100 * level; break;
            case 2: score += 300 * level; break;
            case 3: score += 500 * level; break;
            case 4: score += 800 * level; break;
        }

        level = 1 + (lines_cleared / 10);
        uint32_t drop_speed = 40 - (level * 2);
        if (drop_speed < 8) drop_speed = 8;
    }
}

void new_piece() {
    current_piece_type = next_piece_type;
    next_piece_type = rand() % 7;
    current_piece_rotation = 0;
    current_piece_x = GAME_WIDTH / 2 - 2;
    current_piece_y = 0;

    if (check_collision(0, 0, current_piece_rotation)) {
        game_over = true;
    }
}
