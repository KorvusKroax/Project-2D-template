#include "ui/ui_manager.h"
#include "canvas.h"
#include "open_gl.h"
#include "font.h"
#include "ui/button.h"
#include "ui/toggle.h"

#include <iostream>

Canvas canvas(320, 200);
OpenGL openGL(&canvas, 4);

UIManager ui;
int mouseX, mouseY;
int count = 0;

int main()
{
    glfwSetKeyCallback(openGL.window,
        [](GLFWwindow* window, int key, int scancode, int action, int mod) {
            if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);
        }
    );

    glfwSetCursorPosCallback(openGL.window,
        [](GLFWwindow*, double xpos, double ypos) {
            openGL.screenToCanvas(&xpos, &ypos);
            mouseX = xpos;
            mouseY = ypos;
            ui.updateHover(mouseX, mouseY);
        }
    );

    glfwSetMouseButtonCallback(openGL.window,
        [](GLFWwindow*, int button, int action, int) {
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                ui.handleClick(mouseX, mouseY);
            }
        }
    );





    Font font("resources/font/PetMe64.ttf", 8);

    Text::Options opts = {
        .font = &font,
        // .textColor = WHITE,
        // .shadowColor = CLEAR,
        // .tabSize = 4,
        // .lineHeightScale = 1.0f
    };

    std::shared_ptr<Button> btn = std::make_shared<Button>(10, 10, 80, 20, "Button", opts);
    btn->onClickCallback = [wp = std::weak_ptr<Button>(btn)]() {
        if (std::shared_ptr<Button> b = wp.lock()) {
            std::cout << count++ << ": Button clicked!" << std::endl;
        }
    };
    ui.add(btn);

    std::shared_ptr<Toggle> tgl = std::make_shared<Toggle>(10, 50, 8, 8, "Toggle", opts);
    tgl->onClickCallback = [wp = std::weak_ptr<Toggle>(tgl)]() {
        if (std::shared_ptr<Toggle> t = wp.lock()) {
            std::cout << count++ << ": Toggle clicked!" << std::endl;
        }
    };
    ui.add(tgl);





    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clear();

        ui.draw(&canvas);

        openGL.update();
    }
}
