#include "solarium/celestial/celestial_body.hpp"
#include "solarium/physics/gravity.hpp"
#include "solarium/physics/verlet.hpp"
#include "solarium/rendering/camera.hpp"
#include "solarium/rendering/renderer.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

namespace {

constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;

constexpr double AstronomicalUnit =
    1.495978707e11;

constexpr double Day =
    86'400.0;

double previousMouseX = 0.0;
double previousMouseY = 0.0;

bool firstMouse = true;

solarium::rendering::Camera* camera = nullptr;

void mouseCallback(
    GLFWwindow*,
    double x,
    double y
) {

    if (firstMouse) {

        previousMouseX = x;
        previousMouseY = y;

        firstMouse = false;
    }

    const double xOffset =
        x - previousMouseX;

    const double yOffset =
        previousMouseY - y;

    previousMouseX = x;
    previousMouseY = y;

    if (camera != nullptr) {
        camera->processMouse(
            xOffset,
            yOffset
        );
    }
}

void scrollCallback(
    GLFWwindow*,
    double,
    double yOffset
) {

    if (camera != nullptr) {
        camera->processScroll(yOffset);
    }
}

}

int main() {

    using namespace solarium;

    if (!glfwInit()) {

        std::cerr
            << "Failed to initialize GLFW.\n";

        return 1;
    }

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow* window =
        glfwCreateWindow(
            WindowWidth,
            WindowHeight,
            "Solarium V0.2",
            nullptr,
            nullptr
        );

    if (window == nullptr) {

        std::cerr
            << "Failed to create GLFW window.\n";

        glfwTerminate();

        return 1;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    const int version =
        gladLoadGL(
            glfwGetProcAddress
        );

    if (version == 0) {

        std::cerr
            << "Failed to initialize GLAD.\n";

        glfwDestroyWindow(window);
        glfwTerminate();

        return 1;
    }

    std::cout
        << "OpenGL "
        << GLAD_VERSION_MAJOR(version)
        << "."
        << GLAD_VERSION_MINOR(version)
        << "\n";

    rendering::Camera cameraObject;

    camera = &cameraObject;

    glfwSetCursorPosCallback(
        window,
        mouseCallback
    );

    glfwSetScrollCallback(
        window,
        scrollCallback
    );

    rendering::Renderer renderer(
        WindowWidth,
        WindowHeight
    );

    celestial::CelestialBody sun(
        "Sun",
        1.98847e30,
        6.9634e8,
        math::Vec3{},
        math::Vec3{}
    );

    celestial::CelestialBody earth(
        "Earth",
        5.9722e24,
        6.371e6,
        math::Vec3{
            AstronomicalUnit,
            0.0,
            0.0
        },
        math::Vec3{
            0.0,
            29'780.0,
            0.0
        }
    );

    earth.setAcceleration(
        physics::gravitationalAcceleration(
            sun,
            earth
        )
    );

    // One simulation day per rendered frame.
    constexpr double simulationStep =
        Day;

    double simulationDays = 0.0;

    double lastTime =
        glfwGetTime();

    while (
        !glfwWindowShouldClose(window)
    ) {

        const double currentTime =
            glfwGetTime();

        const double deltaTime =
            currentTime - lastTime;

        lastTime = currentTime;

        // -------------------------------
        // Input
        // -------------------------------

        if (
            glfwGetKey(
                window,
                GLFW_KEY_ESCAPE
            ) == GLFW_PRESS
        ) {

            glfwSetWindowShouldClose(
                window,
                GLFW_TRUE
            );
        }

        // -------------------------------
        // Physics
        // -------------------------------

        const math::Vec3 acceleration =
            physics::gravitationalAcceleration(
                sun,
                earth
            );

        physics::VelocityVerlet::integrate(
            earth,
            acceleration,
            simulationStep
        );

        simulationDays += 1.0;

        // -------------------------------
        // Camera
        // -------------------------------

        cameraObject.update(
            deltaTime
        );

        // -------------------------------
        // Rendering
        // -------------------------------

        renderer.beginFrame();

        renderer.renderBody(
            sun,
            cameraObject
        );

        renderer.renderBody(
            earth,
            cameraObject
        );

        renderer.endFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout
        << "Simulated "
        << simulationDays
        << " days.\n";

    return 0;
}