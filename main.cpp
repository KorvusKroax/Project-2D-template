#include "canvas.h"
#include "open_gl.h"
#include "line.h"
#include "circle.h"
#include "fill.h"

#include <iostream>
#include <vector>

const unsigned int WIDTH = 320;
const unsigned int HEIGHT = 240;
const unsigned int PIXEL_SIZE = 4;

const char* FILE_NAME = "161.png";

Canvas canvas(WIDTH, HEIGHT);
OpenGL openGL(&canvas, PIXEL_SIZE);//, FULLSCREEN_RESOLUTION);
std::vector<std::pair<int, int>> points;

int main()
{
    glfwSetKeyCallback(openGL.window,
        [](GLFWwindow* window, int key, int scancode, int action, int mod) {
            if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
        }
    );

    // // click
    // glfwSetMouseButtonCallback(canvas.window,
    //     [](GLFWwindow* window, int button, int action, int mods) {
    //         if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    //             int px, py;
    //             canvas.getMousePos(&px, &py);
    //             points.push_back({px, py});
    //         }
    //     }
    // );

    Canvas image(FILE_NAME);

    points.clear();

    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clearPixelBuffer();

        Circle::draw(&canvas, canvas.width >> 1, canvas.height >> 1, 100, Color(255, 255, 0, 63));
        Circle::draw_filled(&canvas, canvas.width >> 1, canvas.height >> 1, 100, Color(255, 255, 0, 63));
        Circle::draw_filled(&canvas, canvas.width >> 1, canvas.height >> 1, 75, Color(255, 255, 0, 63));
        Circle::draw_filled(&canvas, canvas.width >> 1, canvas.height >> 1, 50, Color(255, 255, 0, 63));
        Circle::draw_filled(&canvas, canvas.width >> 1, canvas.height >> 1, 25, Color(255, 255, 0, 63));
        Line::draw(&canvas, 0, 0, canvas.width, canvas.height, Color(255, 255, 255, 127));

        canvas.setPixels(10, 10, &image);

        for (std::pair<int, int> p : points) {
            canvas.setPixel(p.first, p.second, Color(255, 255, 255, 31));
        }

        int mx, my;
        openGL.getMousePos(&mx, &my);

        // drag
        if (glfwGetMouseButton(openGL.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            points.push_back({mx, my});
        }

        Canvas sprite;
        canvas.getPixels(100, 100, 20, 20, &sprite);
        canvas.setPixels(mx, my, &sprite);

        Circle::draw(&canvas, mx, my, 5, Color(0, 255, 255));
        canvas.setPixel(mx, my, EGA_LIGHT_CYAN);

        openGL.updateScroll();
        openGL.update();
    }
}
