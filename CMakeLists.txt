cmake_minimum_required(VERSION 3.31)
project(openglFinal C CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

add_subdirectory(external/glfw)

add_subdirectory(external/glm)

add_executable(openglFinal
        main.cpp
        external/stb/stb_image.h
        Camera.cpp
        pch.cpp
        gl_util.cpp
        gl_util.h
        Game.cpp
        Game.h
)

target_precompile_headers(openglFinal PRIVATE pch.h)

target_link_libraries(openglFinal PRIVATE
        glfw
        glad
        glm::glm
)

file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/shaders/*.glsl")
set(GENERATED_HEADER ${CMAKE_BINARY_DIR}/generated/shaders.h)

string (REPLACE ";" "|" SHADER_FILES_ESCAPED "${SHADER_FILES}")

add_custom_command(
        OUTPUT ${GENERATED_HEADER}
        COMMAND ${CMAKE_COMMAND}
        -DSHADER_FILES="${SHADER_FILES_ESCAPED}"
        -DGENERATED_HEADER="${GENERATED_HEADER}"
        -P ${CMAKE_SOURCE_DIR}/cmake/GenerateShaders.cmake
        DEPENDS ${SHADER_FILES}
        COMMENT "Generating shaders.h from GLSL files"
)

add_custom_target(generate_shaders DEPENDS ${GENERATED_HEADER})

add_dependencies(openglFinal generate_shaders)
target_include_directories(openglFinal PRIVATE ${CMAKE_BINARY_DIR}/generated)

file(COPY ${CMAKE_SOURCE_DIR}/skybox DESTINATION ${CMAKE_BINARY_DIR})