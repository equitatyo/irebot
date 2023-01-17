#include "includes.h"

#include "gui/gui.h"

#include "networking.h"

GUI* gui;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 300

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	GUI_OnClick(gui, button, action);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    GUI_OnKey(gui, key, action == GLFW_PRESS);
}

int main() {
	GLFWwindow* window;

	if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "IreBot", NULL, NULL);
    if (!window)
    {
    	printf("Failed to create window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gui = GUI_Init(WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

#ifdef __DEBUG
    double previous_time = glfwGetTime();
    int frame_count = 0;
    int fps = 0;
#endif

    while (!glfwWindowShouldClose(window))
    {
    	glClear(GL_COLOR_BUFFER_BIT);

    	double mouse_x, mouse_y;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		GUI_SetMousePos(gui, mouse_x, mouse_y);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GUI_Render(gui);

#ifdef __DEBUG
        double current_time = glfwGetTime();
        frame_count++;

        if (current_time - previous_time >= 1.0) {
            fps = frame_count;
            frame_count = 0;
            previous_time = current_time;
        }

        char buf[16];
        sprintf(buf, "%i", fps);

        FontRenderer_DrawString(gui->font_renderer, buf, 0, 0, 16, color_from_bytes(255, 255, 255, 255));
#endif

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
}