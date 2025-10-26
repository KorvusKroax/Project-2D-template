#include "canvas.h"
#include "open_gl.h"

#include "draw/line.h"
#include "draw/circle.h"
#include "draw/rectangle.h"
#include "text/font.h"
#include "text/text.h"
#include "text/text_field.h"
#include "button.h"

#include <vector>

const unsigned int WIDTH = 320;
const unsigned int HEIGHT = 200;
const unsigned int PIXEL_SIZE = 4;

Canvas canvas(WIDTH, HEIGHT);
OpenGL openGL({
    .canvas = &canvas,
    .pixelSize = PIXEL_SIZE,
    // .windowMode = FULLSCREEN_RESOLUTION
});

std::vector<std::pair<int, int>> points;

int px = 8;

int main()
{
    glfwSetKeyCallback(openGL.window,
        [](GLFWwindow* window, int key, int scancode, int action, int mod) {
            if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
            if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) px++;
            if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) px--;
        }
    );

    // Canvas image("resources/img/161.png");

    points.clear();





    Font font("resources/font/PetMe64.ttf", 8);

    Button::Options opts = {
        .x = 50,
        .y = 50,
        .width = 100,
        .height = 25,

        .text ="HELLO",
        .font = &font,
    };

    Button button(opts);





    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clear();





        button.update(&openGL);

        switch (button.status) {
            case Button::HOVER:
                opts.bgColor = EGA_LIGHT_RED;
                break;
            case Button::CLICKED:
                opts.bgColor = EGA_LIGHT_BLUE;
                break;
            default: // NOPE
                opts.bgColor = CLEAR;
                break;
        }

        button.draw(&canvas, &opts);





        // canvas.setPixels(50, 20, &image);

        // draw points
        for (std::pair<int, int> p : points) {
            canvas.setPixel(p.first, p.second, Color(255, 255, 255, 31));
        }

        // mouse
        int mx, my;
        openGL.getMousePosition(&mx, &my);
        Circle::draw(&canvas, mx, my, 5, Color(0, 255, 255, 127));
        canvas.setPixel(mx, my, Color(255, 255, 255, 63));

        // drag mouse
        if (glfwGetMouseButton(openGL.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            points.push_back({mx, my});
        }



        openGL.updateScroll();
        openGL.update();
    }
}

// Lorem ipsum, dolor sit amet consectetur adipisicing elit. Similique, asperiores amet. Ducimus beatae magni dolores sed excepturi voluptatem dolor. Reiciendis hic dolorem accusamus minus nesciunt blanditiis deleniti aut ab eum?
