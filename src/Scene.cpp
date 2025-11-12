//
// Created by Max on 13.10.2025.
//

#include "Scene.h"

Camera Scene::camera;

void Scene::onEnter(Game* game) {}
void Scene::onExit() {}
void Scene::onRender() {}
void Scene::onUpdate(Game* game, double deltaTime) {}
void Scene::setCameraSettings(int width, int height) {
    camera.position = glm::vec3(3.0f, -2.0f, 3.0f);
    camera.rotation = glm::quat(glm::vec3(glm::radians(30.0f), glm::radians(45.0f), 0.0f));
    camera.width = width;
    camera.height = height;
}
