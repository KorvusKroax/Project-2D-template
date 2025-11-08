#include "open_gl.h"

#include <algorithm> // clamp

OpenGL::OpenGL(Canvas* canvas, unsigned int pixelSize, WindowMode windowMode, const char* title)
    : canvas(canvas), pixelSize(pixelSize)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DECORATED, (windowMode != WINDOWED_BORDERLESS ? GLFW_TRUE : GLFW_FALSE));
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);//(windowMode == WINDOWED_RESIZABLE ? GLFW_TRUE : GLFW_FALSE));

    this->primaryMonitor =  glfwGetPrimaryMonitor();
    this->videoMode = glfwGetVideoMode(this->primaryMonitor);

    switch (windowMode) {
        case FULLSCREEN_SCREEN:
            createFullScreen_quadToScreen();
            break;
        case FULLSCREEN_RESOLUTION:
            createFullScreen_quadToResolution();
            break;
        default:
            createWindow();
            break;
    }

    // centering window's content
    this->xPixelOffset = (this->canvas->width * this->pixelSize - this->windowWidth) >> 1;
    this->yPixelOffset = (this->canvas->height * this->pixelSize - this->windowHeight) >> 1;

    glfwMakeContextCurrent(this->window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glGenTextures(1, &this->texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, this->texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->canvas->width, this->canvas->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->canvas->pixelBuffer.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    this->shader = createShader();
    glUseProgram(this->shader);
    glUniform1i(glGetUniformLocation(this->shader, "screenTexture"), 0);
    glUniform1f(glGetUniformLocation(this->shader, "canvasWidth"), this->canvas->width * this->pixelSize);
    glUniform1f(glGetUniformLocation(this->shader, "canvasHeight"), this->canvas->height * this->pixelSize);
    glUniform1f(glGetUniformLocation(this->shader, "windowWidth"), this->windowWidth);
    glUniform1f(glGetUniformLocation(this->shader, "windowHeight"), this->windowHeight);
}

OpenGL::~OpenGL()
{
    glDeleteTextures(1, &this->texColorBuffer);
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
    glDeleteProgram(this->shader);
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void OpenGL::update(float bgColor_r, float bgColor_g, float bgColor_b, float bgColor_a)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    glUseProgram(this->shader);

    glUniform1f(glGetUniformLocation(this->shader, "xPixelOffset"), this->xPixelOffset);
    glUniform1f(glGetUniformLocation(this->shader, "yPixelOffset"), this->yPixelOffset);

    glBindTexture(GL_TEXTURE_2D, this->texColorBuffer);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->canvas->width, this->canvas->height, GL_RGBA, GL_UNSIGNED_BYTE, this->canvas->pixelBuffer.data());

    glClearColor(bgColor_r, bgColor_g, bgColor_b, bgColor_a);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(this->window);
    glfwPollEvents();
}

void OpenGL::updateScroll()
{
    double xCurrMouse, yCurrMouse;
    glfwGetCursorPos(this->window, &xCurrMouse, &yCurrMouse);

    if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        if (scrollButtonPressed) {
            this->xPixelOffset = std::clamp(
                int(this->xPixelOffset - (xCurrMouse - this->xLastMouse)),
                int(0), int(this->canvas->width * this->pixelSize - this->windowWidth)
            );
            this->yPixelOffset = std::clamp(
                int(this->yPixelOffset + (yCurrMouse - this->yLastMouse)),
                int(0), int(this->canvas->height * this->pixelSize - this->windowHeight)
            );
        } else {
            scrollButtonPressed = true;
        }
        this->xLastMouse = xCurrMouse;
        this->yLastMouse = yCurrMouse;
    } else if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE) {
        scrollButtonPressed = false;
    }
}

void OpenGL::getMousePosition(int* x, int* y)
{
    double mouseX, mouseY;
    glfwGetCursorPos(this->window, &mouseX, &mouseY);
    screenToCanvas(&mouseX, &mouseY);
    *x = mouseX;
    *y = mouseY;
}

void OpenGL::screenToCanvas(double* x, double* y)
{
    *x = (                         *x + this->xPixelOffset + this->xMouseOffset) / (float)this->pixelSize;
    *y = (this->windowHeight - 1 - *y + this->yPixelOffset - this->yMouseOffset) / (float)this->pixelSize;
}

void OpenGL::createWindow(const char* title)
{
    int xWorkareaPosition, yWorkareaPosition, workareaWidth, workareaHeight;
    glfwGetMonitorWorkarea(this->primaryMonitor, &xWorkareaPosition, &yWorkareaPosition, &workareaWidth, &workareaHeight);

    this->windowWidth = std::min(this->canvas->width * (int)this->pixelSize, workareaWidth);
    this->windowHeight = std::min(this->canvas->height * (int)this->pixelSize, workareaHeight);

    this->window = glfwCreateWindow(this->windowWidth, this->windowHeight, title, nullptr, nullptr);



    // // remove frame size from window size (not working)
    // int left, top, right, bottom;
    // glfwGetWindowFrameSize(this->window, &left, &top, &right, &bottom);

    // int maxContentWidth = workareaWidth - (left + right);
    // int maxContentHeight = workareaHeight - (top + bottom);

    // this->windowWidth = (this->windowWidth <= maxContentWidth) ? this->windowWidth : maxContentWidth;
    // this->windowHeight = (this->windowHeight <= maxContentHeight) ? this->windowHeight : maxContentHeight;

    // glfwSetWindowSize(this->window, this->windowWidth, this->windowHeight);



    // centering window
    int xPosition = xWorkareaPosition + ((workareaWidth - this->windowWidth) >> 1);
    int yPosition = yWorkareaPosition + ((workareaHeight - this->windowHeight) >> 1);
    glfwSetWindowPos(this->window, xPosition, yPosition);
}

void OpenGL::createFullScreen_quadToResolution()
{
    createFullScreen();

    this->xMouseOffset = (this->windowWidth - this->videoMode->width) >> 1;
    this->yMouseOffset = (this->windowHeight - this->videoMode->height) >> 1;
}

void OpenGL::createFullScreen_quadToScreen()
{
    this->canvas->resize(
        (float)this->videoMode->width / this->pixelSize,
        (float)this->videoMode->height / this->pixelSize
    );

    createFullScreen();
}

void OpenGL::createFullScreen()
{
    this->windowWidth = std::min(int(this->canvas->width * this->pixelSize), this->videoMode->width);
    this->windowHeight = std::min(int(this->canvas->height * this->pixelSize), this->videoMode->height);

    for (int i = 0; i < 24; i += 4) {
        this->quadVertices[i + 0] *= std::min((float)this->canvas->width / this->videoMode->width * this->pixelSize, 1.0f);
        this->quadVertices[i + 1] *= std::min((float)this->canvas->height / this->videoMode->height * this->pixelSize, 1.0f);
    }

    this->window = glfwCreateWindow(this->videoMode->width, this->videoMode->height, "", this->primaryMonitor, nullptr);
}

GLuint OpenGL::createShader() {
    const char* vertexShader = R"(
        #version 330 core
        layout (location = 0) in vec2 pos;
        layout (location = 1) in vec2 texCoord;
        out vec2 TexCoord;
        uniform float canvasWidth;
        uniform float canvasHeight;
        uniform float windowWidth;
        uniform float windowHeight;
        uniform float xPixelOffset;
        uniform float yPixelOffset;
        void main() {
            gl_Position = vec4(pos, 0.0, 1.0);
            TexCoord = texCoord;
            TexCoord.x = texCoord.x * (windowWidth / canvasWidth) + (xPixelOffset / canvasWidth);
            TexCoord.y = texCoord.y * (windowHeight / canvasHeight) + (yPixelOffset / canvasHeight);
        }
    )";
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShader, nullptr);
    glCompileShader(vs);

    const char* fragmentShader = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;
        uniform sampler2D screenTexture;
        void main() {
            FragColor = texture(screenTexture, TexCoord);
        }
    )";
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShader, nullptr);
    glCompileShader(fs);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
