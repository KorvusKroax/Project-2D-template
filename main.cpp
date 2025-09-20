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
    Font mono_8x8_c64("resources/font/mono_8x8_c64.png", 8);
    Font mono_6x8("resources/font/mono_6x8.png", 6);
    Font mono_6x8_space_invaders("resources/font/mono_6x8_space_invaders.png", 6);
    Font mono_5x7("resources/font/mono_5x7.png", 5);
    Font prop_8("resources/font/prop_8.png");
    Font prop_8_minecraft("resources/font/prop_8_minecraft.png");
    Font prop_7("resources/font/prop_7.png");

    TextField textField(
        130, 100, LEFT_MIDDLE,
        "Lorem ipsum, dolor sit amet consectetur adipisicing elit. Similique, asperiores amet.",
        TextColor(WHITE),//, Color(0, 0, 0, 63), EGA_GREEN),
        &prop_7//, 4, 4
    );

    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clearPixelBuffer();

        Line::draw(&canvas, canvas.width >> 1, 0, canvas.width >> 1, canvas.height - 1, DARK_GRAY, 0x33333333);
        Line::draw(&canvas, 0, canvas.height >> 1, canvas.width - 1, canvas.height >> 1, DARK_GRAY, 0x33333333);





        // textField.text = ("Time: " + std::to_string(glfwGetTime())).c_str();
        textField.draw(&canvas,
            (canvas.width >> 1) - (textField.width >> 1),
            (canvas.height >> 1) - (textField.height >> 1)
        );





        // Text::draw_line(&canvas, 30, 50,
        //     "Lorem ipsum dolor sit amet\nconsectetur adipisicing elit.\nLaborum fugit eveniet iste\ndistinctio vel.",
        //     TextColor(EGA_WHITE, Color(0, 0, 0, 64), EGA_GREEN),
        //     &prop_7
        // );
        // Text::draw_char(&canvas, 30, 100,
        //     'y',
        //     TextColor(EGA_WHITE, Color(0, 0, 0, 64), EGA_BRIGHT_RED),
        //     &prop_7
        // );

        // Text::draw_line(&canvas, 30, 150,
        //     "Lorem ipsum dolor sit amet\nconsectetur adipisicing elit.\nLaborum fugit eveniet iste\ndistinctio vel.",
        //     TextColor(EGA_WHITE, Color(0, 0, 0, 64), EGA_LIGHT_BLUE),
        //     &mono_5x7
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

// Lorem ipsum, dolor sit amet consectetur adipisicing elit. Similique, asperiores amet. Ducimus beatae magni dolores sed excepturi voluptatem dolor. Reiciendis hic dolorem accusamus minus nesciunt blanditiis deleniti aut ab eum?
