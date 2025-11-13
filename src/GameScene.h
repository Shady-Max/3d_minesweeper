//
// Created by Max on 13.10.2025.
//

#ifndef OPENGLFINAL_GAMESCENE_H
#define OPENGLFINAL_GAMESCENE_H

#define ROW 8
#define COL 8

#include "Scene.h"

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

struct Cell {
    bool isMine = false;
    bool isRevealed = false;
    bool isFlagged = false;
    int neighborMines = 0;
};

class GameScene : public Scene {
public:
    void onEnter(Game* game) override;
    void onExit() override;
    void onUpdate(Game* game, double deltaTime) override;
    void onRender() override;

    void restartGame();
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    Ray getRayFromMouse(double mouseX, double mouseY, int screenWidth, int screenHeight);
    bool rayAABBIntersection(const Ray& ray, const AABB& aabb, float& t);
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void static_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    void initializeMinesweeper(int totalMines);
    void placeMines(int numMines);
    void calculateNeighborMines();
    int countNeighborMines(int row, int col);
    static void revealEmptyArea(int row, int col);

protected:
    GLuint skyboxVBO, skyboxVAO, skyboxProg, skyboxCubemap;
    GLuint VBO,VAO, prog, unrevealedTexture[6], numberedTexture[9][6], mineTexture[6];
    glm::vec3 lightPos;
    static Cell grid[ROW][COL];
    static bool isGameOver;
    static int MINES;
};


#endif //OPENGLFINAL_GAMESCENE_H
