#include <iostream>
#include <array>
#include <string>
#include <sstream>
#include <numbers>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "camera/fpc.hpp"
#include "shader/shader.hpp"
#include "render/cloth.hpp"

#include "cloth/cloth.hpp"

void processCameraInput(GLFWwindow* window, FirstPersonCamera* camera);

int main(int argc, char* argv[])
{
    GLFWwindow* window;

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
        window = glfwCreateWindow(800, 600, "Cloth Simulation with Mass Spring", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return -1;
        }

        // window configurations
        {
            // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        // Make the window's context current
        glfwMakeContextCurrent(window);

        // Load Opengl
        if (!gladLoadGL()) {
            glfwTerminate();
            return -1;
        };

        glEnable(GL_DEPTH_TEST);
    }

    // Main Loop
    {
        /**** Initiate Objects Here ****/
        Shader shader("../../res/shader/cloth.vs", "../../res/shader/cloth.fs");
        FirstPersonCamera camera;

        auto transform = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), {1.0f, 0.0f, 0.0f});

        const float timestep = 0.003f;
        RectCloth cloth {40, 30, 0.1f, 1.0f, timestep, transform};
        RectClothRenderer renderer(&shader, &camera, &cloth);

        float currentTime = static_cast<float>(glfwGetTime());
        float lastTime = currentTime;
        float deltaTime = 0.0f;

        int totalIterCount = 0;
        float totalIterTime = 0.0f;
        float overTakenTime = 0.0f;

        // Loop until the user closes the window
        while (!glfwWindowShouldClose(window))
        {
            // Terminate condition
            if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

            // Updating
            {
                // Calculate dt
                currentTime = static_cast<float>(glfwGetTime());
                deltaTime = currentTime - lastTime - overTakenTime; // maintain the deltaTime not get too large
                overTakenTime = 0.0f;
                lastTime = currentTime;

                processCameraInput(window, &camera);

                // Debug Update here only when p is pressed
                if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
                {
                    // A fixed timestep which should not be too large in order to stabilize the simulation
                    totalIterTime += deltaTime;
                    float curIterTime = totalIterTime - totalIterCount * timestep;
                    int iter_count = curIterTime / timestep;

                    for (int i = 0; i < iter_count; ++i) {
                        totalIterCount += 1;

                        // Iter Here
                        cloth.step();

                        float timeTaken = static_cast<float>(glfwGetTime()) - currentTime;
                        if (timeTaken > deltaTime) {
                            overTakenTime = timeTaken - deltaTime;
                            break;
                        }
                    }
                }
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

void processCameraInput(GLFWwindow* window, FirstPersonCamera* camera)
{
    static bool firstRun {true};
    static float lastFrame {0};

    static float lastCursorX {0};
    static float lastCursorY {0};

    double curCursorX, curCursorY;
    glfwGetCursorPos(window, &curCursorX, &curCursorY);

    float currentFrame = static_cast<float>(glfwGetTime());

    if (firstRun) {
        lastFrame = currentFrame;
        firstRun = false;

        lastCursorX = static_cast<float>(curCursorX);
        lastCursorY = static_cast<float>(curCursorY);

        return; // everything zero, so we return directly
    }

    float deltaTime = currentFrame - lastFrame;
    float deltaCursorX = curCursorX - lastCursorX;
    float deltaCursorY = curCursorY - lastCursorY;

    float cameraMoveSpeed = 3.5f * deltaTime;
    float cameraRotateSpeed = 1.0f * deltaTime;

	if((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_S) != GLFW_PRESS))
        camera->moveForward(cameraMoveSpeed);
    if((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS))
        camera->moveBackward(cameraMoveSpeed);
    if((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_A) != GLFW_PRESS))
        camera->moveRight(cameraMoveSpeed);
    if((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_D) != GLFW_PRESS))
        camera->moveLeft(cameraMoveSpeed);
    if((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS))
        camera->moveUp(cameraMoveSpeed);
    if((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS))
        camera->moveDown(cameraMoveSpeed);

    if((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_RIGHT) != GLFW_PRESS))
        camera->lookRight(cameraRotateSpeed);
    if((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT) != GLFW_PRESS))
        camera->lookLeft(cameraRotateSpeed);
    if((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_PRESS))
        camera->lookUp(cameraRotateSpeed);
    if((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_UP) != GLFW_PRESS))
        camera->lookDown(cameraRotateSpeed);
    if((glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_E) != GLFW_PRESS))
        camera->rotateLeft(cameraRotateSpeed);
    if((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS))
        camera->rotateRight(cameraRotateSpeed);

    // camera->lookLeft(cameraRotateSpeed * deltaCursorX * 0.2f);
    // camera->lookDown(cameraRotateSpeed * deltaCursorY * 0.2f);

    // update record
    lastCursorX = static_cast<float>(curCursorX);
    lastCursorY = static_cast<float>(curCursorY);

    lastFrame = currentFrame;
}
