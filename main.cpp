#include <vector>

#include "canvas.h"
#include "open_gl.h"
#include "line.h"
#include "circle.h"
#include "rectangle.h"
#include "text/font.h"
#include "text/text.h"
#include "text/text_field.h"

const unsigned int WIDTH = 320;
const unsigned int HEIGHT = 200;
const unsigned int PIXEL_SIZE = 4;

Canvas canvas(WIDTH, HEIGHT);
OpenGL openGL(&canvas, PIXEL_SIZE);//, FULLSCREEN_RESOLUTION);

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

    points.clear();

    Canvas image("resources/img/161.png");


    // FULL
    // graph-35-pix.regular.ttf: 8px (mono)
    // 7-12-serif.regular.ttf: 12px (mono)
    // blockblueprint.medium.ttf: 15px
    // bitpotion.ext.ttf: 14px
    // pixel-millennium.regular.ttf: 8px (very small)

    // PARTIAL
    // pc-senior.regular.ttf: 8px (mono)
    // basis33.ttf: 16px (mono)
    // Perfect DOS VGA 437 Win.ttf: 16px (mono)
    // space-invaders-full-version.otf: 8px (mono)
    // advanced-pixel-7.regular.ttf: 20px
    // high-pixel-7.regular.ttf: 10px
    // thin-pixe-l7.regular.ttf: 20px
    // dedicool.regular.ttf: 8px
    // double-pixel-7.regular.ttf: 20px
    // Mojang-Regular.ttf: 8px
    // Mojang-Bold.ttf: 8px
    // post-pixel-7.regular.ttf: 20px
    // PixelOperator.ttf: 16px
    // PixelOperator8.ttf: 8px (mono)

    // C64_Pro_Mono-STYLE: 8px
    // C64_Pro-STYLE: 8px
    // Berkelium64.ttf: 10px
    // Berkelium1541.ttf: 6px


    Font font("resources/font/PetMe64.ttf", 8);
    TextField textField(
        216, 120,
        "Lorém ipsüm, dólor sit ámet consectetúr adipisicing elit.\nSimilique, asperiores amet.", &font,
        C64_VICE_CYAN, 1.2f
    );

    printf("%i, %i", (int)((unsigned char)" "[0]), font.utf8ToCodepoint(" "));

    while (!glfwWindowShouldClose(openGL.window)) {
        canvas.clear();

        // canvas.setPixels(50, 20, &image);

        Line::draw(&canvas, canvas.width >> 1, 0, canvas.width >> 1, canvas.height - 1, Color(64,64,64), 0x33333333);
        Line::draw(&canvas, 0, canvas.height >> 1, canvas.width - 1, canvas.height >> 1, Color(64,64,64), 0x33333333);
        Line::draw(&canvas, canvas.width >> 2, 0, canvas.width >> 2, canvas.height - 1, Color(32,32,32), 0x30303030);
        Line::draw(&canvas, 0, canvas.height >> 2, canvas.width - 1, canvas.height >> 2, Color(32,32,32), 0x30303030);
        Line::draw(&canvas, (canvas.width >> 1) + (canvas.width >> 2), 0, (canvas.width >> 1) + (canvas.width >> 2), canvas.height - 1, Color(32,32,32), 0x30303030);
        Line::draw(&canvas, 0, (canvas.height >> 1) + (canvas.height >> 2), canvas.width - 1, (canvas.height >> 1) + (canvas.height >> 2), Color(32,32,32), 0x30303030);





        // Rectangle::draw_filled(&canvas, 5, (canvas.height >> 2), 200, 100, C64_VICE_GREEN);
        // Text::draw_line(&canvas, 20, (canvas.height >> 1), "Helló, világ!", &font, C64_VICE_WHITE, CLEAR);
        // Text::draw_multiline(&canvas, 20, (canvas.height >> 1), "Helló, világ!\nhelló megint...", &font, C64_VICE_WHITE, CLEAR, 1.5f);

        textField.draw(&canvas, canvas.width >> 2, canvas.height >> 2);





        // draw points
        for (std::pair<int, int> p : points) {
            canvas.setPixel(p.first, p.second, Color(255, 255, 255, 31));
        }

        // mouse
        int mx, my;
        openGL.getMousePosition(&mx, &my);
        Circle::draw(&canvas, mx, my, 5, Color(0, 255, 255, 63));
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
