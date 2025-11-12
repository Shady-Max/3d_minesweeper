//
// Created by Max on 13.10.2025.
//

#include <shaders.h>
#include "GameScene.h"
#include "gl_util.h"
#include "Game.h"

static bool staticCamera = true;

void GameScene::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0 && !staticCamera) {
        camera.fov = std::max(camera.fov - 1.0f, 30.0f);
    } else if (yoffset < 0 && !staticCamera) {
        camera.fov = std::min(camera.fov + 1.0f, 90.0f);
    }
}

void GameScene::onEnter(Game* game) {
    Scene::onEnter(game);

    lightPos = glm::vec3(30.0f,25.0f, 60.0f);

    float vertices[] = {
            // positions          // normals
            // Front face
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

            // Back face
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            // Left face
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

            // Right face
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            // Bottom face
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

            // Top face
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    prog = create_program(color_vertex_glsl, color_fragment_glsl);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    skyboxProg = create_program(skybox_vertex_glsl, skybox_fragment_glsl);
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> faces {
            "skybox/px.png", // right
            "skybox/nx.png", // left
            "skybox/py.png", // top
            "skybox/ny.png", // bottom
            "skybox/pz.png", // front
            "skybox/nz.png"  // back
    };
    skyboxCubemap = loadCubemap(faces);

    glfwSetScrollCallback(game->window, scroll_callback);
}

void GameScene::onExit() {
    glDeleteBuffers(1,&VBO);
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&skyboxVBO);
    glDeleteVertexArrays(1,&skyboxVAO);
    glDeleteProgram(prog);
    glDeleteProgram(skyboxProg);
    Scene::onExit();
}

void GameScene::onUpdate(Game* game, double deltaTime) {
    Scene::onUpdate(game, deltaTime);

    if (glfwGetKey(game->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        game->close();
    }
    if (glfwGetKey(game->window, GLFW_KEY_W) == GLFW_PRESS && !staticCamera) {
        camera.position.z -= 2 * deltaTime;
    }
    if (glfwGetKey(game->window, GLFW_KEY_S) == GLFW_PRESS && !staticCamera) {
        camera.position.z += 2 * deltaTime;
    }
    if (glfwGetKey(game->window, GLFW_KEY_A) == GLFW_PRESS && !staticCamera) {
        camera.position.x -= 2 * deltaTime;
    }
    if (glfwGetKey(game->window, GLFW_KEY_D) == GLFW_PRESS && !staticCamera) {
        camera.position.x += 2 * deltaTime;
    }
    if (!staticCamera) {
        if (camera.position.x < -5) camera.position.x = -5;
        if (camera.position.x > 5) camera.position.x = 5;
        if (camera.position.z < -5) camera.position.z = -5;
        if (camera.position.z > 5) camera.position.z = 5;
    }
    if (glfwGetKey(game->window, GLFW_KEY_KP_1) == GLFW_PRESS) {
        staticCamera = true;
        camera.position = glm::vec3(3.0f, -2.0f, 3.0f);
        camera.rotation = glm::quat(glm::vec3(glm::radians(30.0f), glm::radians(45.0f), 0.0f));
        camera.fov = 60;
    }
    if (glfwGetKey(game->window, GLFW_KEY_KP_2) == GLFW_PRESS) {
        staticCamera = true;
        camera.position = glm::vec3(3.0f, 2.0f, 3.0f);
        camera.rotation = glm::quat(glm::vec3(glm::radians(-30.0f), glm::radians(45.0f), 0.0f));
        camera.fov = 60;
    }
    if (glfwGetKey(game->window, GLFW_KEY_KP_3) == GLFW_PRESS) {
        staticCamera = true;
        camera.position = glm::vec3(-3.0f, 2.0f, -3.0f);
        camera.rotation = glm::quat(glm::vec3(glm::radians(-30.0f), glm::radians(-135.0f), 0.0f));
        camera.fov = 60;
    }
    if (glfwGetKey(game->window, GLFW_KEY_KP_4) == GLFW_PRESS) {
        staticCamera = true;
        camera.position = glm::vec3(-3.0f, -2.0f, -3.0f);
        camera.rotation = glm::quat(glm::vec3(glm::radians(30.0f), glm::radians(-135.0f), 0.0f));
        camera.fov = 60;
    }
    if (glfwGetKey(game->window, GLFW_KEY_KP_5) == GLFW_PRESS) {
        staticCamera = false;
        camera.position = glm::vec3(0.0f, 10.0f, 0.0f);
        camera.rotation = glm::quat(glm::vec3(glm::radians(-90.0f), glm::radians(0.0f), 0.0f));
        camera.fov = 60;
    }
}

void GameScene::onRender() {
    Scene::onRender();

    glUseProgram(prog);

    unsigned int mvpLoc = glGetUniformLocation(prog, "uMVP");
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::rotate(model, 15.0f, glm::vec3(1.0f, 0.0f, 1.0f));

    glm::mat4 view = camera.getViewMatrix();

    glm::mat4 projection = camera.getProjection();

    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(projection * view * model));

    unsigned int colorLoc = glGetUniformLocation(prog, "uColor");
    glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(0.8f, 0.0f, 0.0f)));

    glUniform3fv(glGetUniformLocation(prog, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(prog, "viewPos"), 1, glm::value_ptr(camera.position));

    glUniformMatrix4fv(glGetUniformLocation(prog, "uModel"), 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDepthFunc(GL_LEQUAL);

    glUseProgram(skyboxProg);

    glm::mat4 viewNoTranslate = glm::mat4(glm::mat3(view)); // remove translation
    glUniformMatrix4fv(glGetUniformLocation(skyboxProg, "view"), 1, GL_FALSE, glm::value_ptr(viewNoTranslate));
    glUniformMatrix4fv(glGetUniformLocation(skyboxProg, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(skyboxVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemap);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
