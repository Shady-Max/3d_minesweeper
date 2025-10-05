#include <iostream>
#include "gl_util.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"

void glfw_error(int code, const char* desc) {
    std::cerr<<"GLFW error"<<code<<": "<<desc<<"\n";
}

GLuint compile_shader(GLenum type,const char* src){
    GLuint s=glCreateShader(type);
    glShaderSource(s,1,&src,nullptr);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s,GL_COMPILE_STATUS,&ok);
    if(!ok)
    {
        char buf[512];
        glGetShaderInfoLog(s,512,nullptr,buf);
        std::cerr<<"Shader compile error:\n"<<buf<<"\n";
    }
    return s;
}

GLuint create_program(const char* vsrc,const char* fsrc){
    GLuint v=compile_shader(GL_VERTEX_SHADER,vsrc);
    GLuint f=compile_shader(GL_FRAGMENT_SHADER,fsrc);
    GLuint p=glCreateProgram();
    glAttachShader(p,v);
    glAttachShader(p,f);
    glLinkProgram(p); GLint ok;
    glGetProgramiv(p,GL_LINK_STATUS,&ok);
    if(!ok){
        char buf[512];
        glGetProgramInfoLog(p,512,nullptr,buf);
        std::cerr<<"Program link error:\n"<<buf<<"\n";
    }
    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}

GLuint loadCubemap(std::vector<std::string> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            if (nrChannels == 1) format = GL_RED;
            else if (nrChannels == 3) format = GL_RGB;
            else if (nrChannels == 4) format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap failed at: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}