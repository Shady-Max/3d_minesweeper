//
// Created by Max on 13.10.2025.
//

#ifndef OPENGLFINAL_GAMESCENE_H
#define OPENGLFINAL_GAMESCENE_H

#include "Scene.h"

class GameScene : public Scene {
public:
    void onEnter(Game* game) override;
    void onExit() override;
    void onUpdate(Game* game, double deltaTime) override;
    void onRender() override;
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
protected:
    GLuint skyboxVBO, skyboxVAO, skyboxProg, skyboxCubemap;
    GLuint VBO,VAO, prog;
    glm::vec3 lightPos;
};


#endif //OPENGLFINAL_GAMESCENE_H
