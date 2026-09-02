#include "solarium/rendering/camera.hpp"
#include "solarium/rendering/orbit_trail.hpp"
#include "solarium/rendering/renderer.hpp"
#include "solarium/rendering/trail_renderer.hpp"
#include "solarium/simulation/simulation.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {

constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;

solarium::rendering::Camera* gCamera = nullptr;

// ------------------------------------------------------------
// Mouse state
// ------------------------------------------------------------

double previousMouseX = 0.0;
double previousMouseY = 0.0;

bool firstMouse = true;

// ------------------------------------------------------------
// Keyboard edge detection
// ------------------------------------------------------------

bool spaceWasPressed = false;
bool rWasPressed = false;
bool plusWasPressed = false;
bool minusWasPressed = false;

// ------------------------------------------------------------
// GLFW callbacks
// ------------------------------------------------------------

void mouseCallback(
    GLFWwindow*,
    double x,
    double y
) {
    if (firstMouse) {
        previousMouseX = x;
        previousMouseY = y;
        firstMouse = false;
        return;
    }

    const double xOffset =
        x - previousMouseX;

    const double yOffset =
        previousMouseY - y;

    previousMouseX = x;
    previousMouseY = y;

    if (gCamera != nullptr) {
        gCamera->processMouse(
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
    if (gCamera != nullptr) {
        gCamera->processScroll(yOffset);
    }
}

// ------------------------------------------------------------
// Keyboard helper
// ------------------------------------------------------------

bool keyPressed(
    GLFWwindow* window,
    int key
) {
    return glfwGetKey(
        window,
        key
    ) == GLFW_PRESS;
}

// ------------------------------------------------------------
// Format simulation time
// ------------------------------------------------------------

std::string formatSimulationTime(
    double seconds
) {
    const double days =
        seconds / 86'400.0;

    std::ostringstream stream;

    stream
        << std::fixed
        << std::setprecision(2)
        << days
        << " days";

    return stream.str();
}

// ------------------------------------------------------------
// Format simulation speed
// ------------------------------------------------------------

std::string formatTimeScale(
    double scale
) {
    const double daysPerSecond =
        scale / 86'400.0;

    std::ostringstream stream;

    stream
        << std::fixed
        << std::setprecision(2)
        << daysPerSecond
        << " days/sec";

    return stream.str();
}

} // namespace

int main() {

    using namespace solarium;

    // =========================================================
    // GLFW INITIALIZATION
    // =========================================================

    if (!glfwInit()) {

        std::cerr
            << "ERROR: Failed to initialize GLFW.\n";

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

#ifdef __APPLE__
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GLFW_TRUE
    );
#endif

    GLFWwindow* window =
        glfwCreateWindow(
            WindowWidth,
            WindowHeight,
            "Solarium V0.5",
            nullptr,
            nullptr
        );

    if (window == nullptr) {

        std::cerr
            << "ERROR: Failed to create GLFW window.\n";

        glfwTerminate();

        return 1;
    }

    glfwMakeContextCurrent(window);

    // Enable VSync.
    glfwSwapInterval(1);

    // =========================================================
    // GLAD INITIALIZATION
    // =========================================================

    const int openGLVersion =
        gladLoadGL(
            glfwGetProcAddress
        );

    if (openGLVersion == 0) {

        std::cerr
            << "ERROR: Failed to initialize GLAD.\n";

        glfwDestroyWindow(window);
        glfwTerminate();

        return 1;
    }

    std::cout
        << "OpenGL "
        << GLAD_VERSION_MAJOR(openGLVersion)
        << "."
        << GLAD_VERSION_MINOR(openGLVersion)
        << "\n";

    std::cout
        << "Renderer: "
        << reinterpret_cast<
               const char*
           >(
               glGetString(
                   GL_RENDERER
               )
           )
        << "\n";

    // =========================================================
    // CAMERA
    // =========================================================

    rendering::Camera camera;

    gCamera = &camera;

    glfwSetCursorPosCallback(
        window,
        mouseCallback
    );

    glfwSetScrollCallback(
        window,
        scrollCallback
    );

    // =========================================================
    // SIMULATION
    // =========================================================

    simulation::Simulation simulation;

    // =========================================================
    // RENDERER
    // =========================================================

    rendering::Renderer renderer(
        WindowWidth,
        WindowHeight
    );

    rendering::TrailRenderer trailRenderer;

    // =========================================================
    // ORBIT TRAILS
    // =========================================================

    rendering::OrbitTrail earthTrail(
        2'000
    );

    // =========================================================
    // FRAME TIMING
    // =========================================================

    double lastFrameTime =
        glfwGetTime();

    double fpsTimer =
        lastFrameTime;

    int frameCount = 0;

    double currentFPS = 0.0;

    // =========================================================
    // MAIN LOOP
    // =========================================================

    while (
        !glfwWindowShouldClose(window)
    ) {

        // -----------------------------------------------------
        // Time
        // -----------------------------------------------------

        const double currentTime =
            glfwGetTime();

        double deltaTime =
            currentTime -
            lastFrameTime;

        lastFrameTime =
            currentTime;

        // Prevent an enormous simulation jump if the program
        // is paused by the operating system/debugger.
        if (deltaTime > 0.1) {
            deltaTime = 0.1;
        }

        // -----------------------------------------------------
        // FPS
        // -----------------------------------------------------

        ++frameCount;

        if (
            currentTime -
            fpsTimer >= 1.0
        ) {

            currentFPS =
                static_cast<double>(
                    frameCount
                ) /
                (
                    currentTime -
                    fpsTimer
                );

            frameCount = 0;
            fpsTimer = currentTime;
        }

        // =====================================================
        // INPUT
        // =====================================================

        // -----------------------------------------------------
        // Escape
        // -----------------------------------------------------

        if (
            keyPressed(
                window,
                GLFW_KEY_ESCAPE
            )
        ) {

            glfwSetWindowShouldClose(
                window,
                GLFW_TRUE
            );
        }

        // -----------------------------------------------------
        // Space - Pause / Resume
        // -----------------------------------------------------

        const bool spacePressed =
            keyPressed(
                window,
                GLFW_KEY_SPACE
            );

        if (
            spacePressed &&
            !spaceWasPressed
        ) {

            simulation.togglePause();
        }

        spaceWasPressed =
            spacePressed;

        // -----------------------------------------------------
        // R - Reset
        // -----------------------------------------------------

        const bool rPressed =
            keyPressed(
                window,
                GLFW_KEY_R
            );

        if (
            rPressed &&
            !rWasPressed
        ) {

            simulation.reset();
            earthTrail.clear();
        }

        rWasPressed =
            rPressed;

        // -----------------------------------------------------
        // + / = - Increase speed
        // -----------------------------------------------------

        const bool plusPressed =
            keyPressed(
                window,
                GLFW_KEY_EQUAL
            ) ||
            keyPressed(
                window,
                GLFW_KEY_KP_ADD
            );

        if (
            plusPressed &&
            !plusWasPressed
        ) {

            simulation.increaseSpeed();
        }

        plusWasPressed =
            plusPressed;

        // -----------------------------------------------------
        // - - Decrease speed
        // -----------------------------------------------------

        const bool minusPressed =
            keyPressed(
                window,
                GLFW_KEY_MINUS
            ) ||
            keyPressed(
                window,
                GLFW_KEY_KP_SUBTRACT
            );

        if (
            minusPressed &&
            !minusWasPressed
        ) {

            simulation.decreaseSpeed();
        }

        minusWasPressed =
            minusPressed;

        // =====================================================
        // SIMULATION
        // =====================================================

        simulation.update(
            deltaTime
        );

        // =====================================================
        // ORBIT TRAIL
        // =====================================================

        const auto& bodies =
            simulation.bodies();

        if (bodies.size() > 1) {

            earthTrail.addPoint(
                bodies[1].position()
            );

            earthTrail.upload();
        }

        // =====================================================
        // CAMERA
        // =====================================================

        camera.update(
            deltaTime
        );

        // =====================================================
        // RENDER
        // =====================================================

        renderer.beginFrame();

        // Draw Earth's orbit first.
        trailRenderer.render(
            earthTrail,
            camera
        );

        // Draw celestial bodies.
        for (
            const auto& body :
            bodies
        ) {

            renderer.renderBody(
                body,
                camera
            );
        }

        renderer.endFrame();

        // =====================================================
        // WINDOW
        // =====================================================

        glfwSwapBuffers(window);

        glfwPollEvents();

        // =====================================================
        // WINDOW TITLE / SIMULATION STATUS
        // =====================================================

        std::ostringstream title;

        title
            << "Solarium V0.5 | "
            << (
                simulation.paused()
                    ? "PAUSED"
                    : "RUNNING"
            )
            << " | Time: "
            << formatSimulationTime(
                simulation.simulationTime()
            )
            << " | Speed: "
            << formatTimeScale(
                simulation.timeScale()
            )
            << " | FPS: "
            << std::fixed
            << std::setprecision(1)
            << currentFPS;

        glfwSetWindowTitle(
            window,
            title.str().c_str()
        );
    }

    // =========================================================
    // CLEANUP
    // =========================================================

    gCamera = nullptr;

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
