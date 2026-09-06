#include "solarium/rendering/camera.hpp"
#include "solarium/rendering/body_picker.hpp"
#include "solarium/rendering/orbit_renderer.hpp"
#include "solarium/rendering/ring_renderer.hpp"
#include "solarium/rendering/renderer.hpp"
#include "solarium/rendering/star_field_renderer.hpp"
#include "solarium/rendering/trail_renderer.hpp"
#include "solarium/simulation/simulation.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

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
bool rightMouseDown = false;

// ------------------------------------------------------------
// Keyboard edge detection
// ------------------------------------------------------------

bool spaceWasPressed = false;
bool rWasPressed = false;
bool plusWasPressed = false;
bool minusWasPressed = false;
bool leftMouseWasPressed = false;
bool fWasPressed = false;
bool gWasPressed = false;
bool oWasPressed = false;
bool tWasPressed = false;
bool pWasPressed = false;
bool mWasPressed = false;
bool lWasPressed = false;
bool vWasPressed = false;

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

    if (gCamera != nullptr && rightMouseDown) {
        gCamera->processPan(
            xOffset * 0.01,
            yOffset * 0.01
        );
    } else if (gCamera != nullptr) {
        gCamera->processMouse(
            xOffset,
            yOffset
        );
    }
}

void mouseButtonCallback(
    GLFWwindow* window,
    int button,
    int action,
    int
) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        rightMouseDown = action == GLFW_PRESS;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        firstMouse = true;
        glfwGetCursorPos(window, &previousMouseX, &previousMouseY);
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

const char* bodyTypeName(solarium::celestial::BodyType type) {
    using solarium::celestial::BodyType;
    switch (type) {
        case BodyType::Star: return "Star";
        case BodyType::Planet: return "Planet";
        case BodyType::DwarfPlanet: return "Dwarf Planet";
        case BodyType::Moon: return "Moon";
        case BodyType::Asteroid: return "Asteroid";
        case BodyType::Comet: return "Comet";
    }
    return "Body";
}

solarium::math::Vec3 renderPosition(
    const solarium::math::Vec3& position
) {
    constexpr double au = 1.495978707e11;
    return position / au;
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
            "Solarium V0.6",
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

    glfwSetMouseButtonCallback(
        window,
        mouseButtonCallback
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

    rendering::OrbitRenderer orbitRenderer;
    rendering::StarFieldRenderer starFieldRenderer;
    rendering::RingRenderer ringRenderer;
    rendering::TrailRenderer trailRenderer;

    std::unordered_map<
        std::string,
        std::unique_ptr<rendering::OrbitTrail>
    > trails;
    for (const auto& body : simulation.bodies()) {
        trails.emplace(
            body.name(),
            std::make_unique<rendering::OrbitTrail>(1'200)
        );
    }

    // =========================================================
    // ORBIT TRAILS
    // =========================================================

    // =========================================================
    // FRAME TIMING
    // =========================================================

    double lastFrameTime =
        glfwGetTime();

    double fpsTimer =
        lastFrameTime;

    int frameCount = 0;

    double currentFPS = 0.0;
    std::size_t selectedBody = std::numeric_limits<std::size_t>::max();
    bool followSelected = false;
    bool showOrbits = true;
    bool showPlanetOrbits = true;
    bool showMoonOrbits = true;
    bool selectedOrbitOnly = false;
    bool showTrails = false;
    bool oneWasPressed = false;
    bool twoWasPressed = false;
    bool threeWasPressed = false;

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

        const bool leftMousePressed =
            glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        if (leftMousePressed && !leftMouseWasPressed) {
            double cursorX = 0.0;
            double cursorY = 0.0;
            glfwGetCursorPos(window, &cursorX, &cursorY);
            selectedBody = rendering::BodyPicker::pick(
                simulation.bodies(),
                camera,
                cursorX,
                cursorY,
                WindowWidth,
                WindowHeight
            );
        }
        leftMouseWasPressed = leftMousePressed;

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
            for (auto& [name, trail] : trails) {
                (void)name;
                trail->clear();
            }
            camera.reset();
            followSelected = false;
        }

        if (camera.mode() != rendering::CameraMode::Follow) {
            camera.processMovement(
                (keyPressed(window, GLFW_KEY_W) ? deltaTime : 0.0) -
                (keyPressed(window, GLFW_KEY_S) ? deltaTime : 0.0),
                (keyPressed(window, GLFW_KEY_D) ? deltaTime : 0.0) -
                (keyPressed(window, GLFW_KEY_A) ? deltaTime : 0.0),
                (keyPressed(window, GLFW_KEY_E) ? deltaTime : 0.0) -
                (keyPressed(window, GLFW_KEY_Q) ? deltaTime : 0.0)
            );
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

        const bool fPressed = keyPressed(window, GLFW_KEY_F);
        if (fPressed && !fWasPressed && selectedBody < simulation.bodies().size()) {
            camera.setMode(rendering::CameraMode::Orbit);
            followSelected = false;
            camera.focus(renderPosition(simulation.bodies()[selectedBody].position()));
        }
        fWasPressed = fPressed;

        const bool gPressed = keyPressed(window, GLFW_KEY_G);
        if (gPressed && !gWasPressed && selectedBody < simulation.bodies().size()) {
            followSelected = !followSelected;
            camera.setMode(
                followSelected
                    ? rendering::CameraMode::Follow
                    : rendering::CameraMode::Orbit
            );
        }
        gWasPressed = gPressed;

        const bool oPressed = keyPressed(window, GLFW_KEY_O);
        if (oPressed && !oWasPressed) {
            showOrbits = !showOrbits;
        }
        oWasPressed = oPressed;

        const bool tPressed = keyPressed(window, GLFW_KEY_T);
        if (tPressed && !tWasPressed) {
            showTrails = !showTrails;
        }
        tWasPressed = tPressed;

        const bool pPressed = keyPressed(window, GLFW_KEY_P);
        if (pPressed && !pWasPressed) {
            showPlanetOrbits = !showPlanetOrbits;
        }
        pWasPressed = pPressed;

        const bool mPressed = keyPressed(window, GLFW_KEY_M);
        if (mPressed && !mWasPressed) {
            showMoonOrbits = !showMoonOrbits;
        }
        mWasPressed = mPressed;

        const bool lPressed = keyPressed(window, GLFW_KEY_L);
        if (lPressed && !lWasPressed) {
            selectedOrbitOnly = !selectedOrbitOnly;
        }
        lWasPressed = lPressed;

        const bool vPressed = keyPressed(window, GLFW_KEY_V);
        if (vPressed && !vWasPressed) {
            camera.setMode(
                camera.mode() == rendering::CameraMode::Free
                    ? rendering::CameraMode::Orbit
                    : rendering::CameraMode::Free
            );
            followSelected = false;
        }
        vWasPressed = vPressed;

        const bool onePressed = keyPressed(window, GLFW_KEY_1);
        const bool twoPressed = keyPressed(window, GLFW_KEY_2);
        const bool threePressed = keyPressed(window, GLFW_KEY_3);
        if (onePressed && !oneWasPressed) {
            renderer.setVisualizationMode(rendering::VisualizationMode::Realistic);
        }
        if (twoPressed && !twoWasPressed) {
            renderer.setVisualizationMode(rendering::VisualizationMode::Presentation);
        }
        if (threePressed && !threeWasPressed) {
            renderer.setVisualizationMode(rendering::VisualizationMode::Exploration);
        }
        oneWasPressed = onePressed;
        twoWasPressed = twoPressed;
        threeWasPressed = threePressed;

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

        for (const auto& body : bodies) {
            auto trail = trails.find(body.name());
            if (trail != trails.end()) {
                trail->second->addPoint(body.position());
                trail->second->upload();
            }
        }

        if (followSelected && selectedBody < bodies.size()) {
            camera.setFollowTarget(renderPosition(bodies[selectedBody].position()));
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

        starFieldRenderer.render(camera);

        for (const auto& body : bodies) {
            math::Vec3 parentPosition{};
            if (!body.parentName().empty()) {
                for (const auto& candidate : bodies) {
                    if (candidate.name() == body.parentName()) {
                        parentPosition = candidate.position();
                        break;
                    }
                }
            }
            const bool isMoon = body.type() == celestial::BodyType::Moon;
            const bool selected = selectedBody < bodies.size() &&
                body.name() == bodies[selectedBody].name();
            const bool orbitVisible = showOrbits &&
                (isMoon ? showMoonOrbits : showPlanetOrbits) &&
                (!selectedOrbitOnly || selected);
            orbitRenderer.render(body, camera, parentPosition, orbitVisible);
        }

        if (showTrails) {
            for (const auto& [name, trail] : trails) {
                (void)name;
                trailRenderer.render(*trail, camera);
            }
        }

        // Draw celestial bodies.
        for (
            const auto& body :
            bodies
        ) {

            renderer.renderBody(body, camera, &body ==
                (selectedBody < bodies.size() ? &bodies[selectedBody] : nullptr));

            ringRenderer.render(body, camera);
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
            << "Solarium V0.6 | "
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
            << currentFPS
            << " | Camera: "
            << (camera.mode() == rendering::CameraMode::Follow
                ? "FOLLOW"
                : camera.mode() == rendering::CameraMode::Free ? "FREE" : "ORBIT")
            << " | Orbits: "
            << (showOrbits ? "ON" : "OFF")
            << " | Trails: "
            << (showTrails ? "ON" : "OFF");

        if (selectedBody < bodies.size()) {
            const auto& body = bodies[selectedBody];
            title << " | Selected: " << body.name()
                  << " (" << bodyTypeName(body.type()) << ")"
                  << " | Parent: "
                << (body.parentName().empty() ? "-" : body.parentName())
                << " | Mass: " << std::scientific << body.mass()
                << " | Radius: " << std::fixed << std::setprecision(0)
                << body.radius() / 1'000.0 << " km"
                << " | Period: " << std::setprecision(2)
                << body.orbitalParameters().period / 86'400.0 << " d";
        }

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
