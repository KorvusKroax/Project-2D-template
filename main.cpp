#include "canvas.h"
#include "open_gl.h"
#include "line.h"
#include "rectangle.h"
#include "circle.h"
#include "fill.h"
#include "text.h"

#include <iostream>
#include <vector>

const unsigned int WIDTH = 320;
const unsigned int HEIGHT = 240;
const unsigned int PIXEL_SIZE = 4;

const char* FONT_FILE_NAME = "resources/font/c64_font.png";

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



        Rectangle::draw_filled(&canvas, 10, 10, 16 * 4, 5 * 6, BROWN);
        Text::draw_char(&canvas, 18, 28, 'F', YELLOW);
        Text::draw_char(&canvas, 30, 28, 'G', YELLOW, Color(0, 0, 0, 63));
        Text::draw_text(&canvas, 18, 16, "Hello megint", YELLOW, Color(0, 0, 0, 63), DARK_GREEN);

        Rectangle::draw_filled(&canvas, 5, canvas.height - 5 - 40, 140, 40, C64_VICE_LIGHT_BLUE);
        Font font(FONT_FILE_NAME, 32, 3, 8, 8);
        Text::draw_char(&canvas, 15, canvas.height - 16, 'F', &font, C64_VICE_WHITE);
        Text::draw_char(&canvas, 35, canvas.height - 16, 'G', &font, C64_VICE_WHITE, Color(0, 0, 0, 63));
        Text::draw_text(&canvas, 15, canvas.height - 32, "Hello megint", &font, C64_VICE_WHITE, Color(0, 0, 0, 63), C64_VICE_BLUE);



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
