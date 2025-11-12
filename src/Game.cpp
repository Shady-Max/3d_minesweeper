#include <iostream>
#include <chrono>
#include "Game.h"
#include "gl_util.h"
#include "GameScene.h"
#include "Scene.h"

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

    scene = new GameScene();
    scene->setCameraSettings(viewportWidth, viewportHeight);
    scene->onEnter(this);

    return 0;
}

void Game::update() {
    double deltaTime = 0.0;
    auto lastFrame = std::chrono::high_resolution_clock::now();

    while(!glfwWindowShouldClose(window)) {

        auto currentFrame = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentFrame - lastFrame;
        deltaTime = elapsed.count();
        lastFrame = currentFrame;

        glfwPollEvents();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene->onUpdate(this, deltaTime);
        scene->onRender();

        glfwSwapBuffers(window);

    }

}

Game::Game(float width, float height) : viewportWidth(width), viewportHeight(height) {}

void Game::close() {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Game::onClose() {
    scene->onExit();
    glfwDestroyWindow(window);
    glfwTerminate();
}
