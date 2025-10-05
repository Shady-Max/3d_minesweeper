#ifndef OPENGLFINAL_GL_UTIL_H
#define OPENGLFINAL_GL_UTIL_H

void glfw_error(int code, const char* desc);

GLuint compile_shader(GLenum type,const char* src);

GLuint create_program(const char* vsrc,const char* fsrc);

GLuint loadCubemap(std::vector<std::string> faces);

#endif //OPENGLFINAL_GL_UTIL_H
