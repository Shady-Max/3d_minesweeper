#include "pch.h"
#include <iostream>
#include "shaders.h"
#include "Camera.h"
#include "gl_util.h"
#include "Game.h"
#include <chrono>
#include <thread>
#include <string>

int main() {
    Game game(800, 600);
    game.init();
    std::cout << "Controls:\n"
              << "WASD - Move camera\n"
              << "Mouse Scroll - Zoom in/out\n"
              << "R - Restart game\n"
              << "Numpad 1 - Easy mode\n"
              << "Numpad 2 - Medium mode\n"
              << "Numpad 3 - Hard mode\n"
              << "Numpad 7 - Top-down view\n"
              << "Numpad 8 - Angled view\n"
              << "Numpad 9 - Side view\n";
    game.update();
    game.onClose();
    return 0;
}
