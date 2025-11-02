#pragma once

#include "open_gl/glad/glad.h"
#include "open_gl/GLFW/glfw3.h"
#include "canvas.h"

enum WindowMode {
    FULLSCREEN_SCREEN,
    FULLSCREEN_RESOLUTION,
    WINDOWED,
    WINDOWED_BORDERLESS,
    // WINDOWED_RESIZABLE,
};

class OpenGL
{
public:
    Canvas* canvas;
    unsigned int pixelSize;
    int xPixelOffset, yPixelOffset;

    GLFWwindow* window;
    int windowWidth, windowHeight;

    OpenGL(Canvas* canvas, unsigned int pixelSize = 1, WindowMode windowMode = WINDOWED, const char* title = "OpenGL 2D canvas - scrollable");
    ~OpenGL();

    void update(float bgColor_r = .1f, float bgColor_g = .2f, float bgColor_b = .2f, float bgColor_a = 1.0f);
    void updateScroll();
    void getMousePosition(int* x, int* y);
    void screenToCanvas(double* x, double* y);

private:
    GLFWmonitor* primaryMonitor;
    const GLFWvidmode* videoMode;

    GLuint VAO, VBO, shader;
    GLuint texColorBuffer;

    float quadVertices[24] = {
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
        1.0f, -1.0f,   1.0f, 0.0f,

        -1.0f,  1.0f,   0.0f, 1.0f,
        1.0f, -1.0f,   1.0f, 0.0f,
        1.0f,  1.0f,   1.0f, 1.0f,
    };

    int xMouseOffset = 0;
    int yMouseOffset = 0;
    double xLastMouse, yLastMouse;
    bool scrollButtonPressed = false;

    void createWindow(const char* title = "OpenGL 2D canvas - scrollable");
    void createFullScreen_quadToResolution();
    void createFullScreen_quadToScreen();
    void createFullScreen();
    GLuint createShader();
};
