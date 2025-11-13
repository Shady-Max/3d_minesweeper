#include <shaders.h>
#include <ctime>
#include <queue>
#include <iostream>
#include "GameScene.h"
#include "gl_util.h"
#include "Game.h"

Cell GameScene::grid[ROW][COL];
bool GameScene::isGameOver = false;
int GameScene::MINES = 10;

void GameScene::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0) {
        camera.fov = std::max(camera.fov - 1.0f, 30.0f);
    } else if (yoffset < 0) {
        camera.fov = std::min(camera.fov + 1.0f, 90.0f);
    }
}

Ray GameScene::getRayFromMouse(double mouseX, double mouseY, int screenWidth, int screenHeight) {
    // Convert mouse coordinates to NDC (Normalized Device Coordinates)
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;

    // Create ray in clip space
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

    // Convert to view space
    glm::mat4 projection = camera.getProjection();
    glm::vec4 rayView = glm::inverse(projection) * rayClip;
    rayView = glm::vec4(rayView.x, rayView.y, -1.0f, 0.0f);

    // Convert to world space
    glm::mat4 view = camera.getViewMatrix();
    glm::vec4 rayWorld = glm::inverse(view) * rayView;
    glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

    Ray ray;
    ray.origin = camera.position;
    ray.direction = rayDirection;

    return ray;
}

bool GameScene::rayAABBIntersection(const Ray& ray, const AABB& aabb, float& t) {
    float tMin = 0.0f;
    float tMax = FLT_MAX;

    for (int i = 0; i < 3; i++) {
        if (abs(ray.direction[i]) < 0.0001f) {
            // Ray is parallel to slab
            if (ray.origin[i] < aabb.min[i] || ray.origin[i] > aabb.max[i])
                return false;
        } else {
            // Compute intersection t value of ray with near and far plane of slab
            float t1 = (aabb.min[i] - ray.origin[i]) / ray.direction[i];
            float t2 = (aabb.max[i] - ray.origin[i]) / ray.direction[i];

            if (t1 > t2) std::swap(t1, t2);

            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);

            if (tMin > tMax) return false;
        }
    }

    t = tMin;
    return true;
}

void GameScene::static_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    GameScene* scene = static_cast<GameScene*>(glfwGetWindowUserPointer(window));
    if (scene) {
        scene->mouse_button_callback(window, button, action, mods);
    }
}

void GameScene::revealEmptyArea(int startRow, int startCol) {
    if (grid[startRow][startCol].isMine || grid[startRow][startCol].isFlagged)
        return;

    std::queue<std::pair<int, int>> q;
    q.push({startRow, startCol});
    grid[startRow][startCol].isRevealed = true;

    int directions[8][2] = {
            {-1, -1}, {-1, 0}, {-1, 1},
            { 0, -1},          { 0, 1},
            { 1, -1}, { 1, 0}, { 1, 1}
    };

    while (!q.empty()) {
        auto [row, col] = q.front();
        q.pop();

        // If this cell has neighboring mines, stop expanding here
        if (grid[row][col].neighborMines > 0)
            continue;

        // Explore all 8 neighbors
        for (auto& d : directions) {
            int nr = row + d[0];
            int nc = col + d[1];

            if (nr < 0 || nr >= ROW || nc < 0 || nc >= COL)
                continue;
            if (grid[nr][nc].isRevealed || grid[nr][nc].isFlagged)
                continue;

            grid[nr][nc].isRevealed = true;

            // Only enqueue if neighbor also has 0 mines
            if (grid[nr][nc].neighborMines == 0)
                q.push({nr, nc});
        }
    }
}

void GameScene::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS) {
        // Get mouse position
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Get window size
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Create ray from mouse position
        Ray ray = getRayFromMouse(mouseX, mouseY, width, height);

        // Check intersection with each cube
        float closestT = FLT_MAX;
        int closestRow = -1;
        int closestCol = -1;

        for (int row = 0; row < ROW; ++row) {
            for (int col = 0; col < COL; ++col) {
                // Calculate cube position (same as in onRender)
                glm::vec3 cubePos = glm::vec3((col-(COL-1)*0.5f)*1.3f, 0.0f, (row-(ROW-1)*0.5f)*1.3f);

                // Create AABB for this cube (cube vertices go from -0.5 to 0.5)
                AABB aabb;
                aabb.min = cubePos + glm::vec3(-0.5f, -0.5f, -0.5f);
                aabb.max = cubePos + glm::vec3(0.5f, 0.5f, 0.5f);

                float t;
                if (rayAABBIntersection(ray, aabb, t)) {
                    if (t < closestT) {
                        closestT = t;
                        closestRow = row;
                        closestCol = col;
                    }
                }
            }
        }

        if (closestRow != -1 && !grid[closestRow][closestCol].isRevealed && !isGameOver) {
            if (button == GLFW_MOUSE_BUTTON_LEFT && !grid[closestRow][closestCol].isFlagged) {
                // Left click - reveal cell
                grid[closestRow][closestCol].isRevealed = true;

                if (grid[closestRow][closestCol].isMine) {
                    printf("BOOM! Hit a mine at [%d, %d]\n", closestRow, closestCol);
                    isGameOver = true;
                    for (int r = 0; r < ROW; ++r) {
                        for (int c = 0; c < COL; ++c) {
                            if (grid[r][c].isMine)
                                grid[r][c].isRevealed = true;
                        }
                    }
                    printf("\n\nGame over!\n");
                    // Game over logic here
                } else {
                    printf("Revealed [%d, %d] - Mines nearby: %d\n",
                           closestRow, closestCol,
                           grid[closestRow][closestCol].neighborMines);

                    if (grid[closestRow][closestCol].neighborMines == 0)
                        revealEmptyArea(closestRow, closestCol);
                }
            } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                // Right click - toggle flag
                grid[closestRow][closestCol].isFlagged = !grid[closestRow][closestCol].isFlagged;
                printf("Flagged [%d, %d]: %s\n", closestRow, closestCol,
                       grid[closestRow][closestCol].isFlagged ? "true" : "false");
            }
            checkWinCondition();
        }
    }
}

void GameScene::initializeMinesweeper(int totalMines) {
    // Initialize all cells
    for (int row = 0; row < ROW; ++row) {
        for (int col = 0; col < COL; ++col) {
            grid[row][col].isMine = false;
            grid[row][col].isRevealed = false;
            grid[row][col].isFlagged = false;
            grid[row][col].neighborMines = 0;
        }
    }

    // Place mines randomly
    placeMines(totalMines);

    // Calculate neighbor mine counts
    calculateNeighborMines();
}

void GameScene::placeMines(int numMines) {
    std::srand(std::time(nullptr)); // Seed random number generator

    int minesPlaced = 0;
    while (minesPlaced < numMines) {
        int row = std::rand() % ROW;
        int col = std::rand() % COL;

        // Only place mine if cell doesn't already have one
        if (!grid[row][col].isMine) {
            grid[row][col].isMine = true;
            minesPlaced++;
        }
    }
}

void GameScene::calculateNeighborMines() {
    for (int row = 0; row < ROW; ++row) {
        for (int col = 0; col < COL; ++col) {
            if (!grid[row][col].isMine) {
                grid[row][col].neighborMines = countNeighborMines(row, col);
            }
        }
    }
}

int GameScene::countNeighborMines(int row, int col) {
    int count = 0;

    // Check all 8 neighbors
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            // Skip the center cell
            if (dr == 0 && dc == 0) continue;

            int newRow = row + dr;
            int newCol = col + dc;

            // Check bounds
            if (newRow >= 0 && newRow < ROW && newCol >= 0 && newCol < COL) {
                if (grid[newRow][newCol].isMine) {
                    count++;
                }
            }
        }
    }

    return count;
}


void GameScene::onEnter(Game* game) {
    Scene::onEnter(game);

    initializeMinesweeper(MINES);

    lightPos = glm::vec3(30.0f,25.0f, 60.0f);

    float vertices[] = {
            // positions           // normals            // texcoords
            // Front face
            -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,

            // Back face
            -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,

            // Left face
            -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,

            // Right face
            0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
            0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

            // Bottom face
            -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

            // Top face
            -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f
    };

    prog = create_program(cube_texture_vertex_glsl, cube_texture_fragment_glsl);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLuint dirtTexture = loadTexture("textures/dirt.png");
    GLuint grassTopTexture = loadTexture("textures/grass_block_top.jpg");
    GLuint grassSideTexture = loadTexture("textures/grass_block_side.png");
    GLuint mineTopTexture = loadTexture("textures/TNT_top.png");
    GLuint mineSideTexture = loadTexture("textures/TNT_side.png");
    GLuint mineBottomTexture = loadTexture("textures/TNT_bottom.png");
    GLuint flagTexture = loadTexture("textures/Block_of_iron.png");

    flaggedTexture[0] = flagTexture;
    flaggedTexture[1] = flagTexture;
    flaggedTexture[2] = flagTexture;
    flaggedTexture[3] = flagTexture;
    flaggedTexture[4] = flagTexture;
    flaggedTexture[5] = flagTexture;

    mineTexture[0] = mineSideTexture;
    mineTexture[1] = mineSideTexture;
    mineTexture[2] = mineSideTexture;
    mineTexture[3] = mineSideTexture;
    mineTexture[4] = mineBottomTexture;
    mineTexture[5] = mineTopTexture;

    GLuint numberTextures[9] = {
            dirtTexture,
            loadTexture("textures/1.jpg"),
            loadTexture("textures/2.jpg"),
            loadTexture("textures/3.jpg"),
            loadTexture("textures/4.jpg"),
            loadTexture("textures/5.jpg"),
            loadTexture("textures/6.jpg"),
            loadTexture("textures/7.jpg"),
            loadTexture("textures/8.jpg")
    };
    for (int i=0; i<9; ++i) {
        numberedTexture[i][0] = dirtTexture;
        numberedTexture[i][1] = dirtTexture;
        numberedTexture[i][2] = dirtTexture;
        numberedTexture[i][3] = dirtTexture;
        numberedTexture[i][4] = dirtTexture;
        numberedTexture[i][5] = numberTextures[i];
    }
    unrevealedTexture[0] = grassSideTexture; //front
    unrevealedTexture[1] = grassSideTexture; //back
    unrevealedTexture[2] = grassSideTexture; //left
    unrevealedTexture[3] = grassSideTexture; //right
    unrevealedTexture[4] = dirtTexture; //bottom
    unrevealedTexture[5] = grassTopTexture; //top

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

    std::vector<std::string> faces =  {
            "skybox/px.png", // right
            "skybox/nx.png", // left
            "skybox/py.png", // top
            "skybox/ny.png", // bottom
            "skybox/pz.png", // front
            "skybox/nz.png"  // back
    };
    skyboxCubemap = loadCubemap(faces);

    glfwSetScrollCallback(game->window, scroll_callback);
    glfwSetWindowUserPointer(game->window, this);
    glfwSetMouseButtonCallback(game->window, static_mouse_button_callback);
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
    static bool rKeyWasPressed = false;
    bool rKeyIsPressed = glfwGetKey(game->window, GLFW_KEY_R) == GLFW_PRESS;
    if (rKeyIsPressed && !rKeyWasPressed) {
        restartGame();
    }
    rKeyWasPressed = rKeyIsPressed;

    static bool n1KeyWasPressed = false;
    bool n1KeyIsPressed = glfwGetKey(game->window, GLFW_KEY_KP_1) == GLFW_PRESS;
    if (n1KeyIsPressed && !n1KeyWasPressed) {
        MINES = ROW*COL/6.4f;
        printf("Easy mode: %d mines\n", MINES);
        restartGame();
    }
    n1KeyWasPressed = n1KeyIsPressed;

    static bool n2KeyWasPressed = false;
    bool n2KeyIsPressed = glfwGetKey(game->window, GLFW_KEY_KP_2) == GLFW_PRESS;
    if (n2KeyIsPressed && !n2KeyWasPressed) {
        MINES = ROW*COL/4.5f;
        printf("Medium mode: %d mines\n", MINES);
        restartGame();
    }
    n2KeyWasPressed = n2KeyIsPressed;

    static bool n3KeyWasPressed = false;
    bool n3KeyIsPressed = glfwGetKey(game->window, GLFW_KEY_KP_3) == GLFW_PRESS;
    if (n3KeyIsPressed && !n3KeyWasPressed) {
        MINES = ROW*COL/3.0f;
        printf("Hard mode: %d mines\n", MINES);
        restartGame();
    }
    n3KeyWasPressed = n3KeyIsPressed;

    if (glfwGetKey(game->window, GLFW_KEY_KP_7) == GLFW_PRESS) {
        camera.rotation = glm::quat(glm::vec3(glm::radians(-90.0f), glm::radians(0.0f), 0.0f));
        camera.position.y = 10;
    }

    if (glfwGetKey(game->window, GLFW_KEY_KP_8) == GLFW_PRESS) {
        camera.rotation = glm::quat(glm::vec3(glm::radians(-45.0f), glm::radians(0.0f), 0.0f));
        camera.position.y = 5;
    }

    if (glfwGetKey(game->window, GLFW_KEY_KP_9) == GLFW_PRESS) {
        camera.rotation = glm::quat(glm::vec3(glm::radians(0.0f), glm::radians(0.0f), 0.0f));
        camera.position.y = 2;
    }

    if (glfwGetKey(game->window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.position.z -= 2 * deltaTime;
    }
    if (glfwGetKey(game->window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.position.z += 2 * deltaTime;
    }
    if (glfwGetKey(game->window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.position.x -= 2 * deltaTime;
    }
    if (glfwGetKey(game->window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.position.x += 2 * deltaTime;
    }
    if (camera.position.x < -9) camera.position.x = -9;
    if (camera.position.x > 9) camera.position.x = 9;
    if (camera.position.z < -9) camera.position.z = -9;
    if (camera.position.z > 9) camera.position.z = 9;
}

void GameScene::onRender() {
    Scene::onRender();

    glUseProgram(prog);

    //model = glm::rotate(model, 15.0f, glm::vec3(1.0f, 0.0f, 1.0f));

    glm::mat4 view = camera.getViewMatrix();

    glm::mat4 projection = camera.getProjection();

    unsigned int vLoc = glGetUniformLocation(prog, "uView");
    glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(view));
    unsigned int pLoc = glGetUniformLocation(prog, "uProjection");
    glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(prog, "uCameraPos"), 1, glm::value_ptr(camera.position));
    glUniform3fv(glGetUniformLocation(prog, "uLightPos"), 1, glm::value_ptr(lightPos));

    for (int row = 0; row < ROW; ++row) {
        for (int col = 0; col < COL; ++col) {
            glUniform1f(glGetUniformLocation(prog, "lp.ambient"), 0.2f);
            glUniform1f(glGetUniformLocation(prog, "lp.diffuse"), 0.8f);
            glUniform1f(glGetUniformLocation(prog, "lp.specular"), 0.3f);
            glUniform1f(glGetUniformLocation(prog, "lp.shininess"), 32.0f);
            glUniform1f(glGetUniformLocation(prog, "lp.reflection"), 0.1f);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3((col-(COL-1)*0.5f)*1.3f,  0.0f, (row-(ROW-1)*0.5f)*1.3f));
            unsigned int mLoc = glGetUniformLocation(prog, "uModel");
            glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(VAO);
            for (int i = 0; i < 6; ++i) {
                glActiveTexture(GL_TEXTURE0);
                if (grid[row][col].isFlagged) {
                    glUniform1f(glGetUniformLocation(prog, "lp.reflection"), 0.8f);
                    glBindTexture(GL_TEXTURE_2D, flaggedTexture[i]);
                } else if (!grid[row][col].isRevealed) {
                    glBindTexture(GL_TEXTURE_2D, unrevealedTexture[i]);
                } else if (!grid[row][col].isMine) {
                    glBindTexture(GL_TEXTURE_2D, numberedTexture[grid[row][col].neighborMines][i]);
                } else if (grid[row][col].isMine && grid[row][col].isRevealed) {
                    glBindTexture(GL_TEXTURE_2D, mineTexture[i]);
                } else {
                    glBindTexture(GL_TEXTURE_2D, unrevealedTexture[5]);
                }
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemap);
                glUniform1i(glGetUniformLocation(prog, "uTexture"), 0);
                glUniform1i(glGetUniformLocation(prog, "skybox"), 1);
                glDrawArrays(GL_TRIANGLES, i * 6, 6);
            }
        }
    }


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

void GameScene::restartGame() {
    initializeMinesweeper(MINES);
    isGameOver = false;
    printf("Game restarted\n");
}

void GameScene::checkWinCondition() {
    for (int row = 0; row < ROW; ++row) {
        for (int col = 0; col < COL; ++col) {
            if (!grid[row][col].isMine && !grid[row][col].isRevealed) {
                return; // Found a non-mine cell that is not revealed
            }
            if (grid[row][col].isMine && !grid[row][col].isFlagged) {
                return; // Found a mine that is not flagged
            }
        }
    }
    printf("\n\nCongratulations!\n You've cleared the minefield!\n");
    isGameOver = true;
}
