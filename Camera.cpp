#include "Camera.h"

glm::mat4 Camera::getViewMatrix() {
    glm::mat4 rot = glm::mat4_cast(glm::conjugate(rotation));
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), -position);

    glm::mat4 world = rot * trans;

    return world;
}

glm::mat4 Camera::getProjection() {
    return glm::perspective(
            glm::radians(fov), // field of view
            (float)width/(float)height, // aspect ratio
            near, // near plane
            far // far plane
    );
}
