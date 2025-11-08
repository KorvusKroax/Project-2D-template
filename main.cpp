#include "canvas.h"
#include "open_gl.h"

#include "color/color.h"
// #include "color/palettes/ega.h"
// #include "color/palettes/c64_vice.h"
#include "color/palettes/c64_colodore.h"

#include "ui/ui_manager.h"
#include "ui/font.h"
#include "ui/button.h"
#include "ui/toggle.h"

#include <iostream>

Canvas canvas(320, 200);
OpenGL openGL(&canvas, 4);//, FULLSCREEN_RESOLUTION);

UIManager ui;
double mouseX, mouseY;
double lastMouseX, lastMouseY;
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
            mouseX = xpos;
            mouseY = ypos;
            openGL.screenToCanvas(&mouseX, &mouseY);
            ui.updateHover(mouseX, mouseY);
            // ui.handleDrag(mouseX, mouseY);
            lastMouseX = mouseX;
            lastMouseY = mouseY;
        }
    );

    glfwSetMouseButtonCallback(openGL.window,
        [](GLFWwindow*, int button, int action, int) {
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                ui.handleClick(mouseX, mouseY);
            }
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
                ui.handleRelease(mouseX, mouseY);
            }
        }
    );





    Font font("resources/font/PetMe64.ttf", 8);
    // Font font("c:/windows/fonts/arialbd.ttf", 15);

    Text::Options opts = {
        .font = &font,
        // .shadow_color = Color(0, 0, 0, 100),
        // .shadow_direction = std::make_pair(1, -2),
        .outline_color = C64_BLACK,
        .outline_size = 1
    };



    Canvas buttonImage("resources/img/button.png");
    std::shared_ptr<Button> btn = std::make_shared<Button>(10, 10, &buttonImage, "Button", opts);
    // std::shared_ptr<Button> btn = std::make_shared<Button>(10, 10, 80, 20, "Button", opts);
    btn->onClick_callback = [wp = std::weak_ptr<Button>(btn)]() {
        if (std::shared_ptr<Button> b = wp.lock()) {
            std::cout << count++ << ": Button clicked!" << std::endl;
        }
    };
    btn->onDrag_callback = [wp = std::weak_ptr<Button>(btn)]() {
        if (std::shared_ptr<Button> b = wp.lock()) {
            std::cout << count++ << ": Button drag!" << std::endl;
            float offX = mouseX - lastMouseX;
            float offY = mouseY - lastMouseY;
            b->x += offX;
            b->y += offY;
        }
    };
    btn->onRelease_callback = [wp = std::weak_ptr<Button>(btn)]() {
        if (std::shared_ptr<Button> b = wp.lock()) {
            std::cout << count++ << ": Button released!" << std::endl;
        }
    };
    ui.add(btn);



    Canvas toggleImage("resources/img/checkbox.png");
    Canvas checkImage("resources/img/check.png");
    std::shared_ptr<Toggle> tgl = std::make_shared<Toggle>(10, 50, &toggleImage, &checkImage, "Toggle", opts);
    // std::shared_ptr<Toggle> tgl = std::make_shared<Toggle>(10, 50, 16, 16, "Toggle", opts);
    tgl->onClick_callback = [wp = std::weak_ptr<Toggle>(tgl)]() {
        if (std::shared_ptr<Toggle> t = wp.lock()) {
            std::cout << count++ << ": Toggle clicked!" << std::endl;
        }
    };
    ui.add(tgl);





    while (!glfwWindowShouldClose(openGL.window)) {
        // canvas.clear();
        canvas.fastFill(C64_BLUE);

        ui.draw(&canvas);

        Text::draw(&canvas, 8, canvas.height - 16, "   **** COMMODORE 64 BASIC V2 ****", {.font = &font, .color = C64_LIGHT_BLUE});
        Text::draw(&canvas, 8, canvas.height - 32, "64K RAM SYSTEM  38911 BASIC BYTES FREE", {.font = &font, .color = C64_LIGHT_BLUE});
        Text::draw(&canvas, 0, canvas.height - 48, "READY.", {.font = &font, .color = C64_LIGHT_BLUE});

        openGL.update(C64_LIGHT_BLUE.r / 255.0f, C64_LIGHT_BLUE.g / 255.0f, C64_LIGHT_BLUE.b / 255.0f, 255);
    }
}
