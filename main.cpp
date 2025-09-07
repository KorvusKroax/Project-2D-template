#include "canvas.h"
#include "open_gl.h"
#include "line.h"
#include "rectangle.h"
#include "circle.h"
#include "fill.h"
#include "text.h"
#include "text_field.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

const unsigned int WIDTH = 320;
const unsigned int HEIGHT = 240;
const unsigned int PIXEL_SIZE = 4;

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
    Font font8x8("resources/font/font8x8_c64.png", 32, 3, 8, 8);
    Font font6x8("resources/font/font6x8.png", 32, 3, 6, 8);
    Font font5x7("resources/font/font5x7.png", 32, 3, 5, 7);

    TextField textField(
        200, 150,
        "Lorem ipsum, dolor sit amet consectetur adipisicing elit.", LEFT_MIDDLE,
        TextColor(WHITE),
        &font8x8
    );

    // Hotspot hotspot(10, 10, 100, 50);

    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clearPixelBuffer();

        Line::draw(&canvas, canvas.width >> 1, 0, canvas.width >> 1, canvas.height - 1, C64_VICE_DARK_GREY);
        Line::draw(&canvas, 0, canvas.height >> 1, canvas.width - 1, canvas.height >> 1, C64_VICE_DARK_GREY);





        // textField.update(("Time:\n" + std::to_string(glfwGetTime())).c_str());

        textField.draw(&canvas,
            (canvas.width >> 1) - (textField.width >> 1),
            (canvas.height >> 1) - (textField.height >> 1)
        );






        // Rectangle::draw_filled(&canvas, (canvas.width >> 1) - 10, (canvas.height >> 1) - 10, 100, 50, EGA_LIGHT_BLUE);

        // Text::draw_line(&canvas, canvas.width >> 1, canvas.height >> 1,
        //     "Lorem ipsum dolor sit amet\nconsectetur adipisicing elit.\nLaborum fugit eveniet iste\ndistinctio vel.\nvalamimás",
        //     TextColor(WHITE, Color(0, 0, 0, 64))
        // );


        // draw points
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
