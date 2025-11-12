//
// Created by Max on 13.10.2025.
//

#ifndef OPENGLFINAL_SCENE_H
#define OPENGLFINAL_SCENE_H


#include "Camera.h"
#include "Game.h"

class Game;

class Scene {
public:
    virtual void setCameraSettings(int width, int height);
    virtual void onEnter(Game* game);
    virtual void onExit();
    virtual void onRender();
    virtual void onUpdate(Game* game, double deltaTime);

protected:
    static Camera camera;
};


#endif //OPENGLFINAL_SCENE_H
