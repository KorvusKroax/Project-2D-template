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

const char* FILE_NAME = "resources/img/161.png";

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

    // // click mouse
    // glfwSetMouseButtonCallback(openGL.window,
    //     [](GLFWwindow* window, int button, int action, int mods) {
    //         if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    //             int px, py;
    //             openGL.getMousePosition(&px, &py);
    //             points.push_back({px, py});
    //         }
    //     }
    // );

    points.clear();

    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clearPixelBuffer();



        for (std::pair<int, int> p : points) {
            canvas.setPixel(p.first, p.second, Color(255, 255, 255, 31));
        }

        // mouse
        int mx, my;
        openGL.getMousePosition(&mx, &my);
        Circle::draw(&canvas, mx, my, 5, EGA_LIGHT_CYAN);
        canvas.setPixel(mx, my, EGA_LIGHT_CYAN);

        // drag mouse
        if (glfwGetMouseButton(openGL.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            points.push_back({mx, my});
        }



        openGL.updateScroll();
        openGL.update();
    }
}
