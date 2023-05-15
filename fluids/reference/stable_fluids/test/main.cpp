#include <iostream>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader/shader.hpp"
#include "render/fluid.hpp"

#include "fluid/scene.hpp"

void processMouseInput(GLFWwindow* window, FluidScene* camera);

int main(int argc, char* argv[])
{
    GLFWwindow* window;
    unsigned width = 1000;
    unsigned height = 400 ;

    // Window setups
    {
        if (!glfwInit()) // Initialize glfw library
            return -1;

        // setting glfw window hints and global configurations
        {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use Core Mode
            // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Use Debug Context
        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
        #endif
        }

        // Create a windowed mode window and its OpenGL context
        window = glfwCreateWindow(width, height, "Stable Fluids Simulation", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return -1;
        }

        // window configurations
        {
            // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            // glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
        }

        // Make the window's context current
        glfwMakeContextCurrent(window);

        // Load Opengl
        if (!gladLoadGL()) {
            glfwTerminate();
            return -1;
        };

        // On Window Resized
        glfwSetFramebufferSizeCallback(window,
            [](GLFWwindow*, int _w, int _h) {
                glViewport(0, 0, _w, _h);
            }
        );
    }

    // Main Loop
    {
        /**** Initiate Objects Here ****/
        Shader shader("../../res/shader/fluid.vs", "../../res/shader/fluid.fs");

        FluidScene fluid {125, 50};
        FluidVisualizer renderer {&shader, &fluid};

        { // Initialize here
            fluid.setBackground();
        }

        // Loop until the user closes the window
        while (!glfwWindowShouldClose(window))
        {
            // Terminate condition
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

            processMouseInput(window, &fluid);

            // Updating
            {
                // Uncomment to debug update step by step
                // if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
                {
                    fluid.step();
                }
            }

            glClear(GL_COLOR_BUFFER_BIT);

            // Draw here
            {
                renderer.draw();
            }

            // Swap front and back buffers
            glfwSwapBuffers(window);

            // Poll for and process events
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}


void processMouseInput(GLFWwindow* window, FluidScene* fluid)
{
    static bool firstRun {true};
    static float lastCursorX {0};
    static float lastCursorY {0};
    static int lastLeftButtonState {GLFW_RELEASE};
    static int lastRightButtonState {GLFW_RELEASE};

    double curCursorX, curCursorY;
    int curLeftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int curRightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

    glfwGetCursorPos(window, &curCursorX, &curCursorY);
    if (firstRun) {
        firstRun = false;
        lastCursorX = static_cast<float>(curCursorX);
        lastCursorY = static_cast<float>(curCursorY);
        return; // everything zero, so we return directly
    }

    if (curLeftButtonState == GLFW_PRESS || curRightButtonState == GLFW_PRESS) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float deltaCursorX = (curCursorX - lastCursorX) / float(width);
        float deltaCursorY = (curCursorY - lastCursorY) / float(height);

        // map from screen to fluid space
        glm::vec2 applyPos = {
            curCursorX / width * fluid->width,
            (1.0 - curCursorY / height) * fluid->height
        };

        if (curLeftButtonState == GLFW_PRESS) {
            // Click to apply delta velocity
            glm::vec2 applyVel = {deltaCursorX * 100.0f, -deltaCursorY * 100.0f};
            fluid->applyImpulsiveVelocity(applyPos, applyVel);

            std::cout << "Apply ("<< applyPos.x << ", " << applyPos.y << "): (" << applyVel.x << ", " << applyVel.y << ")" << std::endl;

        } else if (lastRightButtonState == GLFW_RELEASE) {
            // Click to read data for debugging..?
            glm::vec2 vel = fluid->getVelocity(applyPos.x, applyPos.y); // interpolateVelocityAt(applyPos);
            float dens = fluid->getDensity(applyPos.x, applyPos.y); // interpolateDensityAt(applyPos);

            std::cout << "Read ("<< applyPos.x << ", " << applyPos.y << "): (" << vel.x << ", " << vel.y << ") " << dens << std::endl;
        }
    }

    // update record
    lastCursorX = static_cast<float>(curCursorX);
    lastCursorY = static_cast<float>(curCursorY);
    lastLeftButtonState = curLeftButtonState;
    lastRightButtonState = curRightButtonState;
}