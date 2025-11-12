#include "pch.h"
#include <iostream>
#include "shaders.h"
#include "Camera.h"
#include "gl_util.h"
#include "Game.h"
#include <chrono>
#include <thread>

int main() {
    Game game(800, 600);
    game.init();
    game.update();
    game.onClose();
    return 0;
}
