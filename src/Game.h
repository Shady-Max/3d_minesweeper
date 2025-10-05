//
// Created by Max on 05.10.2025.
//

#ifndef OPENGLFINAL_GAME_H
#define OPENGLFINAL_GAME_H


class Game {
public:
    GLFWwindow* window;
    float viewportWidth, viewportHeight;

    Game(float width, float height);
    int init();
    void update();
    void close();
    void onClose();
};


#endif //OPENGLFINAL_GAME_H
