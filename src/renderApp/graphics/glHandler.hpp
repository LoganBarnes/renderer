#ifndef GL_HANDLER_H
#define GL_HANDLER_H

#include <unordered_map>

typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;

class GLHandler
{

public:
    explicit GLHandler(GLsizei width = 640, GLsizei height = 480);
    virtual ~GLHandler();

    // getters
    GLuint getTexture() { return m_texture; }
    GLsizei getViewportWidth() { return m_viewportWidth; }
    GLsizei getViewportHeight() { return m_viewportHeight; }


    void addProgram(const char *name, const char *vertFilePath, const char *fragFilePath);
    void setBuffer(const char *program, GLfloat *data);

    void render(const char *program);

    void resize(GLsizei width, GLsizei height, bool resetTexture);

    static std::string readFile(const char *filePath);
    static GLuint LoadShader(const char *vertex_path, const char *fragment_path);

private:
    void _setTexture();

    std::unordered_map <const char*, GLuint> m_programs;
    GLuint m_vbo, m_vao;

    GLsizei m_viewportWidth, m_viewportHeight;

    GLuint m_texture;

};

#endif // GL_HANDLER_H