#include "../game/game.h"

void kernel_main() {
    game_main();
    // Game over, just halt the system
    while (1) {
        __asm__ volatile("cli");
        __asm__ volatile("hlt");
    }
}
