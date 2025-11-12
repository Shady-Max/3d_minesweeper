#ifndef OPENGLFINAL_GAME_H
#define OPENGLFINAL_GAME_H

struct GLFWwindow;
class Scene;

class Game {
public:
    GLFWwindow* window;
    Scene* scene;
    int viewportWidth, viewportHeight;

    Game(float width, float height);
    int init();
    void update();
    void close();
    void onClose();
};


#endif //OPENGLFINAL_GAME_H
