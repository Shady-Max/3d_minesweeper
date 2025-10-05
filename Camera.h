//
// Created by Max on 05.10.2025.
//

#ifndef OPENGLFINAL_CAMERA_H
#define OPENGLFINAL_CAMERA_H


#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    float fov = 60.0f;
    int width, height;
    float near = 0.01f, far = 100.0f;
    glm::vec3 position;
    glm::quat rotation;

    glm::mat4 getViewMatrix();

    glm::mat4 getProjection();
};


#endif //OPENGLFINAL_CAMERA_H
