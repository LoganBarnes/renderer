#include <GL/glew.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "renderApp.hpp"
#include "pathTracer.hpp"
#include "renderer-config.hpp"
#include "renderObjects.hpp"

const int DEFAULT_WIDTH = 640;
const int DEFAULT_HEIGHT = 480;


#ifdef USE_GRAPHICS
#include "graphicsHandler.hpp"
#include "camera.hpp"
#endif


RenderApp::RenderApp()
    : m_graphics(NULL),
      m_pathTracer(NULL),
      m_camera(NULL),
      m_loopFPS(60.0),
      m_iterationWithoutClear(2)
{
#ifdef USE_GRAPHICS
    m_graphics = new GraphicsHandler(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    m_camera = new Camera();
#endif
    m_pathTracer = new PathTracer();
}

RenderApp::~RenderApp()
{
#ifdef USE_GRAPHICS
    m_pathTracer->unregisterTexture("tex");

    if (m_camera)
        delete m_camera;
    if (m_graphics)
        delete m_graphics;
#endif
    if (m_pathTracer)
        delete m_pathTracer;
}

//int RenderApp::execute(int , const char **)
int RenderApp::execute(int argc, const char **argv)
{

#ifdef USE_GRAPHICS
    if (!m_graphics->init("Render App"))
        return 1;


    std::string vertPath = std::string(RESOURCES_PATH) + "/shaders/default.vert";
    std::string fragPath = std::string(RESOURCES_PATH) + "/shaders/default.frag";
    m_graphics->addProgram("default", vertPath.c_str(), fragPath.c_str());

    GLfloat screenData[8] = {-1, 1, -1, -1, 1, 1, 1, -1};
    m_graphics->addUVBuffer("fullscreen", "default", screenData, 8);

    m_graphics->addTextureArray("tex", DEFAULT_WIDTH, DEFAULT_HEIGHT, NULL);

    m_camera->setAspectRatio(
                static_cast<float>(DEFAULT_WIDTH) /
                static_cast<float>(DEFAULT_HEIGHT));

//    m_graphics->setBlending(true);

#endif
    m_pathTracer->init(argc, argv, DEFAULT_WIDTH, DEFAULT_HEIGHT);

#ifdef USE_GRAPHICS
    m_pathTracer->register2DTexture("tex", m_graphics->getTexture("tex"));
    m_pathTracer->setScaleViewInvEye(m_camera->getEye(), m_camera->getScaleViewInvMatrix());
#endif

    _buildScene();

    return this->_runLoop();
}


int RenderApp::_runLoop()
{
    uint counterMax = 100000000;
    uint counter = counterMax + 1;
#ifdef USE_GRAPHICS
    m_iterationWithoutClear = 2;

    while (!m_graphics->checkWindowShouldClose())
    {
        if (counter > counterMax)
        {
            if (m_iterationWithoutClear < 4)
            {
                m_pathTracer->tracePath("tex", DEFAULT_WIDTH, DEFAULT_HEIGHT, m_iterationWithoutClear);
                m_graphics->render("default", "fullscreen", 4, GL_TRIANGLE_STRIP, "tex");
                m_graphics->updateWindow();
            }
            else
            {
                m_pathTracer->tracePath("tex", DEFAULT_WIDTH, DEFAULT_HEIGHT, m_iterationWithoutClear);
                m_graphics->render("default", "fullscreen", 4, GL_TRIANGLE_STRIP, "tex", false);
                m_graphics->updateWindow();
            }
            ++m_iterationWithoutClear;
            counter = 0;
        }
        ++counter;
    }
#endif

    return 0;
}


void RenderApp::_buildScene()
{
    // 0: right (green) wall
    glm::mat4 trans = glm::mat4();
    trans *= glm::translate(glm::mat4(), glm::vec3(-2.5f, 0.f, -1.5f));
    trans *= glm::rotate(glm::mat4(), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
    trans *= glm::scale(glm::mat4(), glm::vec3(2.f, 2.5f, 1.f));
    m_pathTracer->addShape(QUAD, trans, glm::vec3(0.8f, 0.f, 0.f));

    // 1: left (red) wall
    trans = glm::mat4();
    trans *= glm::translate(glm::mat4(), glm::vec3(2.5f, 0.f, -1.5f));
    trans *= glm::rotate(glm::mat4(), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
    trans *= glm::scale(glm::mat4(), glm::vec3(2.f, 2.5f, 1.f));
    m_pathTracer->addShape(QUAD, trans, glm::vec3(0.f, 0.8f, 0.f));

    // 2: ceiling
    trans = glm::mat4();
    trans *= glm::translate(glm::mat4(), glm::vec3(0.f, 2.5f, -1.5f));
    trans *= glm::rotate(glm::mat4(), glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    trans *= glm::scale(glm::mat4(), glm::vec3(2.501f, 2.001f, 1.f));
    m_pathTracer->addShape(QUAD, trans, glm::vec3(0.8f, 0.8f, 0.8f));

    // 3: floor
    trans = glm::mat4();
    trans *= glm::translate(glm::mat4(), glm::vec3(0.f, -2.5f, -1.5f));
    trans *= glm::rotate(glm::mat4(), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    trans *= glm::scale(glm::mat4(), glm::vec3(2.501f, 2.001f, 1.f));
    m_pathTracer->addShape(QUAD, trans, glm::vec3(0.8f, 0.8f, 0.8f));

    // 4: back wall
    trans = glm::mat4();
    trans *= glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, -3.5f));
    trans *= glm::rotate(glm::mat4(), glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
    trans *= glm::scale(glm::mat4(), glm::vec3(2.501f, 2.501f, 1.f));
    m_pathTracer->addShape(QUAD, trans, glm::vec3(0.8f, 0.8f, 0.8f));

    // 5: close sphere
    trans = glm::mat4();
    trans *= glm::translate(glm::mat4(), glm::vec3(1.f, -1.5f, -0.5f));
    m_pathTracer->addShape(SPHERE, trans, glm::vec3(1.f));

    // 6: far sphere
    trans = glm::mat4();
    trans *= glm::translate(glm::mat4(), glm::vec3(-1.f, -1.5f, -2.5f));
    m_pathTracer->addShape(SPHERE, trans, glm::vec3(1.f));

    // 7: light
    trans = glm::mat4();
    trans *= glm::translate(glm::mat4(), glm::vec3(0.f, 2.475f, -1.5f));
    trans *= glm::rotate(glm::mat4(), glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    trans *= glm::scale(glm::mat4(), glm::vec3(0.5f, 0.3, 1.f));
    m_pathTracer->addAreaLight(QUAD, trans, glm::vec3(60.f), 0.5f * 0.3f);
}



