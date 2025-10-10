#include <iostream>
#include "Game.h"
#include "gl_util.h"

int Game::init() {
    glfwSetErrorCallback(glfw_error);
    if(!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(viewportWidth, viewportHeight,"3D Minesweeper",NULL,NULL);
    if(!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if(!gladLoadGL())
    {
        std::cerr<<"Failed to init GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    return 0;
}

void Game::update() {

}

Game::Game(float width, float height) : viewportWidth(width), viewportHeight(height) {}

void Game::close() {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Game::onClose() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
