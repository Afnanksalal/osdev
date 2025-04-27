#include "input.h"
#include "game.h"

unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void handle_input() {
    static uint8_t last_scancode = 0;
    uint8_t scancode = inb(0x60);

    if (scancode == last_scancode) return;
    last_scancode = scancode;

    if (!(scancode & 0x80)) {
        switch (scancode) {
            case 0x4B: // Left arrow
                if (!check_collision(-1, 0, current_piece_rotation)) {
                    current_piece_x--;
                }
                break;
            case 0x4D: // Right arrow
                if (!check_collision(1, 0, current_piece_rotation)) {
                    current_piece_x++;
                }
                break;
            case 0x48: // Up arrow - rotate
                rotate_piece();
                break;
            case 0x50: // Down arrow - soft drop
                if (!check_collision(0, 1, current_piece_rotation)) {
                    current_piece_y++;
                }
                break;
            case 0x1C: // Enter - hard drop
                while (!check_collision(0, 1, current_piece_rotation)) {
                    current_piece_y++;
                }
                lock_piece();
                clear_lines();
                new_piece();
                break;
        }
    }
}
